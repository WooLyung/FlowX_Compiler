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

    struct SemanticModel
    {
        SymbolTable symbols;
        std::vector<StructDefinition> structs;
    };
}
