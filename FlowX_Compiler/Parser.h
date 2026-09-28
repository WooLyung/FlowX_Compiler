#pragma once
#include "pch.h"
#include "Token.h"
#include "TypeReference.h"

#include "ProgramNode.h"
#include "StructDeclarationNode.h"
#include "FieldNode.h"
#include "ClassDeclarationNode.h"
#include "FunctionRequirementNode.h"

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

        const flowx::TypeReference TypeReference();
        const TypeName TypeName();
        const TypeReferenceKind PrimitiveType();
        const TypeModifierKind TypeModifier();
        const std::string Identifier();
        std::vector<flowx::TypeReference> TypeList();

        std::span<const Token> tokens_;
        std::size_t position_ = 0;

    public:
        explicit Parser(std::span<const Token> tokens);
        std::unique_ptr<ProgramNode> Parse();
    };
}
