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
        std::vector<std::unique_ptr<ClassDeclarationNode>> classDeclarations;

        while (Peek().kind != TokenKind::EndOfFile)
        {
            switch (Peek().kind)
            {
                case TokenKind::StructKeyword:
                    structDeclarations.push_back(StructDeclaration());
                    break;
                case TokenKind::ClassKeyword:
                    classDeclarations.push_back(ClassDeclaration());
                    break;
                default:
                    const auto token = Peek();
                    throw ParserError(token.location, 
                        "Expected declaration, got " + std::string(TokenKindName(token.kind))
                        + " '" + token.lexeme + "'");
            }
        }
        Expect(TokenKind::EndOfFile);

        std::unique_ptr<ProgramNode> node = std::make_unique<ProgramNode>(location, structDeclarations, classDeclarations);
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

    const flowx::TypeReference Parser::TypeReference()
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
                return { TypeReferenceKind::Named, Identifier() };
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

    const std::string Parser::Identifier()
    {
        std::string identifier = Peek().lexeme;
        Expect(TokenKind::Identifier);
        return identifier;
    }

    std::unique_ptr<ClassDeclarationNode> Parser::ClassDeclaration()
    {
        const SourceLocation location = GetLocation();
        std::vector<std::unique_ptr<FunctionRequirementNode>> requirements;

        Expect(TokenKind::ClassKeyword);
        const std::string identifier = Identifier();
        Expect(TokenKind::LessThan);
        const std::string generic = Identifier();
        Expect(TokenKind::GreaterThan);
        Expect(TokenKind::LeftBrace);
        while (Peek().kind == TokenKind::FnKeyword)
            requirements.push_back(FunctionRequirement());
        Expect(TokenKind::RightBrace);

        std::unique_ptr<ClassDeclarationNode> node = std::make_unique<ClassDeclarationNode>(location, identifier, generic, requirements);
        return node;
    }

    std::unique_ptr<FunctionRequirementNode> Parser::FunctionRequirement()
    {
        const SourceLocation location = GetLocation();

        Expect(TokenKind::FnKeyword);
        const std::string identifier = Identifier();
        Expect(TokenKind::LeftParen);
        std::vector<flowx::TypeReference> inputs = TypeList();
        Expect(TokenKind::RightParen);
        Expect(TokenKind::Arrow);
        Expect(TokenKind::LeftParen);
        std::vector<flowx::TypeReference> outputs = TypeList();
        Expect(TokenKind::RightParen);
        Expect(TokenKind::Semicolon);

        std::unique_ptr<FunctionRequirementNode> node = std::make_unique<FunctionRequirementNode>(location, identifier, inputs, outputs);
        return node;
    }

    std::vector<flowx::TypeReference> Parser::TypeList()
    {
        std::vector<flowx::TypeReference> result;

        if (Peek().kind != TokenKind::PrimitiveType && Peek().kind != TokenKind::Identifier)
            return result;

        result.push_back(TypeReference());
        while (Peek().kind == TokenKind::Comma)
        {
            Expect(TokenKind::Comma);
            result.push_back(TypeReference());
        }

        return result;
    }
}
