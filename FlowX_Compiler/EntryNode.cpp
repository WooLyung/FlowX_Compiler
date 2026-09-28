#include "pch.h"
#include "EntryNode.h"
#include "ExpressionNode.h"

namespace flowx::parser
{
    EntryNode::~EntryNode() = default;

    bool EntryNode::IsTerminal() const
    {
        return isTerminal_;
    }

    const std::string& EntryNode::GetIdentifier() const
    {
        return identifier_;
    }

    const std::vector<std::unique_ptr<ExpressionNode>>& EntryNode::GetExpressions() const
    {
        return expressions_;
    }

    EntryNode::EntryNode(const SourceLocation location, const std::span<std::unique_ptr<ExpressionNode>>& expressions)
        : ParseTreeNode(location), isTerminal_(false), identifier_("")
    {
        this->expressions_.reserve(expressions.size());
        for (auto& expression : expressions)
            this->expressions_.push_back(std::move(expression));
    }

    EntryNode::EntryNode(const SourceLocation location, const std::string& identifier)
        : ParseTreeNode(location), isTerminal_(true), identifier_(identifier)
    {
    }

    std::string_view EntryNode::SymbolName() const noexcept
    {
        return "entry";
    }
}
