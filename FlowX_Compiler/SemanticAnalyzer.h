#pragma once
#include "pch.h"
#include "ProgramNode.h"
#include "SemanticModel.h"
#include "BuiltinFunctions.h"

namespace flowx::semantic
{
    class SemanticError : public std::runtime_error
    {
    private:
        SourceLocation location_;

    public:
        SemanticError(SourceLocation location, const std::string& message);
        SourceLocation Location() const noexcept;
    };

    class SemanticAnalyzer
    {
        friend class L3Graph;

    private:
        inline static const std::vector<std::string_view> reservedNames_ = {
            "_", 
            "pass"
        };

        inline static const std::vector<std::string_view> builtinFunctionNames_ = [] {
            std::vector<std::string_view> names;
            for (const auto& function : GetBuiltinFunctions())
                names.push_back(function.name);
            return names;
        }();

        void RegisterBuiltinFunctions(SemanticModel& model);
        void GenerateMain(SemanticModel& model);
        bool AcceptsType(const TypeReference& required, const TypeReference& actual, const SemanticModel& model) const;
        const FunctionOverloadDefinition& ResolveOverload(const FunctionDefinition& function,
            const std::vector<TypeReference>& inputs, SourceLocation location, const SemanticModel& model) const;
        L3Graph& GenerateL3(unsigned int functionIndex, const FunctionOverloadDefinition& definition,
            const std::vector<TypeReference>& inputs, SourceLocation location, SemanticModel& model);

        void ValidateFunctionName(const std::string& name, SourceLocation location);
        void ValidateName(const std::string& name, SourceLocation location);
        void ResolveFunctions(const parser::ProgramNode& program, SemanticModel& model);
        void ValidateParameters(const std::vector<Parameter>& parameters, const SemanticModel& model, SourceLocation location, const std::string& direction);

        enum class VisitState { Unvisited, Visiting, Complete };

        void RegisterStructs(const parser::ProgramNode& program, SemanticModel& model);
        void ResolveFields(const parser::ProgramNode& program, SemanticModel& model);
        void OrderStructs(SemanticModel& model);
        void VisitStruct(unsigned int id, SemanticModel& model, std::vector<VisitState>& states);

        void RegisterClasses(const parser::ProgramNode& program, SemanticModel& model);
        void ResolveClasses(const parser::ProgramNode& program, SemanticModel& model);
        void ResolveClassTypes(SemanticModel& model);
        bool MatchesRequirement(const FunctionRequirementDefinition& requirement, const FunctionOverloadDefinition& overload,
            const ClassDefinition& definition, const TypeName& type) const;
        void ValidateRequirementType(const TypeReference& type, const ClassDefinition& definition, const SemanticModel& model, SourceLocation location);

    public:
        const SemanticModel Analyze(const parser::ProgramNode& program);
    };
}
