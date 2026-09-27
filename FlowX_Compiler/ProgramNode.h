#pragma once
#include "pch.h"
#include "ParseTreeNode.h"
#include "StructDeclarationNode.h"

namespace flowx::parser
{
    class ProgramNode final : public ParseTreeNode
    {
    private:
        std::vector<std::unique_ptr<StructDeclarationNode>> structDeclarations_;

    public:
        ProgramNode(const SourceLocation location, const std::span<std::unique_ptr<StructDeclarationNode>> declarations);
        std::string_view SymbolName() const noexcept override;

        const std::vector<std::unique_ptr<StructDeclarationNode>>& GetStructDeclarations() const;
    };
}
