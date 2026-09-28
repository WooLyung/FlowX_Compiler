#pragma once
#include "pch.h"
#include "ParseTreeNode.h"
#include "FieldNode.h"

namespace flowx::parser
{
    class StructDeclarationNode final : public ParseTreeNode
    {
    private:
        const std::string identifier_;
        std::vector<std::unique_ptr<FieldNode>> fields_;

    public:
        StructDeclarationNode(const SourceLocation location, const std::string identifier, const std::span<std::unique_ptr<FieldNode>>& fields);
        std::string_view SymbolName() const noexcept override;

        const std::string& GetIdentifier() const;
        const std::vector<std::unique_ptr<FieldNode>>& GetFields() const;
    };
}