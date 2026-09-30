#include "pch.h"
#include "Parser.h"
#include "EntryNode.h"
#include <charconv>

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
        std::vector<std::unique_ptr<FunctionDeclarationNode>> functionDeclarations;

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
                case TokenKind::FnKeyword:
                    functionDeclarations.push_back(FunctionDeclaration());
                    break;
                default:
                    const auto token = Peek();
                    throw ParserError(token.location, 
                        "Expected declaration, got " + std::string(TokenKindName(token.kind))
                        + " '" + token.lexeme + "'");
            }
        }
        Expect(TokenKind::EndOfFile);

        std::unique_ptr<ProgramNode> node = std::make_unique<ProgramNode>(location, structDeclarations, classDeclarations, functionDeclarations);
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

        if (lexeme == "int4")
            return TypeReferenceKind::Int4;
        if (lexeme == "int8")
            return TypeReferenceKind::Int8;
        if (lexeme == "float4")
            return TypeReferenceKind::Float4;
        if (lexeme == "float8")
            return TypeReferenceKind::Float8;
        if (lexeme == "bool")
            return TypeReferenceKind::Bool;
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

    std::unique_ptr<ExpressionNode> Parser::Expression()
    {
        const SourceLocation location = GetLocation();

        std::unique_ptr<EntryNode> entry = Entry();
        std::vector<std::unique_ptr<OperationNode>> operations = Operations();

        std::unique_ptr<ExpressionNode> node = std::make_unique<ExpressionNode>(location, std::move(entry), operations);
        return node;
    }

    std::unique_ptr<EntryNode> Parser::Entry()
    {
        const SourceLocation location = GetLocation();

        if (Peek().kind == TokenKind::Identifier)
            return std::make_unique<EntryNode>(location, Identifier());

        const auto number = [&](auto value)
        {
            const auto& token = Peek();
            const bool wide = token.kind == TokenKind::Int8Literal || token.kind == TokenKind::Float8Literal;
            if (token.lexeme.empty() || (wide && token.lexeme.back() != 'l'))
                throw ParserError(location, "Invalid numeric literal");
            const auto* first = token.lexeme.data();
            const auto* last = first + token.lexeme.size() - (wide ? 1 : 0);
            const auto result = std::from_chars(first, last, value);
            if (result.ec != std::errc{} || result.ptr != last)
                throw ParserError(location, "Numeric literal cannot be represented as its declared type");
            Expect(token.kind);
            return std::make_unique<EntryNode>(location, ConstantValue{ value });
        };
        switch (Peek().kind)
        {
            case TokenKind::BoolLiteral:
            {
                if (Peek().lexeme != "true" && Peek().lexeme != "false")
                    throw ParserError(location, "Invalid boolean literal");
                const bool value = Peek().lexeme == "true";
                Expect(TokenKind::BoolLiteral);
                return std::make_unique<EntryNode>(location, ConstantValue{ value });
            }
            case TokenKind::Int4Literal:
                return number(std::int32_t{});
            case TokenKind::Int8Literal:
                return number(std::int64_t{});
            case TokenKind::Float4Literal:
                return number(float{});
            case TokenKind::Float8Literal:
                return number(double{});
            default:
                break;
        }

        std::vector<std::unique_ptr<ExpressionNode>> expressions;
        Expect(TokenKind::LeftParen);
        
        if (Peek().kind == TokenKind::RightParen)
        {
            Expect(TokenKind::RightParen);
            return std::make_unique<EntryNode>(location, expressions);
        }

        expressions.push_back(Expression());
        while (Peek().kind == TokenKind::Comma)
        {
            Expect(TokenKind::Comma);
            expressions.push_back(Expression());
        }

        Expect(TokenKind::RightParen);
        std::unique_ptr<EntryNode> node = std::make_unique<EntryNode>(location, expressions);
        return node;
    }

    std::vector<std::unique_ptr<OperationNode>> Parser::Operations()
    {
        std::vector<std::unique_ptr<OperationNode>> operations;
        while (Peek().kind == TokenKind::Dot || Peek().kind == TokenKind::Arrow || Peek().kind == TokenKind::Ellipsis || Peek().kind == TokenKind::AsKeyword)
            operations.push_back(Operation());
        return operations;
    }

    std::unique_ptr<ExpressionNode> Parser::ImplicitInputExpression()
    {
        const SourceLocation location = GetLocation();
        std::unique_ptr<EntryNode> entry = std::make_unique<EntryNode>(location, Identifier());
        std::vector<std::unique_ptr<OperationNode>> operations = Operations();
        std::unique_ptr<ExpressionNode> node = std::make_unique<ExpressionNode>(location, std::move(entry), operations, true);
        return node;
    }

    std::vector<std::unique_ptr<ExpressionNode>> Parser::Group()
    {
        std::vector<std::unique_ptr<ExpressionNode>> expressions;

        Expect(TokenKind::LeftBrace);
        expressions.push_back(ImplicitInputExpression());

        while (Peek().kind == TokenKind::Comma)
        {
            Expect(TokenKind::Comma);
            expressions.push_back(ImplicitInputExpression());
        }

        Expect(TokenKind::RightBrace);
        return expressions;
    }

    std::unique_ptr<OperationNode> Parser::Operation()
    {
        const SourceLocation location = GetLocation();
        const TokenKind tokenKind = Peek().kind;

        switch (tokenKind)
        {
            case TokenKind::Dot:
            {
                Expect(TokenKind::Dot);
                return std::make_unique<OperationNode>(location, OperationKind::MemberAccess, Identifier());
            }
            case TokenKind::Arrow:
            {
                Expect(TokenKind::Arrow);
                if (Peek().kind == TokenKind::LeftBrace)
                {
                    std::vector<std::unique_ptr<ExpressionNode>> group = Group();
                    return std::make_unique<OperationNode>(location, OperationKind::Broadcast, group);
                }
                return std::make_unique<OperationNode>(location, OperationKind::Call, Identifier());
            }
            case TokenKind::Ellipsis:
            {
                Expect(TokenKind::Ellipsis);
                std::vector<std::unique_ptr<ExpressionNode>> group = Group();
                return std::make_unique<OperationNode>(location, OperationKind::Distribution, group);
            }
            case TokenKind::AsKeyword:
            {
                Expect(TokenKind::AsKeyword);

                if (Peek().kind != TokenKind::LeftParen)
                    return std::make_unique<OperationNode>(location, OperationKind::Alias, Identifier());
                
                Expect(TokenKind::LeftParen);
                
                std::vector<std::string> identifiers;
                identifiers.push_back(Identifier());
                
                while (Peek().kind == TokenKind::Comma)
                {
                    Expect(TokenKind::Comma);
                    identifiers.push_back(Identifier());
                }
                Expect(TokenKind::RightParen);

                return std::make_unique<OperationNode>(location, OperationKind::Alias, identifiers);
            }
            default:
                throw ParserError(location, "Expected '.', '->', '...' or 'as'");
        }
    }

    std::unique_ptr<FunctionDeclarationNode> Parser::FunctionDeclaration()
    {
        const SourceLocation location = GetLocation();
        std::vector<std::unique_ptr<ExpressionNode>> expressions;

        Expect(TokenKind::FnKeyword);
        const std::string identifier = Identifier();
        Expect(TokenKind::LeftParen);
        std::vector<flowx::Parameter> inputs = ParameterList();
        Expect(TokenKind::RightParen);
        Expect(TokenKind::Arrow);
        Expect(TokenKind::LeftParen);
        std::vector<flowx::Parameter> outputs = ParameterList();
        Expect(TokenKind::RightParen);
        Expect(TokenKind::LeftBrace);
        while (Peek().kind != TokenKind::RightBrace)
        {
            expressions.push_back(Expression());
            Expect(TokenKind::Semicolon);
        }
        Expect(TokenKind::RightBrace);

        std::unique_ptr<FunctionDeclarationNode> node = std::make_unique<FunctionDeclarationNode>(location, identifier, inputs, outputs, expressions);
        return node;
    }

    std::vector<flowx::Parameter> Parser::ParameterList()
    {
        std::vector<flowx::Parameter> result;

        if (Peek().kind != TokenKind::Identifier)
            return result;

        result.push_back(Parameter());
        while (Peek().kind == TokenKind::Comma)
        {
            Expect(TokenKind::Comma);
            result.push_back(Parameter());
        }

        return result;
    }

    flowx::Parameter Parser::Parameter()
    {
        const std::string identifier = Identifier();
        Expect(TokenKind::Colon);
        const flowx::TypeReference type = TypeReference();
        return { identifier, type };
    }
}
