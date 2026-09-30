#pragma once
#include <string>
#include <vector>
#include "TypeReference.h"

namespace flowx
{
    struct BuiltinOverloadDefinition
    {
        std::vector<TypeReference> inputs;
        std::vector<TypeReference> outputs;
        std::string code;
    };

    struct BuiltinFunctionDefinition
    {
        std::string name;
        std::vector<BuiltinOverloadDefinition> overloads;
    };

    inline const std::vector<BuiltinFunctionDefinition>& GetBuiltinFunctions()
    {
        static const std::vector<BuiltinFunctionDefinition> functions = {
            { "add", {
                {
                    { { TypeReferenceKind::Int4, "int4", TypeModifierKind::None },
                      { TypeReferenceKind::Int4, "int4", TypeModifierKind::None } },
                    { { TypeReferenceKind::Int4, "int4", TypeModifierKind::None } },
                    "TESTCODE"
                }
            } }
        };

        return functions;
    }
}
