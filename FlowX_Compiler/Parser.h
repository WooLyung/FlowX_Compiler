#pragma once
#include "pch.h"
#include "Token.h"

#include "ProgramNode.h"
#include "DeclarationNode.h"
#include "StructDeclarationNode.h"

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
        const Token& Peek() const;
        void Expect(TokenKind kind);
        std::unique_ptr<ProgramNode> Program();
        std::unique_ptr<DeclarationNode> Declaration();
        std::unique_ptr<StructDeclarationNode> StructDeclaration();
        void Field();
        void TypeReference();
        void TypeName();
        void PrimitiveType();
        void TypeModifier();
        void Identifier();

        std::span<const Token> tokens_;
        std::size_t position_ = 0;

    public:
        explicit Parser(std::span<const Token> tokens);
        std::unique_ptr<ProgramNode> Parse();
    };
}
