#include "pch.h"
#include "FunctionDeclarationNode.h"

namespace flowx::parser
{
    FunctionDeclarationNode::FunctionDeclarationNode(
        const SourceLocation location,
        const std::string identifier, 
        const std::span<Parameter>& inputs,
        const std::span<Parameter>& outputs, 
        const std::span<std::unique_ptr<ExpressionNode>>& expressions)
        : ParseTreeNode(location), identifier_(identifier)
    {
        this->inputs_.reserve(inputs.size());
        for (auto& input : inputs)
            this->inputs_.push_back(std::move(input));

        this->outputs_.reserve(outputs.size());
        for (auto& output : outputs)
            this->outputs_.push_back(std::move(output));

        this->expressions_.reserve(expressions.size());
        for (auto& expression : expressions)
            this->expressions_.push_back(std::move(expression));
    }

    const std::string& FunctionDeclarationNode::GetIdentifier() const
    {
        return identifier_;
    }

    const std::vector<Parameter>& FunctionDeclarationNode::GetInputs() const
    {
        return inputs_;
    }

    const std::vector<Parameter>& FunctionDeclarationNode::GetOutputs() const
    {
        return outputs_;
    }

    const std::vector<std::unique_ptr<ExpressionNode>>& FunctionDeclarationNode::GetExpressions() const
    {
        return expressions_;
    }

    std::string_view FunctionDeclarationNode::SymbolName() const noexcept
    {
        return "function_declaration";
    }
}
