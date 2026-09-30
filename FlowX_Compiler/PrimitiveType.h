#pragma once
#include "pch.h"

namespace flowx
{
    inline constexpr std::string_view PrimitiveTypeNames[] = {
        "int4", "int8", "float4", "float8", "bool"
    };

    constexpr bool IsPrimitiveType(std::string_view name) noexcept
    {
        for (const auto typeName : PrimitiveTypeNames)
            if (name == typeName)
                return true;
        return false;
    }
}
