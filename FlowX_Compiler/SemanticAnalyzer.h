#pragma once
#include "pch.h"
#include "ProgramNode.h"
#include "SemanticModel.h"

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
    private:
        enum class VisitState { Unvisited, Visiting, Complete };

        void RegisterStructs(const parser::ProgramNode& program, SemanticModel& model);
        void ResolveFields(const parser::ProgramNode& program, SemanticModel& model);
        void OrderStructs(SemanticModel& model);
        void VisitStruct(unsigned int id, SemanticModel& model, std::vector<VisitState>& states);

        void RegisterClasses(const parser::ProgramNode& program, SemanticModel& model);
        void ResolveClasses(const parser::ProgramNode& program, SemanticModel& model);
        void ValidateRequirementType(const TypeReference& type, const ClassDefinition& definition, const SemanticModel& model, SourceLocation location);

    public:
        const SemanticModel Analyze(const parser::ProgramNode& program);
    };
}
