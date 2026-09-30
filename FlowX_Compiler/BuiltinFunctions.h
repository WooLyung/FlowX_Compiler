#pragma once
#include "SemanticModel.h"

namespace flowx::semantic
{
    inline const std::vector<FunctionDefinition>& GetBuiltinFunctions()
    {
        static const std::vector<FunctionDefinition> functions = {
            { "add", {}, {
                { {}, std::nullopt,
                    { { "left", { TypeReferenceKind::Int4, "int4", TypeModifierKind::None } },
                      { "right", { TypeReferenceKind::Int4, "int4", TypeModifierKind::None } } },
                    { { "result", { TypeReferenceKind::Int4, "int4", TypeModifierKind::None } } } }
            } }
        };
        return functions;
    }
}
