#include "pch.h"
#include "CodeGenerator.h"

namespace flowx::codegenerator
{
    CodeGeneratorError::CodeGeneratorError(const std::string& message) : std::runtime_error(message)
    {
    }

    namespace
    {
        constexpr TypeReferenceKind primitiveTypes[] = {
            TypeReferenceKind::Int4,
            TypeReferenceKind::Int8, 
            TypeReferenceKind::Float4, 
            TypeReferenceKind::Float8,
            TypeReferenceKind::Bool,
            TypeReferenceKind::Char
        };
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
            case TypeReferenceKind::Char:
                typeName = "i8";
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

        const std::string text = ir.str();
        std::ofstream output(outputPath, std::ios::binary | std::ios::trunc);
        if (!output)
            throw CodeGeneratorError("Failed to open LLVM output file: " + outputPath.string());
        output << text;
        output.close();
        if (!output)
            throw CodeGeneratorError("Failed to write LLVM output file: " + outputPath.string());
    }

    void CodeGenerator::GenerateTypes(std::ostream& output) const
    {
        for (const auto& primitive : primitiveTypes)
        {
            WriteVariants(output, primitive, "");
            output << '\n';
        }

        for (const auto& definition : model_.structs)
        {
            output << TypeName({ TypeReferenceKind::Named, definition.name }) << " = type { ";
            for (std::size_t index = 0; index < definition.fields.size(); ++index)
            {
                if (index != 0) output << ", ";
                output << TypeName(definition.fields[index].type);
            }

            output << " }\n";
            WriteVariants(output, TypeReferenceKind::Named, definition.name);
            output << '\n';
        }
    }

    void CodeGenerator::WriteVariants(std::ostream& output, const TypeReferenceKind& typeKind, const std::string& lexeme) const
    {
        const std::string baseType = TypeName({ typeKind, lexeme, TypeModifierKind::None });
        output << TypeName({ typeKind, lexeme, TypeModifierKind::Nullable }) << " = type { i1, " << baseType << " }\n";
        output << TypeName({ typeKind, lexeme, TypeModifierKind::Errorable }) << " = type { i1, " << baseType << " }\n";
        output << TypeName({ typeKind, lexeme, TypeModifierKind::NullErrorable }) << " = type { i1, i1, " << baseType << " }\n";
    }
}
