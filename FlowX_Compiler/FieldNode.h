#pragma once
#include "pch.h"
#include "ParseTreeNode.h"
#include "TypeReference.h"

namespace flowx::parser
{
    class FieldNode final : public ParseTreeNode
    {
    private:
        std::string identifier_;
        TypeReference typeReference_;

    public:
        FieldNode(const SourceLocation location, const std::string identifier, const TypeReference typeReference);
        std::string_view SymbolName() const noexcept override;
    };
}