#pragma once
#include "pch.h"
#include "DeclarationNode.h"

namespace flowx::parser
{
    class StructDeclarationNode final : public DeclarationNode
    {
        std::string_view SymbolName() const noexcept override;
    };
}