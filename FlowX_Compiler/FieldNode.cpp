#include "pch.h"
#include "FieldNode.h"

namespace flowx::parser
{
    FieldNode::FieldNode(const SourceLocation location, const std::string identifier, const TypeReference typeReference) 
        : ParseTreeNode(location), identifier_(identifier), typeReference_(typeReference)
    {
    }

    const std::string& FieldNode::GetIdentifier() const
    {
        return identifier_;
    }

    const TypeReference& FieldNode::GetTypeReference() const
    {
        return typeReference_;
    }

    std::string_view FieldNode::SymbolName() const noexcept
    {
        return "field";
    }
}
