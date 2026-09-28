#include "pch.h"
#include "StructDeclarationNode.h"

namespace flowx::parser
{
    StructDeclarationNode::StructDeclarationNode(const SourceLocation location, const std::string identifier, const std::span<std::unique_ptr<FieldNode>>& fields) 
        : ParseTreeNode(location), identifier_(identifier)
    {
        this->fields_.reserve(fields.size());
        for (auto& field : fields)
            this->fields_.push_back(std::move(field));
    }

    const std::string& StructDeclarationNode::GetIdentifier() const
    {
        return identifier_;
    }

    const std::vector<std::unique_ptr<FieldNode>>& StructDeclarationNode::GetFields() const
    {
        return fields_;
    }

    std::string_view StructDeclarationNode::SymbolName() const noexcept
    {
        return "struct_declaration";
    }
}
