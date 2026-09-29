#pragma once
#include "pch.h"
#include "TypeReference.h"
#include <variant>

namespace flowx::semantic
{
    struct TypeTuple
    {
        std::vector<std::variant<TypeReference, TypeTuple>> elements;
    };
}