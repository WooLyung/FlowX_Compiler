#pragma once
#include "pch.h"
#include "ParseTreeNode.h"

namespace flowx::parser
{
    class EntryNode;
    class OperationNode;

    class ExpressionNode final : public ParseTreeNode
    {
    private:
        std::unique_ptr<EntryNode> entry_;
        const bool isImplicitInput_;
        std::vector<std::unique_ptr<OperationNode>> operations_;

    public:
        ExpressionNode(const SourceLocation location, std::unique_ptr<EntryNode> entry,
            const std::span<std::unique_ptr<OperationNode>>& operations, const bool isImplicitInput = false);
        ~ExpressionNode() override;
        std::string_view SymbolName() const noexcept override;

        const std::unique_ptr<EntryNode>& GetEntry() const;
        bool IsImplicitInput() const;
        const std::vector<std::unique_ptr<OperationNode>>& GetOperations() const;
    };
}
