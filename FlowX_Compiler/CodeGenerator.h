#pragma once
#include "pch.h"
#include "SemanticModel.h"

namespace flowx::codegenerator
{
    class CodeGeneratorError : public std::runtime_error
    {
    public:
        explicit CodeGeneratorError(const std::string& message);
    };

    class CodeGenerator
    {
    private:
        const semantic::SemanticModel& model_;

        const std::string TypeName(const TypeReference type) const;
        void WriteVariants(std::ostream& output, const TypeReferenceKind& typeKind, const std::string& lexeme) const;

        void GenerateTypes(std::ostream& output) const;


    public:
        CodeGenerator(const semantic::SemanticModel& model);
        void Generate(const std::filesystem::path& outputPath) const;
    };
}
