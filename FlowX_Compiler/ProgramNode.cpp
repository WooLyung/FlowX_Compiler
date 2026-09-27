#include "pch.h"
#include "ProgramNode.h"

namespace flowx::parser
{
    ProgramNode::ProgramNode(const SourceLocation location, const std::span<std::unique_ptr<StructDeclarationNode>> declarations) : ParseTreeNode(location)
    {
        this->structDeclarations_.reserve(declarations.size());
        for (auto& declaration : declarations)
            this->structDeclarations_.push_back(std::move(declaration));
    }

    const std::vector<std::unique_ptr<StructDeclarationNode>>& ProgramNode::GetStructDeclarations() const
    {
        return structDeclarations_;
    }

    std::string_view ProgramNode::SymbolName() const noexcept
    {
        return "program";
    }
}
