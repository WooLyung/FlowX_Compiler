#include "pch.h"
#include "ProgramNode.h"

namespace flowx::parser
{
    ProgramNode::ProgramNode(const std::span<std::unique_ptr<DeclarationNode>> declarations)
    {
        this->declarations.reserve(declarations.size());
        for (auto& declaration : declarations)
            this->declarations.push_back(std::move(declaration));
    }

    std::string_view ProgramNode::SymbolName() const noexcept
    {
        return "program";
    }
}
