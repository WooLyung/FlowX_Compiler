#pragma once
#include "pch.h"
#include "ParseTreeNode.h"
#include "TypeReference.h"

namespace flowx::parser
{
    class FunctionRequirementNode final : public ParseTreeNode
    {
    private:
        const std::string identifier_;
        std::vector<TypeReference> inputs_;
        std::vector<TypeReference> outputs_;

    public:
        FunctionRequirementNode(const SourceLocation location, const std::string identifier, const std::span<TypeReference> inputs, const std::span<TypeReference> outputs);
        std::string_view SymbolName() const noexcept override;

        const std::string& GetIdentifier() const;
        const std::vector<TypeReference>& GetInputs() const;
        const std::vector<TypeReference>& GetOutputs() const;
    };
}