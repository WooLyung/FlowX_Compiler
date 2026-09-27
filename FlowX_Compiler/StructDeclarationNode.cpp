#include "pch.h"
#include "StructDeclarationNode.h"

namespace flowx::parser
{
    std::string_view StructDeclarationNode::SymbolName() const noexcept
    {
        return "struct_declaration";
    }
}
