#include "pch.h"
#include "SemanticAnalyzer.h"
#include "L1Graph.h"

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
        ResolveFunctions(program, model);
        for (const auto& declaration : program.GetFunctionDeclarations())
        {
            // 함수를 1차 그래프로 생성, 별칭과 DAG 형태 검사
            L1Graph graph(*declaration, model, reservedNames_, builtinFunctionNames_);
        }

        return model;
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
            model.classes.push_back({ name, location, declaration->GetGeneric(), {} });
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
