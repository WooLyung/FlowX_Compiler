#pragma once
#include "pch.h"
#include "ParseTreeNode.h"
#include "DeclarationNode.h"

namespace flowx::parser
{
    class ProgramNode final : public ParseTreeNode
    {
    private:
        std::vector<std::unique_ptr<DeclarationNode>> declarations;

    public:
        ProgramNode(const std::span<std::unique_ptr<DeclarationNode>> declarations);
        std::string_view SymbolName() const noexcept override;
    };
}
