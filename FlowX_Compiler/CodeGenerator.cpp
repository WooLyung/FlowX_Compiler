#include "pch.h"
#include "CodeGenerator.h"
#include "BuiltinFunctions.h"
#include <bit>
#include <iomanip>

namespace flowx::codegenerator
{
    CodeGeneratorError::CodeGeneratorError(const std::string& message) : std::runtime_error(message)
    {
    }

    CodeGenerator::CodeGenerator(const semantic::SemanticModel& model) : model_(model)
    {
    }

    const std::string CodeGenerator::TypeName(const TypeReference type) const
    {
        std::string prefix = "%flowx.primitive.";
        std::string postfix = "";
        std::string typeName;

        switch (type.kind)
        {
            case TypeReferenceKind::Int4:
                typeName = "i32";
                break;
            case TypeReferenceKind::Int8:
                typeName = "i64";
                break;
            case TypeReferenceKind::Float4:
                typeName = "float";
                break;
            case TypeReferenceKind::Float8:
                typeName = "double";
                break;
            case TypeReferenceKind::Bool:
                typeName = "i1";
                break;
            default:
                typeName = type.lexeme;
                prefix = "%flowx.struct.";
        }

        switch (type.modifier)
        {
            case TypeModifierKind::Errorable:
                postfix = ".errorable";
                break;
            case TypeModifierKind::Nullable:
                postfix = ".nullable";
                break;
            case TypeModifierKind::NullErrorable:
                postfix = ".nullerrorable";
                break;
            default:
                if (type.kind != TypeReferenceKind::Named)
                    return typeName;
        }

        return prefix + typeName + postfix;
    }

    void CodeGenerator::Generate(const std::filesystem::path& outputPath) const
    {
        std::ostringstream ir;

        GenerateTypes(ir);
        GenerateFunctionReturnTypes(ir);
        GenerateBuiltinFunctions(ir);
        GenerateFunctions(ir);
        GenerateEntryPoint(ir);

        const std::string text = ir.str();
        std::ofstream output(outputPath, std::ios::binary | std::ios::trunc);
        if (!output)
            throw CodeGeneratorError("Failed to open LLVM output file: " + outputPath.string());
        output << text;
        output.close();
        if (!output)
            throw CodeGeneratorError("Failed to write LLVM output file: " + outputPath.string());
    }

    std::string CodeGenerator::ListTypeName(const TypeReference& type) const
    {
        const auto name = TypeName(type);
        return "%flowx.list." + (name.starts_with("%flowx.") ? name.substr(7) : name);
    }

    std::string CodeGenerator::FunctionName(const std::string& name, const std::vector<TypeReference>& inputs) const
    {
        std::string result = name;
        for (const auto& type : inputs)
        {
            result += "." + type.lexeme;
            switch (type.modifier)
            {
                case TypeModifierKind::Nullable: result += ".nullable"; break;
                case TypeModifierKind::Errorable: result += ".errorable"; break;
                case TypeModifierKind::NullErrorable: result += ".nullerrorable"; break;
                default: break;
            }
        }
        return result;
    }

    std::string CodeGenerator::FunctionName(const semantic::L4Graph& graph) const
    {
        std::map<std::size_t, const TypeReference*> ordered;
        for (const auto& node : graph.GetNodes())
            if (node.kind == semantic::L4NodeKind::Input)
                ordered.emplace(node.index, &node.type.value());
        std::vector<TypeReference> inputs;
        for (const auto& [index, type] : ordered)
            inputs.push_back(*type);
        return FunctionName("user." + model_.functions.at(graph.GetFunctionIndex()).name, inputs);
    }

    void CodeGenerator::GenerateFunctionReturnTypes(std::ostream& output) const
    {
        for (const auto& graph : model_.l4Graphs)
        {
            std::map<std::size_t, const TypeReference*> outputs;
            for (const auto& node : graph->GetNodes())
                if (node.kind == semantic::L4NodeKind::Output)
                    outputs.emplace(node.index, &node.type.value());
            output << "%flowx.return." << FunctionName(*graph) << " = type { ";
            bool first = true;
            for (const auto& [index, type] : outputs)
            {
                if (!first) output << ", ";
                output << ListTypeName(*type);
                first = false;
            }
            output << " }\n";
        }
    }

    void CodeGenerator::GenerateBuiltinFunctions(std::ostream& output) const
    {
        // 사용하는 내장 함수를 생성
        const auto& builtins = GetBuiltinFunctions();

        std::unordered_set<std::string> declarations;
        if (!model_.l4Graphs.empty())
        {
            output << "declare ptr @calloc(i64, i64)\n"
                   << "declare void @free(ptr)\n"
                   << "declare void @llvm.trap()\n";
            declarations.insert("declare void @llvm.trap()");
        }
        for (const auto& builtin : model_.usedBuiltins)
            for (const auto& declaration : builtins[builtin.functionIndex].overloads[builtin.overloadIndex].declarations)
                if (declarations.insert(declaration).second)
                    output << declaration << '\n';

        for (const auto& builtin : model_.usedBuiltins)
        {
            const unsigned int& functionIndex = builtin.functionIndex;
            const std::size_t& overloadIndex = builtin.overloadIndex;

            const auto& def = builtins[functionIndex].overloads[overloadIndex];
            output << '\n' << def.code << '\n';
        }
    }

    void CodeGenerator::GenerateFunctions(std::ostream& output) const
    {
        // 함수 생성
        using semantic::L4Node;
        using semantic::L4NodeKind;

        for (const auto& graph : model_.l4Graphs)
        {
            std::map<const L4Node*, std::map<std::size_t, const L4Node*>> incoming;
            std::map<const L4Node*, std::size_t> remaining;
            std::map<const L4Node*, std::string> values;
            std::map<std::size_t, const L4Node*> inputs, outputs;
            std::deque<const L4Node*> ready;
            std::vector<std::string> arrays;
            for (const auto& node : graph->GetNodes())
            {
                remaining.emplace(&node, 0);
                if (node.kind == L4NodeKind::Input) inputs.emplace(node.index, &node);
                if (node.kind == L4NodeKind::Output) outputs.emplace(node.index, &node);
            }
            for (const auto& node : graph->GetNodes())
                for (const auto& edge : node.edges)
                {
                    if (!incoming[edge.target].emplace(edge.inputIndex, &node).second)
                        throw CodeGeneratorError("Duplicate input edge in L4 graph");
                    ++remaining.at(edge.target);
                }
            for (const auto& node : graph->GetNodes())
                if (remaining.at(&node) == 0) ready.push_back(&node);

            const auto name = FunctionName(*graph);
            const auto returnType = "%flowx.return." + name;
            output << "\ndefine " << returnType << " @flowx." << name << "(";
            for (const auto& [index, node] : inputs)
                output << ListTypeName(*node->type) << " %input" << index << ", ";
            output << "i64 %length";
            for (const auto& [index, node] : outputs)
                output << ", ptr %output" << index;
            output << ") {\nentry:\n";

            std::size_t number = 0;
            std::string block = "entry";
            const auto next = [&]() { return "%v" + std::to_string(++number); };
            const auto callName = [&](const L4Node& node)
            {
                if (node.kind == L4NodeKind::Call)
                {
                    if (!node.function) throw CodeGeneratorError("Missing custom function in L4 call");
                    return FunctionName(*node.function);
                }
                if (!node.builtinIndex) throw CodeGeneratorError("Missing builtin signature in L4 call");
                const auto& reference = model_.usedBuiltins.at(*node.builtinIndex);
                const auto& function = GetBuiltinFunctions().at(reference.functionIndex);
                return FunctionName(function.name, function.overloads.at(reference.overloadIndex).inputs);
            };
            const auto allocate = [&](const TypeReference& type)
            {
                if (arrays.empty()) output << "    %nonempty = icmp ne i64 %length, 0\n";
                const auto data = next(), failed = next(), invalid = next();
                const auto resume = "allocated" + std::to_string(number);
                output << "    " << data << " = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr ("
                       << TypeName(type) << ", ptr null, i64 1) to i64))\n"
                       << "    " << failed << " = icmp eq ptr " << data << ", null\n"
                       << "    " << invalid << " = and i1 " << failed << ", %nonempty\n"
                       << "    br i1 " << invalid << ", label %allocationFailed, label %" << resume << "\n\n"
                       << resume << ":\n";
                block = resume;
                arrays.push_back(data);
                return data;
            };
            const auto makeList = [&](const TypeReference& type, const std::string& data)
            {
                const auto pointer = next(), list = next();
                const auto listType = ListTypeName(type);
                output << "    " << pointer << " = insertvalue " << listType << " poison, ptr " << data << ", 0\n"
                       << "    " << list << " = insertvalue " << listType << " " << pointer << ", i64 %length, 1\n";
                return list;
            };
            const auto loop = [&](auto body)
            {
                const auto suffix = std::to_string(++number);
                const auto check = "check" + suffix, row = "row" + suffix, exit = "exit" + suffix;
                const auto index = next(), following = next(), hasNext = next();
                output << "    br label %" << check << "\n\n" << check << ":\n"
                       << "    " << index << " = phi i64 [ 0, %" << block << " ], [ " << following << ", %" << row << " ]\n"
                       << "    " << hasNext << " = icmp ult i64 " << index << ", %length\n"
                       << "    br i1 " << hasNext << ", label %" << row << ", label %" << exit << "\n\n" << row << ":\n";
                body(index);
                output << "    " << following << " = add i64 " << index << ", 1\n"
                       << "    br label %" << check << "\n\n" << exit << ":\n";
                block = exit;
            };

            std::size_t processed = 0;
            while (!ready.empty())
            {
                const auto* node = ready.front();
                ready.pop_front();
                ++processed;
                std::vector<const L4Node*> arguments;
                for (const auto& [index, source] : incoming[node])
                    arguments.push_back(source);

                switch (node->kind)
                {
                    case L4NodeKind::Input:
                        values.emplace(node, "%input" + std::to_string(node->index));
                        break;
                    case L4NodeKind::Call:
                    case L4NodeKind::BuiltinCall:
                    {
                        std::map<std::size_t, const TypeReference*> results;
                        for (const auto& edge : node->edges)
                            results.emplace(edge.target->index, &edge.target->type.value());
                        std::vector<std::string> buffers;
                        for (const auto& [index, type] : results)
                            buffers.push_back(allocate(*type));
                        const auto result = next();
                        const auto target = callName(*node);
                        output << "    " << result << " = call %flowx.return." << target << " @flowx." << target << "(";
                        bool first = true;
                        for (const auto* argument : arguments)
                        {
                            if (!first) output << ", ";
                            output << ListTypeName(*argument->type) << " " << values.at(argument);
                            first = false;
                        }
                        if (node->kind == L4NodeKind::Call)
                        {
                            if (!first) output << ", ";
                            output << "i64 %length";
                            first = false;
                        }
                        for (const auto& buffer : buffers)
                        {
                            if (!first) output << ", ";
                            output << "ptr " << buffer;
                            first = false;
                        }
                        output << ")\n";
                        values.emplace(node, result);
                        break;
                    }
                    case L4NodeKind::Value:
                    {
                        const auto* source = arguments.at(0);
                        const auto value = next();
                        output << "    " << value << " = extractvalue %flowx.return." << callName(*source)
                               << " " << values.at(source) << ", " << node->index << "\n";
                        values.emplace(node, value);
                        break;
                    }
                    case L4NodeKind::Constant:
                    case L4NodeKind::Construct:
                    case L4NodeKind::MemberAccess:
                    case L4NodeKind::Output:
                    {
                        const auto& type = node->type.value();
                        const auto elementType = TypeName(type);
                        const auto data = node->kind == L4NodeKind::Output ? "%output" + std::to_string(node->index) : allocate(type);
                        std::vector<std::string> pointers;
                        for (const auto* argument : arguments)
                        {
                            const auto pointer = next();
                            output << "    " << pointer << " = extractvalue " << ListTypeName(*argument->type)
                                   << " " << values.at(argument) << ", 0\n";
                            pointers.push_back(pointer);
                        }
                        std::string literal;
                        std::size_t member = 0;
                        if (node->kind == L4NodeKind::Constant)
                        {
                            switch (type.kind)
                            {
                                case TypeReferenceKind::Bool: literal = std::get<bool>(node->constant) ? "true" : "false"; break;
                                case TypeReferenceKind::Int4: literal = std::to_string(std::get<std::int32_t>(node->constant)); break;
                                case TypeReferenceKind::Int8: literal = std::to_string(std::get<std::int64_t>(node->constant)); break;
                                case TypeReferenceKind::Float4:
                                case TypeReferenceKind::Float8:
                                {
                                    const double value = type.kind == TypeReferenceKind::Float4 ? static_cast<double>(std::get<float>(node->constant)) : std::get<double>(node->constant);
                                    std::ostringstream text;
                                    text << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << std::bit_cast<std::uint64_t>(value);
                                    literal = text.str();
                                    break;
                                }
                                default: throw CodeGeneratorError("Invalid constant type");
                            }
                        }
                        if (node->kind == L4NodeKind::MemberAccess)
                        {
                            semantic::Symbol symbol;
                            const auto& sourceType = arguments.at(0)->type.value();
                            if (!model_.symbols.Find(sourceType.lexeme, symbol) || symbol.kind != semantic::SymbolKind::Struct)
                                throw CodeGeneratorError("Invalid structure type '" + sourceType.lexeme + "'");
                            const auto& fields = model_.structs.at(symbol.definitionIndex).fields;
                            while (member < fields.size() && fields[member].name != node->id) ++member;
                            if (member == fields.size()) throw CodeGeneratorError("Unknown structure member '" + node->id + "'");
                        }
                        loop([&](const std::string& index)
                        {
                            std::vector<std::string> elements;
                            for (std::size_t argument = 0; argument < arguments.size(); ++argument)
                            {
                                const auto sourceType = TypeName(*arguments[argument]->type);
                                const auto pointer = next(), value = next();
                                output << "    " << pointer << " = getelementptr " << sourceType << ", ptr " << pointers[argument] << ", i64 " << index << "\n"
                                       << "    " << value << " = load " << sourceType << ", ptr " << pointer << "\n";
                                elements.push_back(value);
                            }
                            std::string value = literal;
                            if (node->kind == L4NodeKind::Output) value = elements.at(0);
                            if (node->kind == L4NodeKind::Construct)
                            {
                                value = "poison";
                                for (std::size_t field = 0; field < elements.size(); ++field)
                                {
                                    const auto result = next();
                                    output << "    " << result << " = insertvalue " << elementType << " " << value << ", "
                                           << TypeName(*arguments[field]->type) << " " << elements[field] << ", " << field << "\n";
                                    value = result;
                                }
                            }
                            if (node->kind == L4NodeKind::MemberAccess)
                            {
                                value = next();
                                output << "    " << value << " = extractvalue " << TypeName(*arguments.at(0)->type)
                                       << " " << elements.at(0) << ", " << member << "\n";
                            }
                            const auto destination = next();
                            output << "    " << destination << " = getelementptr " << elementType << ", ptr " << data << ", i64 " << index << "\n"
                                   << "    store " << elementType << " " << value << ", ptr " << destination << "\n";
                        });
                        values.emplace(node, makeList(type, data));
                        break;
                    }
                    default: throw CodeGeneratorError("Unsupported L4 node kind");
                }
                for (const auto& edge : node->edges)
                    if (--remaining.at(edge.target) == 0) ready.push_back(edge.target);
            }
            if (processed != graph->GetNodes().size())
                throw CodeGeneratorError("Cycle in L4 graph");

            for (const auto& array : arrays)
                output << "    call void @free(ptr " << array << ")\n";
            std::string result = "poison";
            for (const auto& [index, node] : outputs)
            {
                const auto value = next();
                output << "    " << value << " = insertvalue " << returnType << " " << result << ", "
                       << ListTypeName(*node->type) << " " << values.at(node) << ", " << index << "\n";
                result = value;
            }
            output << "    ret " << returnType << " " << result << "\n";
            if (!arrays.empty())
                output << "\nallocationFailed:\n    call void @llvm.trap()\n    unreachable\n";
            output << "}\n";
        }
    }

    void CodeGenerator::GenerateEntryPoint(std::ostream& output) const
    {
        for (const auto& graph : model_.l4Graphs)
        {
            if (model_.functions.at(graph->GetFunctionIndex()).name != "main")
                continue;

            std::map<std::size_t, const TypeReference*> inputs;
            std::map<std::size_t, const TypeReference*> outputs;
            for (const auto& node : graph->GetNodes())
            {
                if (node.kind == semantic::L4NodeKind::Input)
                    inputs.emplace(node.index, &node.type.value());
                if (node.kind == semantic::L4NodeKind::Output)
                    outputs.emplace(node.index, &node.type.value());
            }

            output << "\ndefine dllexport void @flowx_entry(ptr %inputs, i64 %length, ptr %outputs) {\nentry:\n";
            for (const auto& [index, type] : inputs)
            {
                const auto listType = ListTypeName(*type);
                output << "    %inputSlot" << index << " = getelementptr ptr, ptr %inputs, i64 " << index << "\n"
                       << "    %inputData" << index << " = load ptr, ptr %inputSlot" << index << "\n"
                       << "    %inputPointer" << index << " = insertvalue " << listType << " poison, ptr %inputData" << index << ", 0\n"
                       << "    %input" << index << " = insertvalue " << listType << " %inputPointer" << index << ", i64 %length, 1\n";
            }
            for (const auto& [index, type] : outputs)
                output << "    %outputSlot" << index << " = getelementptr ptr, ptr %outputs, i64 " << index << "\n"
                       << "    %output" << index << " = load ptr, ptr %outputSlot" << index << "\n";

            const auto name = FunctionName(*graph);
            output << "    call %flowx.return." << name << " @flowx." << name << "(";
            for (const auto& [index, type] : inputs)
                output << ListTypeName(*type) << " %input" << index << ", ";
            output << "i64 %length";
            for (const auto& [index, type] : outputs)
                output << ", ptr %output" << index;
            output << ")\n    ret void\n}\n";
            return;
        }
        throw CodeGeneratorError("Entry function 'main' has no L4 graph");
    }

    void CodeGenerator::GenerateTypes(std::ostream& output) const
    {
        // 사용하는 타입을 생성
        std::map<std::string, bool> generatedTypes;
        for (const auto& graph : model_.l4Graphs)
            for (const auto& node : graph->GetNodes())
                if (node.type)
                    WriteType(output, *node.type, true, generatedTypes);
    }

    void CodeGenerator::WriteType(std::ostream& output, const TypeReference& type, bool isList, std::map<std::string, bool>& generatedTypes) const
    {
        const auto baseName = TypeName(type);
        const auto name = isList ? ListTypeName(type) : baseName;
        if (!generatedTypes.emplace(name, isList).second)
            return;

        if (isList)
        {
            WriteType(output, type, false, generatedTypes);
            output << name << " = type { ptr, i64 }\n";
            return;
        }

        if (type.modifier != TypeModifierKind::None)
        {
            const TypeReference base = { type.kind, type.lexeme, TypeModifierKind::None };
            WriteType(output, base, false, generatedTypes);
            output << name << " = type { i1, ";
            if (type.modifier == TypeModifierKind::NullErrorable)
                output << "i1, ";
            output << TypeName(base) << " }\n";
            return;
        }

        if (type.kind != TypeReferenceKind::Named)
            return;

        semantic::Symbol symbol;
        if (!model_.symbols.Find(type.lexeme, symbol) || symbol.kind != semantic::SymbolKind::Struct)
            throw CodeGeneratorError("Invalid structure type '" + type.lexeme + "'");

        const auto& definition = model_.structs.at(symbol.definitionIndex);
        for (const auto& field : definition.fields)
            WriteType(output, field.type, false, generatedTypes);

        output << name << " = type { ";
        for (std::size_t index = 0; index < definition.fields.size(); ++index)
        {
            if (index != 0) output << ", ";
            output << TypeName(definition.fields[index].type);
        }
        output << " }\n";
    }
}
