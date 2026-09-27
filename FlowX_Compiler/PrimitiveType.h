#pragma once
#include "stdafx.h"

namespace flowx
{
    inline constexpr std::string_view PrimitiveTypeNames[] = {
        "i4", "i8", "f4", "f8", "b", "c"
    };

    constexpr bool IsPrimitiveType(std::string_view name) noexcept
    {
        for (const auto typeName : PrimitiveTypeNames)
            if (name == typeName)
                return true;
        return false;
    }
}
