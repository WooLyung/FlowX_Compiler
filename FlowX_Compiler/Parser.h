#pragma once
#include "pch.h"
#include "Token.h"
#include "TypeReference.h"

#include "ProgramNode.h"
#include "StructDeclarationNode.h"
#include "FieldNode.h"
#include "ClassDeclarationNode.h"
#include "FunctionRequirementNode.h"
#include "FunctionDeclarationNode.h"
#include "ExpressionNode.h"
#include "OperationNode.h"

namespace flowx::parser
{
    class ParserError : public std::runtime_error
    {
    private:
        SourceLocation location_;

    public:
        ParserError(SourceLocation location, const std::string& message);
        SourceLocation Location() const noexcept;
    };

    class Parser
    {
    private:
        const SourceLocation GetLocation();
        const Token& Peek() const;
        void Expect(TokenKind kind);

        std::unique_ptr<ProgramNode> Program();
        std::unique_ptr<StructDeclarationNode> StructDeclaration();
        std::unique_ptr<FieldNode> Field();
        std::unique_ptr<ClassDeclarationNode> ClassDeclaration();
        std::unique_ptr<FunctionRequirementNode> FunctionRequirement();
        std::unique_ptr<FunctionDeclarationNode> FunctionDeclaration();
        std::unique_ptr<ExpressionNode> Expression();
        std::unique_ptr<EntryNode> Entry();
        std::unique_ptr<ExpressionNode> ImplicitInputExpression();
        std::unique_ptr<OperationNode> Operation();
        std::vector<std::unique_ptr<OperationNode>> Operations();
        std::vector<std::unique_ptr<ExpressionNode>> Group();

        const flowx::TypeReference TypeReference();
        const TypeName TypeName();
        const TypeReferenceKind PrimitiveType();
        const TypeModifierKind TypeModifier();
        const std::string Identifier();
        std::vector<flowx::TypeReference> TypeList();
        std::vector<flowx::Parameter> ParameterList();
        flowx::Parameter Parameter();

        std::span<const Token> tokens_;
        std::size_t position_ = 0;

    public:
        explicit Parser(std::span<const Token> tokens);
        std::unique_ptr<ProgramNode> Parse();
    };
}
