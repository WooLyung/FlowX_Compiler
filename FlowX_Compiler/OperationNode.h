#pragma once
#include "pch.h"
#include "ParseTreeNode.h"

namespace flowx::parser
{
    class ExpressionNode;

    enum class OperationKind 
    {
        MemberAccess, 
        Call, 
        Broadcast,
        Distribution,
        Alias
    };

    enum class OperationTargetKind 
    { 
        Identifier, 
        Group, 
        IdentifierList 
    };

    class OperationNode final : public ParseTreeNode
    {
    private:
        const OperationKind kind_;
        const OperationTargetKind targetKind_;

        const std::string identifier_;
        std::vector<std::unique_ptr<ExpressionNode>> group_;
        std::vector<std::string> identifiers_;

    public:
        OperationNode(const SourceLocation location, const OperationKind kind, const std::string& identifier);
        OperationNode(const SourceLocation location, const OperationKind kind, const std::span<std::unique_ptr<ExpressionNode>>& group);
        OperationNode(const SourceLocation location, const OperationKind kind, const std::span<std::string>& identifiers);
        ~OperationNode() override;
        std::string_view SymbolName() const noexcept override;

        OperationKind GetKind() const;
        OperationTargetKind GetTargetKind() const;
        const std::string& GetIdentifier() const;
        const std::vector<std::unique_ptr<ExpressionNode>>& GetGroup() const;
        const std::vector<std::string>& GetIdentifiers() const;
    };
}
