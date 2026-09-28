#include "pch.h"
#include "ExpressionNode.h"
#include "EntryNode.h"
#include "OperationNode.h"

namespace flowx::parser
{
    ExpressionNode::~ExpressionNode() = default;

    ExpressionNode::ExpressionNode(const SourceLocation location, std::unique_ptr<EntryNode> entry,
        const std::span<std::unique_ptr<OperationNode>>& operations, const bool isImplicitInput)
        : ParseTreeNode(location), entry_(std::move(entry)), isImplicitInput_(isImplicitInput)
    {
        this->operations_.reserve(operations.size());
        for (auto& operation : operations)
            this->operations_.push_back(std::move(operation));
    }

    std::string_view ExpressionNode::SymbolName() const noexcept
    {
        return "expression";
    }

    const std::unique_ptr<EntryNode>& ExpressionNode::GetEntry() const
    {
        return entry_;
    }

    bool ExpressionNode::IsImplicitInput() const
    {
        return isImplicitInput_;
    }

    const std::vector<std::unique_ptr<OperationNode>>& ExpressionNode::GetOperations() const
    {
        return operations_;
    }
}
