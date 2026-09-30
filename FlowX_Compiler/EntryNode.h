#pragma once
#include "pch.h"
#include "ParseTreeNode.h"
#include "ConstantValue.h"

namespace flowx::parser
{
    class ExpressionNode;

    using flowx::ConstantValue;

    class EntryNode final : public ParseTreeNode
    {
    private:
        const bool isTerminal_;
        const std::string identifier_;
        const ConstantValue constant_;
        std::vector<std::unique_ptr<ExpressionNode>> expressions_;

    public:
        EntryNode(const SourceLocation location, const std::span<std::unique_ptr<ExpressionNode>>& expressions);
        EntryNode(const SourceLocation location, const std::string& identifier);
        EntryNode(const SourceLocation location, const ConstantValue& constant);
        ~EntryNode() override;
        std::string_view SymbolName() const noexcept override;
        bool IsTerminal() const;
        bool IsConstant() const;
        const ConstantValue& GetConstant() const;
        const std::string& GetIdentifier() const;
        const std::vector<std::unique_ptr<ExpressionNode>>& GetExpressions() const;
    };
}
