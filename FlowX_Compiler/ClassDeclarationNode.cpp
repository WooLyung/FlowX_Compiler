#include "pch.h"
#include "ClassDeclarationNode.h"

namespace flowx::parser
{
    ClassDeclarationNode::ClassDeclarationNode(
        const SourceLocation location, 
        const std::string identifier, 
        const std::string generic, 
        const std::span<std::unique_ptr<FunctionRequirementNode>>& requirements)
        : ParseTreeNode(location), identifier_(identifier), generic_(generic)
    {
        this->requirements_.reserve(requirements.size());
        for (auto& field : requirements)
            this->requirements_.push_back(std::move(field));
    }

    const std::string& ClassDeclarationNode::GetIdentifier() const
    {
        return identifier_;
    }

    const std::string& ClassDeclarationNode::GetGeneric() const
    {
        return generic_;
    }

    const std::vector<std::unique_ptr<FunctionRequirementNode>>& ClassDeclarationNode::GetRequirements() const
    {
        return requirements_;
    }

    std::string_view ClassDeclarationNode::SymbolName() const noexcept
    {
        return "class_declaration";
    }
}
