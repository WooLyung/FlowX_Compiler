#pragma once
#include "pch.h"

namespace flowx::parser
{
    class ParseTreeNode
    {
    public:
        virtual ~ParseTreeNode() = default;
        virtual std::string_view SymbolName() const noexcept = 0;
    };
}
