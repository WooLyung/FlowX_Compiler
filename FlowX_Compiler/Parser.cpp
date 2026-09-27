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

    const SourceLocation Parser::GetLocation()
    {
        return Peek().location;
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
        const SourceLocation location = GetLocation();

        std::vector<std::unique_ptr<StructDeclarationNode>> structDeclarations;

        while (Peek().kind != TokenKind::EndOfFile)
        {
            switch (Peek().kind)
            {
                case TokenKind::StructKeyword:
                    structDeclarations.push_back(StructDeclaration());
                    break;
                default:
                    const auto token = Peek();
                    throw ParserError(token.location, 
                        "Expected declaration, got " + std::string(TokenKindName(token.kind))
                        + " '" + token.lexeme + "'");
            }
        }
        Expect(TokenKind::EndOfFile);

        std::unique_ptr<ProgramNode> node = std::make_unique<ProgramNode>(location, structDeclarations);
        return node;
    }

    std::unique_ptr<StructDeclarationNode> Parser::StructDeclaration()
    {
        const SourceLocation location = GetLocation();
        std::vector<std::unique_ptr<FieldNode>> fields;

        Expect(TokenKind::StructKeyword);
        const std::string identifier = Identifier();
        Expect(TokenKind::LeftBrace);
        fields.push_back(Field());
        while (Peek().kind == TokenKind::Comma)
        {
            Expect(TokenKind::Comma);
            fields.push_back(Field());
        }
        Expect(TokenKind::RightBrace);

        std::unique_ptr<StructDeclarationNode> node = std::make_unique<StructDeclarationNode>(location, identifier, fields);
        return node;
    }

    std::unique_ptr<FieldNode> Parser::Field()
    {
        const SourceLocation location = GetLocation();

        const std::string identifier = Identifier();
        Expect(TokenKind::Colon);
        const auto typeReference = TypeReference();

        std::unique_ptr<FieldNode> node = std::make_unique<FieldNode>(location, identifier, typeReference);
        return node;
    }

    const TypeReference Parser::TypeReference()
    {
        const auto typeName = TypeName();
        switch (Peek().kind)
        {
            case TokenKind::Question:
            case TokenKind::Bang:
            case TokenKind::QuestionBang:
                return { typeName.kind, typeName.lexeme, TypeModifier() };
            default:
                break;
        }
        return { typeName.kind, typeName.lexeme, TypeModifierKind::None };
    }

    const TypeName Parser::TypeName()
    {
        const std::string lexeme = Peek().lexeme;
        switch (Peek().kind)
        {
            case TokenKind::PrimitiveType:
                return { PrimitiveType(), lexeme };
                break;
            case TokenKind::Identifier:
                return { TypeReferenceKind::Struct, Identifier() };
                break;
            default:
                throw ParserError(Peek().location,
                    "Expected primitive type or type name, got "
                    + std::string(TokenKindName(Peek().kind)));
        }
    }

    const TypeReferenceKind Parser::PrimitiveType()
    {
        const std::string lexeme = Peek().lexeme;
        Expect(TokenKind::PrimitiveType);

        if (lexeme == "i4")
            return TypeReferenceKind::Int4;
        if (lexeme == "i8")
            return TypeReferenceKind::Int8;
        if (lexeme == "f4")
            return TypeReferenceKind::Float4;
        if (lexeme == "f8")
            return TypeReferenceKind::Float8;
        if (lexeme == "b")
            return TypeReferenceKind::Bool;
        if (lexeme == "c")
            return TypeReferenceKind::Char;
        throw ParserError(Peek().location,
            "Expected primitive type, got '"
            + std::string(lexeme) + "'");
    }

    const TypeModifierKind Parser::TypeModifier()
    {
        TypeModifierKind kind = TypeModifierKind::None;
        switch (Peek().kind)
        {
            case TokenKind::Question:
                kind = TypeModifierKind::Nullable;
                Expect(Peek().kind);
                break;
            case TokenKind::Bang:
                kind = TypeModifierKind::Errorable;
                Expect(Peek().kind);
                break;
            case TokenKind::QuestionBang:
                kind = TypeModifierKind::NullErrorable;
                Expect(Peek().kind);
                break;
            default:
                throw ParserError(Peek().location, "Expected '?', '!' or '?!'");
        }
        return kind;
    }

    std::string Parser::Identifier()
    {
        Expect(TokenKind::Identifier);
        return Peek().lexeme;
    }
}
