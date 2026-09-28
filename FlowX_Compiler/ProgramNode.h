#pragma once
#include "pch.h"
#include "ParseTreeNode.h"
#include "StructDeclarationNode.h"
#include "ClassDeclarationNode.h"
#include "FunctionDeclarationNode.h"

namespace flowx::parser
{
    class ProgramNode final : public ParseTreeNode
    {
    private:
        std::vector<std::unique_ptr<StructDeclarationNode>> structDeclarations_;
        std::vector<std::unique_ptr<ClassDeclarationNode>> classDeclarations_;
        std::vector<std::unique_ptr<FunctionDeclarationNode>> functionDeclarations_;

    public:
        ProgramNode(const SourceLocation location, const std::span<std::unique_ptr<StructDeclarationNode>>& structDeclarations, const std::span<std::unique_ptr<ClassDeclarationNode>>& classDeclarations, const std::span<std::unique_ptr<FunctionDeclarationNode>>& functionDeclarations);
        std::string_view SymbolName() const noexcept override;

        const std::vector<std::unique_ptr<StructDeclarationNode>>& GetStructDeclarations() const;
        const std::vector<std::unique_ptr<ClassDeclarationNode>>& GetClassDeclarations() const;
        const std::vector<std::unique_ptr<FunctionDeclarationNode>>& GetFunctionDeclarations() const;
    };
}
