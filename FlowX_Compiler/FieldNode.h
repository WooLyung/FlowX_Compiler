#pragma once
#include "pch.h"
#include "ParseTreeNode.h"
#include "TypeReference.h"

namespace flowx::parser
{
    class FieldNode final : public ParseTreeNode
    {
    private:
        const std::string identifier_;
        const TypeReference typeReference_;

    public:
        FieldNode(const SourceLocation location, const std::string identifier, const TypeReference typeReference);
        std::string_view SymbolName() const noexcept override;

        const std::string& GetIdentifier() const;
        const TypeReference& GetTypeReference() const;
    };
}