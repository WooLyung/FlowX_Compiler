#pragma once
#include "pch.h"
#include "ParseTreeNode.h"

namespace flowx::parser
{
    class DeclarationNode : public ParseTreeNode
    {
    public:
        DeclarationNode(const SourceLocation location) : ParseTreeNode(location) {}
    };
}