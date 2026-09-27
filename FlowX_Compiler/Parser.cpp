#include "pch.h"
#include "Parser.h"

namespace flowx::parser
{
    ParserError::ParserError(SourceLocation location, const std::string& message)
        : std::runtime_error(message), location_(location)
    {
    }

    SourceLocation ParserError::Location() const noexcept
    {
        return location_;
    }

    Parser::Parser(std::span<const Token> tokens) : tokens_(tokens)
    {
        if (tokens_.empty() || tokens_.back().kind != TokenKind::EndOfFile)
            throw std::invalid_argument("Parser requires an EOF-terminated token list");
        for (std::size_t index = 0; index + 1 < tokens_.size(); ++index)
            if (tokens_[index].kind == TokenKind::EndOfFile)
                throw std::invalid_argument("Unexpected EOF inside token list");
    }

    const Token& Parser::Peek() const
    {
        return tokens_[position_];
    }

    void Parser::Expect(TokenKind kind)
    {
        const flowx::Token& token = Peek();
        if (token.kind != kind)
        {
            throw ParserError(token.location,
                "Expected " + std::string(TokenKindName(kind))
                + ", got " + std::string(TokenKindName(token.kind))
                + " '" + token.lexeme + "'");
        }
        if (kind != TokenKind::EndOfFile)
            ++position_;
    }

    std::unique_ptr<ProgramNode> Parser::Parse()
    {
        position_ = 0;
        return Program();
    }

    std::unique_ptr<ProgramNode> Parser::Program()
    {
        std::vector<std::unique_ptr<DeclarationNode>> declarations;
        while (Peek().kind != TokenKind::EndOfFile)
            declarations.push_back(Declaration());
        Expect(TokenKind::EndOfFile);

        std::unique_ptr<ProgramNode> node = std::make_unique<ProgramNode>(declarations);
        return node;
    }

    std::unique_ptr<DeclarationNode> Parser::Declaration()
    {
        // 클래스, 함수 추가 필요
        return StructDeclaration();
    }

    std::unique_ptr<StructDeclarationNode> Parser::StructDeclaration()
    {
        Expect(TokenKind::StructKeyword);
        Identifier();
        Expect(TokenKind::LeftBrace);
        Field();
        while (Peek().kind == TokenKind::Comma)
        {
            Expect(TokenKind::Comma);
            Field();
        }
        Expect(TokenKind::RightBrace);

        std::unique_ptr<StructDeclarationNode> node = std::make_unique<StructDeclarationNode>();
        return node;
    }

    void Parser::Field()
    {
        Identifier();
        Expect(TokenKind::Colon);
        TypeReference();
    }

    void Parser::TypeReference()
    {
        TypeName();
        switch (Peek().kind)
        {
            case TokenKind::Question:
            case TokenKind::Bang:
            case TokenKind::QuestionBang:
                TypeModifier();
                break;
            default:
                break;
        }
    }

    void Parser::TypeName()
    {
        switch (Peek().kind)
        {
            case TokenKind::PrimitiveType:
                PrimitiveType();
                break;
            case TokenKind::Identifier:
                Identifier();
                break;
            default:
                throw ParserError(Peek().location,
                    "Expected primitive type or type name, got "
                    + std::string(TokenKindName(Peek().kind)));
        }
    }

    void Parser::PrimitiveType()
    {
        Expect(TokenKind::PrimitiveType);
    }

    void Parser::TypeModifier()
    {
        switch (Peek().kind)
        {
            case TokenKind::Question:
            case TokenKind::Bang:
            case TokenKind::QuestionBang:
                Expect(Peek().kind);
                break;
            default:
                throw ParserError(Peek().location, "Expected '?', '!' or '?!'");
        }
    }

    void Parser::Identifier()
    {
        Expect(TokenKind::Identifier);
    }
}
