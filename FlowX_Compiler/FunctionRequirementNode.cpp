#include "pch.h"
#include "FunctionRequirementNode.h"

namespace flowx::parser
{
    FunctionRequirementNode::FunctionRequirementNode(const SourceLocation location, const std::string identifier, const std::span<TypeReference> inputs, const std::span<TypeReference> outputs)
        : ParseTreeNode(location), identifier_(identifier)
    {
        this->inputs_.reserve(inputs.size());
        for (auto& input : inputs)
            this->inputs_.push_back(std::move(input));

        this->outputs_.reserve(outputs.size());
        for (auto& output : outputs)
            this->outputs_.push_back(std::move(output));
    }

    const std::string& FunctionRequirementNode::GetIdentifier() const
    {
        return identifier_;
    }

    const std::vector<TypeReference>& FunctionRequirementNode::GetInputs() const
    {
        return inputs_;
    }

    const std::vector<TypeReference>& FunctionRequirementNode::GetOutputs() const
    {
        return outputs_;
    }

    std::string_view FunctionRequirementNode::SymbolName() const noexcept
    {
        return "function_requirement";
    }
}