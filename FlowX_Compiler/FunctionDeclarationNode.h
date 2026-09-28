#pragma once
#include "pch.h"
#include "ParseTreeNode.h"
#include "Parameter.h"
#include "ExpressionNode.h"

namespace flowx::parser
{
    class FunctionDeclarationNode final : public ParseTreeNode
    {
    private:
        const std::string identifier_;
        std::vector<Parameter> inputs_;
        std::vector<Parameter> outputs_;
        std::vector<std::unique_ptr<ExpressionNode>> expressions_;

    public:
        FunctionDeclarationNode(const SourceLocation location, const std::string identifier, const std::span<Parameter>& inputs, const std::span<Parameter>& outputs, const std::span<std::unique_ptr<ExpressionNode>>& expressions);
        std::string_view SymbolName() const noexcept override;

        const std::string& GetIdentifier() const;
        const std::vector<Parameter>& GetInputs() const;
        const std::vector<Parameter>& GetOutputs() const;
        const std::vector<std::unique_ptr<ExpressionNode>>& GetExpressions() const;
    };
}