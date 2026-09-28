#pragma once
#include "pch.h"
#include "ParseTreeNode.h"
#include "FunctionRequirementNode.h"

namespace flowx::parser
{
    class ClassDeclarationNode final : public ParseTreeNode
    {
    private:
        const std::string identifier_;
        const std::string generic_;
        std::vector<std::unique_ptr<FunctionRequirementNode>> requirements_;

    public:
        ClassDeclarationNode(const SourceLocation location, const std::string identifier, const std::string generic, const std::span<std::unique_ptr<FunctionRequirementNode>>& requirements);
        std::string_view SymbolName() const noexcept override;

        const std::string& GetIdentifier() const;
        const std::string& GetGeneric() const;
        const std::vector<std::unique_ptr<FunctionRequirementNode>>& GetRequirements() const;
    };
}