#pragma once
#include <variant>
#include <cstdint>

namespace flowx
{
    using ConstantValue = std::variant<std::monostate, bool, std::int32_t, std::int64_t, float, double>;
}
