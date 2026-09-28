#pragma once
#include "pch.h"
#include "TypeReference.h"

namespace flowx
{
    struct Parameter
    {
        const std::string identifier;
        const TypeReference type;
    };
}