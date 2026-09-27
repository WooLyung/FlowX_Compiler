#pragma once
#include "pch.h"
#include "Token.h"

namespace flowx::parser
{
    class ParseTreeNode
    {
    private:
        const SourceLocation location_;

    public:
        ParseTreeNode(const SourceLocation location) : location_(location) {}
        virtual ~ParseTreeNode() = default;
        virtual std::string_view SymbolName() const noexcept = 0;
    };
}
