#include "pch.h"
#include "CodeGenerator.h"
#include "BuiltinFunctions.h"

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

        const std::string text = ir.str();
        std::ofstream output(outputPath, std::ios::binary | std::ios::trunc);
        if (!output)
            throw CodeGeneratorError("Failed to open LLVM output file: " + outputPath.string());
        output << text;
        output.close();
        if (!output)
            throw CodeGeneratorError("Failed to write LLVM output file: " + outputPath.string());
    }

    void CodeGenerator::GenerateFunctionReturnTypes(std::ostream& output) const
    {
        for (std::size_t index = 0; index < model_.l4Graphs.size(); ++index)
        {
            const auto& graph = *model_.l4Graphs[index];
            std::map<std::size_t, std::string> outputs;
            for (const auto& node : graph.GetNodes())
                if (node.kind == semantic::L4NodeKind::Output)
                {
                    const auto name = TypeName(node.type.value());
                    outputs.emplace(node.index, "%flowx.list." + (name.starts_with("%flowx.") ? name.substr(7) : name));
                }
            std::map<std::size_t, std::string> inputs;
            for (const auto& node : graph.GetNodes())
                if (node.kind == semantic::L4NodeKind::Input)
                {
                    std::string name = node.type->lexeme;
                    switch (node.type->modifier)
                    {
                        case TypeModifierKind::Nullable: name += ".nullable"; break;
                        case TypeModifierKind::Errorable: name += ".errorable"; break;
                        case TypeModifierKind::NullErrorable: name += ".nullerrorable"; break;
                        default: break;
                    }
                    inputs.emplace(node.index, std::move(name));
                }
            output << "%flowx.return.user." << model_.functions[graph.GetFunctionIndex()].name;
            for (const auto& [inputIndex, type] : inputs)
                output << "." << type;
            output << " = type { ";
            bool first = true;
            for (const auto& [outputIndex, type] : outputs)
            {
                if (!first) output << ", ";
                output << type;
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
        const auto name = isList ? "%flowx.list." + (baseName.starts_with("%flowx.") ? baseName.substr(7) : baseName) : baseName;
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
