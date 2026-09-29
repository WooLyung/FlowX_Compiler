#pragma once
#include "pch.h"
#include "SymbolTable.h"
#include "TypeReference.h"
#include "Parameter.h"
#include "L2Graph.h"
#include "L3Graph.h"

namespace flowx::semantic
{
    struct FieldDefinition
    {
        std::string name;
        TypeReference type;
        SourceLocation location;
        std::optional<unsigned int> referencedStruct;
    };

    struct StructDefinition
    {
        std::string name;
        SourceLocation location;
        std::vector<FieldDefinition> fields;
    };

    struct FunctionRequirementDefinition
    {
        std::string name;
        SourceLocation location;
        std::vector<TypeReference> inputs;
        std::vector<TypeReference> outputs;
    };

    struct ClassDefinition
    {
        std::string name;
        SourceLocation location;
        std::string generic;
        std::vector<FunctionRequirementDefinition> requirements;
        std::vector<TypeName> satisfyingTypes;
    };

    struct FunctionOverloadDefinition
    {
        SourceLocation location;
        std::optional<unsigned int> declarationIndex;
        std::vector<Parameter> inputs;
        std::vector<Parameter> outputs;
    };

    struct FunctionDefinition
    {
        std::string name;
        SourceLocation location;
        std::vector<FunctionOverloadDefinition> overloads;
    };

    struct SemanticModel
    {
        SymbolTable symbols;
        std::vector<StructDefinition> structs;
        std::vector<ClassDefinition> classes;
        std::vector<FunctionDefinition> functions;
        std::vector<std::unique_ptr<L2Graph>> l2Graphs;
        std::vector<std::unique_ptr<L3Graph>> l3Graphs;
    };
}
