#include "pch.h"
#include "OperationNode.h"
#include "ExpressionNode.h"

namespace flowx::parser
{
    OperationNode::~OperationNode() = default;

    OperationNode::OperationNode(const SourceLocation location, const OperationKind kind, const std::string& identifier)
        : ParseTreeNode(location), kind_(kind), targetKind_(OperationTargetKind::Identifier), identifier_(identifier)
    {
    }

    OperationNode::OperationNode(const SourceLocation location, const OperationKind kind, const std::span<std::unique_ptr<ExpressionNode>>& group)
        : ParseTreeNode(location), kind_(kind), targetKind_(OperationTargetKind::Group), identifier_("")
    {
        this->group_.reserve(group.size());
        for (auto& expression : group)
            this->group_.push_back(std::move(expression));
    }

    OperationNode::OperationNode(const SourceLocation location, const OperationKind kind, const std::span<std::string>& identifiers)
        : ParseTreeNode(location), kind_(kind), targetKind_(OperationTargetKind::IdentifierList), identifier_("")
    {
        this->identifiers_.reserve(identifiers.size());
        for (auto& identifier : identifiers)
            this->identifiers_.push_back(std::move(identifier));
    }

    std::string_view OperationNode::SymbolName() const noexcept
    {
        return "operation";
    }

    OperationKind OperationNode::GetKind() const
    {
        return kind_;
    }

    OperationTargetKind OperationNode::GetTargetKind() const
    {
        return targetKind_;
    }

    const std::string& OperationNode::GetIdentifier() const
    {
        return identifier_;
    }

    const std::vector<std::unique_ptr<ExpressionNode>>& OperationNode::GetGroup() const
    {
        return group_;
    }

    const std::vector<std::string>& OperationNode::GetIdentifiers() const
    {
        return identifiers_;
    }
}
