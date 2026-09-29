#include "pch.h"
#include "SemanticAnalyzer.h"
#include "L1Graph.h"
#include "L2Graph.h"

namespace flowx::semantic
{
    SemanticError::SemanticError(SourceLocation location, const std::string& message)
        : std::runtime_error(message), location_(location)
    {
    }

    SourceLocation SemanticError::Location() const noexcept
    {
        return location_;
    }

    const SemanticModel SemanticAnalyzer::Analyze(const parser::ProgramNode& program)
    {
        SemanticModel model;

        // 구조체
        RegisterStructs(program, model);
        ResolveFields(program, model);
        OrderStructs(model);

        // 클래스
        RegisterClasses(program, model);
        ResolveClasses(program, model);

        // 함수
        RegisterBuiltinFunctions(model);
        ResolveFunctions(program, model);
        ResolveClassTypes(model);
        for (const auto& declaration : program.GetFunctionDeclarations())
        {
            // 1차 그래프 생성: 별칭 검사, DAG 형태 검사
            L1Graph l1graph(*declaration, model, reservedNames_, builtinFunctionNames_);

            // 2차 그래프 생성: 입출력 연결, 미사용 노드 삭제
            model.l2Graphs.push_back(std::make_unique<L2Graph>(l1graph, *declaration));
        }

        GenerateMain(model);
        return model;
    }

    void SemanticAnalyzer::RegisterBuiltinFunctions(SemanticModel& model)
    {
        for (const auto& function : GetBuiltinFunctions())
        {
            const auto id = static_cast<unsigned int>(model.functions.size());
            if (!model.symbols.Define(function.name, { SymbolKind::Function, id, function.location }))
                throw SemanticError(function.location, "Builtin function name conflicts with a declaration");
            model.functions.push_back(function);
        }
    }

    bool SemanticAnalyzer::AcceptsType(const TypeReference& required, const TypeReference& actual, const SemanticModel& model) const
    {
        if (required.modifier != actual.modifier)
            return false;
        Symbol symbol;
        if (required.kind == TypeReferenceKind::Named && model.symbols.Find(required.lexeme, symbol) && symbol.kind == SymbolKind::Class)
        {
            for (const auto& type : model.classes[symbol.definitionIndex].satisfyingTypes)
                if (type.kind == actual.kind && (type.kind != TypeReferenceKind::Named || type.lexeme == actual.lexeme))
                    return true;
            return false;
        }
        return required.kind == actual.kind && (required.kind != TypeReferenceKind::Named || required.lexeme == actual.lexeme);
    }

    const FunctionOverloadDefinition& SemanticAnalyzer::ResolveOverload(const FunctionDefinition& function, const std::vector<TypeReference>& inputs, SourceLocation location, const SemanticModel& model) const
    {
        const FunctionOverloadDefinition* result = nullptr;
        for (const auto& overload : function.overloads)
        {
            if (overload.inputs.size() != inputs.size())
                continue;
            bool matches = true;
            for (std::size_t index = 0; index < inputs.size(); ++index)
                if (!AcceptsType(overload.inputs[index].type, inputs[index], model))
                {
                    matches = false;
                    break;
                }
            if (!matches)
                continue;
            if (result)
                throw SemanticError(location, "Ambiguous overload for function '" + function.name + "'");
            result = &overload;
        }
        if (!result)
            throw SemanticError(location, "No matching overload for function '" + function.name + "'");
        return *result;
    }

    void SemanticAnalyzer::GenerateMain(SemanticModel& model)
    {
        Symbol symbol;
        if (!model.symbols.Find("main", symbol) || symbol.kind != SymbolKind::Function)
            throw SemanticError({}, "Entry function 'main' is not defined");

        const auto& function = model.functions[symbol.definitionIndex];
        if (function.overloads.size() != 1)
            throw SemanticError(function.location, "Entry function 'main' must have exactly one overload");

        const auto& definition = function.overloads.front();
        for (const auto* parameters : { &definition.inputs, &definition.outputs })
        {
            for (const auto& parameter : *parameters)
            {
                Symbol type;
                if (parameter.type.kind == TypeReferenceKind::Named && model.symbols.Find(parameter.type.lexeme, type) && type.kind == SymbolKind::Class)
                    throw SemanticError(definition.location, "Entry function 'main' cannot use class types");
            }
        }

        std::vector<TypeReference> inputs;
        for (const auto& parameter : definition.inputs)
            inputs.push_back(parameter.type);

        GenerateL3(symbol.definitionIndex, definition, inputs, definition.location, model);
    }

    L3Graph& SemanticAnalyzer::GenerateL3(unsigned int functionIndex, const FunctionOverloadDefinition& definition, const std::vector<TypeReference>& inputs, SourceLocation location, SemanticModel& model)
    {
        for (const auto& graph : model.l3Graphs)
        {
            if (graph->functionIndex_ != functionIndex || graph->declarationIndex_ != definition.declarationIndex || graph->inputs_.size() != inputs.size())
                continue;

            bool matches = true;
            for (std::size_t index = 0; index < inputs.size(); ++index)
            {
                if (!AcceptsType(graph->inputs_[index], inputs[index], model))
                {
                    matches = false;
                    break;
                }
            }

            if (!matches)
                continue;

            if (!graph->complete_)
                throw SemanticError(location, "Recursive call to function '" + model.functions[functionIndex].name + "' with the same input types");
            
            return *graph;
        }

        auto graph = std::make_unique<L3Graph>(functionIndex, definition.declarationIndex.value(), inputs);
        auto& result = *graph;
        model.l3Graphs.push_back(std::move(graph));

        result.Build(*model.l2Graphs.at(definition.declarationIndex.value()), definition, *this, model);
        result.complete_ = true;

        return result;
    }

    void SemanticAnalyzer::RegisterStructs(const parser::ProgramNode& program, SemanticModel& model)
    {
        // 구조체 등록 및 식별자 중복 검사
        for (const auto& declaration : program.GetStructDeclarations())
        {
            const auto& name = declaration->GetIdentifier();
            const auto location = declaration->GetLocation();

            ValidateName(name, location);
            Symbol existing;
            if (model.symbols.Find(name, existing))
            {
                throw SemanticError(location, "Duplicate declaration '" + name
                    + "' (first declared at " + std::to_string(existing.location.line)
                    + ":" + std::to_string(existing.location.column) + ")");
            }

            const unsigned int id = (unsigned int)model.structs.size();
            model.symbols.Define(name, { SymbolKind::Struct, id, location });
            model.structs.push_back({ name, location, {} });
        }
    }

    void SemanticAnalyzer::ResolveFields(const parser::ProgramNode& program, SemanticModel& model)
    {
        const auto& declarations = program.GetStructDeclarations();

        for (unsigned int id = 0; id < declarations.size(); id++)
        {
            auto& definition = model.structs[id];

            std::unordered_set<std::string> fieldNames;
            for (const auto& field : declarations[id]->GetFields())
            {
                const auto& name = field->GetIdentifier();
                const auto& type = field->GetTypeReference();
                const auto location = field->GetLocation();
                ValidateName(name, location);

                // 중복된 필드 검사
                if (!fieldNames.insert(name).second)
                    throw SemanticError(location, "Duplicate field '" + definition.name + "." + name + "'");

                // 필드에서 error 금지
                if (type.modifier != TypeModifierKind::None && type.modifier != TypeModifierKind::Nullable)
                    throw SemanticError(location, "Field '" + definition.name + "." + name + "' cannot use '!' or '?!'");

                // 알 수 없는 구조체 검사
                std::optional<unsigned int> referencedStruct;
                if (type.kind == TypeReferenceKind::Named)
                {
                    Symbol symbol{};
                    if (!model.symbols.Find(type.lexeme, symbol) || symbol.kind != SymbolKind::Struct)
                        throw SemanticError(location, "Unknown structure type '" + type.lexeme + "' in field '" + definition.name + "." + name + "'");
                    referencedStruct = symbol.definitionIndex;
                }

                definition.fields.push_back({ name, type, location, referencedStruct });
            }
        }
    }

    void SemanticAnalyzer::RegisterClasses(const parser::ProgramNode& program, SemanticModel& model)
    {
        // 클래스 등록 및 식별자 중복 검사
        for (const auto& declaration : program.GetClassDeclarations())
        {
            const auto& name = declaration->GetIdentifier();
            const auto location = declaration->GetLocation();
            ValidateName(name, location);
            ValidateName(declaration->GetGeneric(), location);

            Symbol existing;
            if (model.symbols.Find(name, existing))
                throw SemanticError(location, "Duplicate declaration '" + name + "'");

            const unsigned int id = static_cast<unsigned int>(model.classes.size());
            model.symbols.Define(name, { SymbolKind::Class, id, location });
            model.classes.push_back({ name, location, declaration->GetGeneric(), {}, {} });
        }
    }

    void SemanticAnalyzer::ValidateRequirementType(const TypeReference& type, const ClassDefinition& definition, const SemanticModel& model, SourceLocation location)
    {
        // 클래스의 함수 시그니처 입출력 검사
        if (type.kind != TypeReferenceKind::Named || type.lexeme == definition.generic)
            return;

        Symbol symbol;
        if (!model.symbols.Find(type.lexeme, symbol) || symbol.kind != SymbolKind::Struct)
            throw SemanticError(location, "Invalid requirement type '" + type.lexeme + "' in class '" + definition.name + "'");
    }

    void SemanticAnalyzer::ResolveClasses(const parser::ProgramNode& program, SemanticModel& model)
    {
        const auto& declarations = program.GetClassDeclarations();

        for (std::size_t id = 0; id < declarations.size(); ++id)
        {
            auto& definition = model.classes[id];

            // 제네릭이 사용중인 식별자인지 검사
            Symbol symbol;
            if (model.symbols.Find(definition.generic, symbol))
                throw SemanticError(definition.location, "Generic parameter '" + definition.generic + "' conflicts");

            // 빈 클래스인지 검사
            const auto& requirements = declarations[id]->GetRequirements();
            if (requirements.empty())
                throw SemanticError(definition.location, "Class '" + definition.name + "' has no requirements");

            for (const auto& requirement : requirements)
            {
                const auto& name = requirement->GetIdentifier();
                const auto location = requirement->GetLocation();
                ValidateFunctionName(name, location);
                if (name == definition.generic ||
                    (model.symbols.Find(name, symbol) &&
                        (symbol.kind == SymbolKind::Struct || symbol.kind == SymbolKind::Class)))
                    throw SemanticError(location, "Requirement name '" + name + "' conflicts with a type name");

                const auto& inputs = requirement->GetInputs();
                const auto& outputs = requirement->GetOutputs();

                // 출력 없는 함수 검사
                if (outputs.empty())
                    throw SemanticError(location, "Requirement '" + name + "' must have at least one output");

                for (const auto& type : inputs)
                    ValidateRequirementType(type, definition, model, location);
                for (const auto& type : outputs)
                    ValidateRequirementType(type, definition, model, location);

                // 중복된 입력을 가진 함수 검사
                for (const auto& previous : definition.requirements)
                {
                    if (previous.name != name || previous.inputs.size() != inputs.size())
                        continue;

                    bool flag = true;
                    for (std::size_t index = 0; index < inputs.size(); ++index)
                    {
                        const auto& left = previous.inputs[index];
                        const auto& right = inputs[index];
                        if (left.kind != right.kind || left.modifier != right.modifier || (left.kind == TypeReferenceKind::Named && left.lexeme != right.lexeme))
                        {
                            flag = false;
                            break;
                        }
                    }

                    if (flag)
                        throw SemanticError(location, "Duplicate input signature for requirement '" + name + "'");
                }

                definition.requirements.push_back({ name, location, inputs, outputs });
            }
        }
    }

    bool SemanticAnalyzer::MatchesRequirement(const FunctionRequirementDefinition& requirement,
        const FunctionOverloadDefinition& overload, const ClassDefinition& definition, const TypeName& type) const
    {
        if (requirement.inputs.size() != overload.inputs.size() || requirement.outputs.size() != overload.outputs.size())
            return false;

        const auto matches = [&](const TypeReference& required, const TypeReference& actual)
        {
            const bool generic = required.kind == TypeReferenceKind::Named && required.lexeme == definition.generic;
            const auto kind = generic ? type.kind : required.kind;
            const auto& name = generic ? type.lexeme : required.lexeme;
            return actual.kind == kind && actual.modifier == required.modifier &&
                (kind != TypeReferenceKind::Named || actual.lexeme == name);
        };

        for (std::size_t index = 0; index < requirement.inputs.size(); ++index)
            if (!matches(requirement.inputs[index], overload.inputs[index].type))
                return false;

        for (std::size_t index = 0; index < requirement.outputs.size(); ++index)
            if (!matches(requirement.outputs[index], overload.outputs[index].type))
                return false;

        return true;
    }

    void SemanticAnalyzer::ResolveClassTypes(SemanticModel& model)
    {
        const auto& structs = model.structs;
        std::vector<TypeName> types = {
            { TypeReferenceKind::Int4, "i4" },
            { TypeReferenceKind::Int8, "i8" },
            { TypeReferenceKind::Float4, "f4" },
            { TypeReferenceKind::Float8, "f8" },
            { TypeReferenceKind::Bool, "b" },
            { TypeReferenceKind::Char, "c" }
        };

        for (const auto& definition : structs)
            types.push_back({ TypeReferenceKind::Named, definition.name });

        for (auto& definition : model.classes)
        {
            definition.satisfyingTypes.clear();
            for (const auto& type : types)
            {
                bool satisfied = true;
                for (const auto& requirement : definition.requirements)
                {
                    Symbol symbol;
                    if (!model.symbols.Find(requirement.name, symbol) || symbol.kind != SymbolKind::Function)
                    {
                        satisfied = false;
                        break;
                    }

                    bool matched = false;
                    for (const auto& overload : model.functions[symbol.definitionIndex].overloads)
                    {
                        if (MatchesRequirement(requirement, overload, definition, type))
                        {
                            matched = true;
                            break;
                        }
                    }

                    if (!matched)
                    {
                        satisfied = false;
                        break;
                    }
                }
                if (satisfied)
                    definition.satisfyingTypes.push_back(type);
            }
        }
    }

    void SemanticAnalyzer::ValidateName(const std::string& name, SourceLocation location)
    {
        ValidateFunctionName(name, location);
        for (const auto builtin : builtinFunctionNames_)
            if (name == builtin)
                throw SemanticError(location, "Builtin function name '" + name + "' cannot be used as a declaration name");
    }

    void SemanticAnalyzer::ValidateFunctionName(const std::string& name, SourceLocation location)
    {
        // 예약어인지 확인
        for (const auto reserved : reservedNames_)
            if (name == reserved)
                throw SemanticError(location, "Reserved name '" + name + "' cannot be declared");
    }

    void SemanticAnalyzer::ResolveFunctions(const parser::ProgramNode& program, SemanticModel& model)
    {
        const auto& declarations = program.GetFunctionDeclarations();

        for (std::size_t index = 0; index < declarations.size(); ++index)
        {
            const auto& declaration = declarations[index];
            const auto& name = declaration->GetIdentifier();
            const auto location = declaration->GetLocation();
            ValidateFunctionName(name, location);

            Symbol symbol;
            const bool exists = model.symbols.Find(name, symbol);
            if (exists && symbol.kind != SymbolKind::Function)
                throw SemanticError(location, "Function name '" + name + "' conflicts with a type name");

            const auto& inputs = declaration->GetInputs();
            const auto& outputs = declaration->GetOutputs();
            
            if (outputs.empty())
                throw SemanticError(location, "Function '" + name + "' must have at least one output");

            ValidateParameters(inputs, model, location, "input");
            ValidateParameters(outputs, model, location, "output");

            unsigned int id;
            if (exists)
                id = symbol.definitionIndex;
            else
            {
                id = static_cast<unsigned int>(model.functions.size());
                model.symbols.Define(name, { SymbolKind::Function, id, location });
                model.functions.push_back({ name, location, {} });
            }

            auto& definition = model.functions[id];
            for (const auto& previous : definition.overloads)
            {
                if (previous.inputs.size() != inputs.size())
                    continue;

                bool flag = true;
                for (std::size_t parameter = 0; parameter < inputs.size(); ++parameter)
                {
                    const auto& left = previous.inputs[parameter].type;
                    const auto& right = inputs[parameter].type;
                    if (left.kind != right.kind || left.modifier != right.modifier || (left.kind == TypeReferenceKind::Named && left.lexeme != right.lexeme))
                    {
                        flag = false;
                        break;
                    }
                }

                if (flag)
                    throw SemanticError(location, "Duplicate input signature for function '" + name + "'");
            }

            definition.overloads.push_back({ location, static_cast<unsigned int>(index), inputs, outputs });
        }
    }

    void SemanticAnalyzer::ValidateParameters(const std::vector<Parameter>& parameters, const SemanticModel& model, SourceLocation location, const std::string& direction)
    {
        // 함수 입출력 파라미터 검사
        std::unordered_set<std::string> names;
        for (const auto& parameter : parameters)
        {
            ValidateName(parameter.identifier, location);

            if (!names.insert(parameter.identifier).second)
                throw SemanticError(location, "Duplicate " + direction + " parameter '" + parameter.identifier + "'");

            if (parameter.type.kind == TypeReferenceKind::Named)
            {
                Symbol symbol;
                if (!model.symbols.Find(parameter.type.lexeme, symbol) || (symbol.kind != SymbolKind::Struct && symbol.kind != SymbolKind::Class))
                    throw SemanticError(location, "Unknown " + direction + " type '" + parameter.type.lexeme + "'");
            }
        }
    }

    void SemanticAnalyzer::OrderStructs(SemanticModel& model)
    {
        // 구조체 순환 참조 검사
        std::vector<VisitState> states(model.structs.size(), VisitState::Unvisited);
        for (unsigned int id = 0; id < model.structs.size(); ++id)
            if (states[id] == VisitState::Unvisited)
                VisitStruct(id, model, states);
    }

    void SemanticAnalyzer::VisitStruct(unsigned int id, SemanticModel& model, std::vector<VisitState>& states)
    {
        states[id] = VisitState::Visiting;

        for (const auto& field : model.structs[id].fields)
        {
            if (!field.referencedStruct)
                continue;

            const unsigned int dependency = *field.referencedStruct;
            if (states[dependency] == VisitState::Visiting)
                throw SemanticError(field.location, "Exist cyclic structure for " + model.structs[id].name);
            if (states[dependency] == VisitState::Unvisited)
                VisitStruct(dependency, model, states);
        }

        states[id] = VisitState::Complete;
    }
}
