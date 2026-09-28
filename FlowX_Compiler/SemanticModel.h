#pragma once
#include "pch.h"
#include "SymbolTable.h"
#include "TypeReference.h"

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
    };

    struct SemanticModel
    {
        SymbolTable symbols;
        std::vector<StructDefinition> structs;
        std::vector<ClassDefinition> classes;
    };
}
