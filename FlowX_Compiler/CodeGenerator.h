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
        std::string ListTypeName(const TypeReference& type) const;
        std::string FunctionName(const std::string& name, const std::vector<TypeReference>& inputs) const;
        std::string FunctionName(const semantic::L4Graph& graph) const;
        void GenerateFunctions(std::ostream& output) const;
        void WriteType(std::ostream& output, const TypeReference& type, bool isList, std::map<std::string, bool>& generatedTypes) const;
        void GenerateTypes(std::ostream& output) const;
        void GenerateBuiltinFunctions(std::ostream& output) const;
        void GenerateFunctionReturnTypes(std::ostream& output) const;

    public:
        CodeGenerator(const semantic::SemanticModel& model);
        void Generate(const std::filesystem::path& outputPath) const;
    };
}
