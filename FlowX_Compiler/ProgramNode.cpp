#include "pch.h"
#include "ProgramNode.h"

namespace flowx::parser
{
    ProgramNode::ProgramNode(const SourceLocation location, 
        const std::span<std::unique_ptr<StructDeclarationNode>>& structDeclarations, 
        const std::span<std::unique_ptr<ClassDeclarationNode>>& classDeclarations,
        const std::span<std::unique_ptr<FunctionDeclarationNode>>& functionDeclarations) 
        : ParseTreeNode(location)
    {
        this->structDeclarations_.reserve(structDeclarations.size());
        for (auto& declaration : structDeclarations)
            this->structDeclarations_.push_back(std::move(declaration));

        this->classDeclarations_.reserve(classDeclarations.size());
        for (auto& declaration : classDeclarations)
            this->classDeclarations_.push_back(std::move(declaration));

        this->functionDeclarations_.reserve(functionDeclarations.size());
        for (auto& declaration : functionDeclarations)
            this->functionDeclarations_.push_back(std::move(declaration));
    }

    const std::vector<std::unique_ptr<StructDeclarationNode>>& ProgramNode::GetStructDeclarations() const
    {
        return structDeclarations_;
    }

    const std::vector<std::unique_ptr<ClassDeclarationNode>>& ProgramNode::GetClassDeclarations() const
    {
        return classDeclarations_;
    }

    const std::vector<std::unique_ptr<FunctionDeclarationNode>>& ProgramNode::GetFunctionDeclarations() const
    {
        return functionDeclarations_;
    }

    std::string_view ProgramNode::SymbolName() const noexcept
    {
        return "program";
    }
}
