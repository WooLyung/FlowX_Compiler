#include "pch.h"
#include "ProgramNode.h"

namespace flowx::parser
{
    ProgramNode::ProgramNode(const SourceLocation location, const std::span<std::unique_ptr<StructDeclarationNode>> declarations) : ParseTreeNode(location)
    {
        this->declarations_.reserve(declarations.size());
        for (auto& declaration : declarations)
            this->declarations_.push_back(std::move(declaration));
    }

    std::string_view ProgramNode::SymbolName() const noexcept
    {
        return "program";
    }
}
