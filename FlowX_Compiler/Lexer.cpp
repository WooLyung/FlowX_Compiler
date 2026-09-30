#include "pch.h"
#include "Lexer.h"
#include "PrimitiveType.h"
#include <charconv>
#include <cstdint>

namespace flowx::lexer
{
    LexerError::LexerError(SourceLocation location, const std::string& message)
        : std::runtime_error(message), location_(location)
    {
    }

    SourceLocation LexerError::Location() const noexcept
    {
        return location_;
    }

    Lexer::Lexer(std::string source) : source_(std::move(source))
    {
    }

    bool Lexer::AtEnd() const noexcept
    {
        return position_ == source_.size();
    }

    char Lexer::Peek() const noexcept
    {
        return AtEnd() ? '\0' : source_[position_];
    }

    void Lexer::Advance()
    {
        const char character = source_[position_++];
        if (character == '\r')
        {
            if (!AtEnd() && Peek() == '\n')
                position_++;
            location_.line++;
            location_.column = 1;
        }
        else if (character == '\n')
        {
            location_.line++;
            location_.column = 1;
        }
        else
        {
            location_.column++;
        }
    }

    void Lexer::SkipWhitespace()
    {
        while (!AtEnd())
        {
            if (std::isspace(Peek()))
            {
                Advance();
                continue;
            }

            if (Peek() == '/' && source_.size() - position_ >= 2 && source_[position_ + 1] == '/')
            {
                while (!AtEnd() && Peek() != '\r' && Peek() != '\n')
                    Advance();
                continue;
            }

            break;
        }
    }

    Token Lexer::ReadIdentifier()
    {
        const auto start = position_;
        const auto location = location_;
        while (!AtEnd() && IsIdentifierContinuation(Peek()))
            Advance();

        auto lexeme = source_.substr(start, position_ - start);
        auto kind = TokenKind::Identifier;
        if (lexeme == "struct")
            kind = TokenKind::StructKeyword;
        else if (lexeme == "class")
            kind = TokenKind::ClassKeyword;
        else if (lexeme == "fn")
            kind = TokenKind::FnKeyword;
        else if (lexeme == "as")
            kind = TokenKind::AsKeyword;
        else if (lexeme == "true" || lexeme == "false")
            kind = TokenKind::BoolLiteral;
        else if (IsPrimitiveType(lexeme))
            kind = TokenKind::PrimitiveType;

        return { kind, std::move(lexeme), location };
    }

    Token Lexer::ReadNumber()
    {
        const auto start = position_;
        const auto location = location_;
        if (Peek() == '-')
            Advance();
        while (Peek() >= '0' && Peek() <= '9')
            Advance();

        bool floating = false;
        if (Peek() == '.' && source_.size() - position_ >= 2 && source_[position_ + 1] >= '0' && source_[position_ + 1] <= '9')
        {
            floating = true;
            Advance();
            while (Peek() >= '0' && Peek() <= '9')
                Advance();
        }
        bool wide = Peek() == 'l';
        if (wide)
            Advance();
        if (IsIdentifierContinuation(Peek()))
            throw LexerError(location, "Invalid numeric literal suffix");

        const auto kind = floating ? (wide ? TokenKind::Float8Literal : TokenKind::Float4Literal) : (wide ? TokenKind::Int8Literal : TokenKind::Int4Literal);
        const auto lexeme = source_.substr(start, position_ - start);
        const auto* first = lexeme.data();
        const auto* last = first + lexeme.size() - (wide ? 1 : 0);
        const auto validate = [&](auto value)
        {
            const auto result = std::from_chars(first, last, value);
            if (result.ec != std::errc{} || result.ptr != last)
            {
                const auto type = floating ? (wide ? "float8" : "float4") : (wide ? "int8" : "int4");
                throw LexerError(location, "Numeric literal '" + lexeme + "' cannot be represented as " + type);
            }
        };
        if (floating)
        {
            if (wide) validate(double{});
            else validate(float{});
        }
        else
        {
            if (wide) validate(std::int64_t{});
            else validate(std::int32_t{});
        }
        return { kind, lexeme, location };
    }

    std::vector<Token> Lexer::Tokenize()
    {
        std::vector<Token> tokens;
        while (true)
        {
            Token nextToken = NextToken();
            tokens.push_back(nextToken);
            if (nextToken.kind == TokenKind::EndOfFile)
                break;
        }
        return tokens;
    }

    Token Lexer::NextToken()
    {
        SkipWhitespace();
        if (AtEnd())
            return { TokenKind::EndOfFile, "", location_ };
        if ((Peek() >= '0' && Peek() <= '9') ||
            (Peek() == '-' && source_.size() - position_ >= 2 && source_[position_ + 1] >= '0' && source_[position_ + 1] <= '9'))
            return ReadNumber();
        if (IsIdentifierStart(Peek()))
            return ReadIdentifier();

        const auto location = location_;
        const char character = Peek();
        TokenKind kind;
        switch (character)
        {
            case '{': 
                kind = TokenKind::LeftBrace;  
                break;
            case '}': 
                kind = TokenKind::RightBrace; 
                break;
            case ':': 
                kind = TokenKind::Colon;      
                break;
            case ',': 
                kind = TokenKind::Comma;      
                break;
            case '?':
                Advance();
                if (Peek() == '!')
                {
                    Advance();
                    return { TokenKind::QuestionBang, "?!", location };
                }
                return { TokenKind::Question, "?", location };
            case '!':
                kind = TokenKind::Bang;
                break;
            case '<':
                kind = TokenKind::LessThan;
                break;
            case '>':
                kind = TokenKind::GreaterThan;
                break;
            case '(':
                kind = TokenKind::LeftParen;
                break;
            case ')':
                kind = TokenKind::RightParen;
                break;
            case ';':
                kind = TokenKind::Semicolon;
                break;
            case '-':
                Advance();
                if (Peek() != '>')
                    throw LexerError(location, "Expected '>' after '-' for '->'");
                Advance();
                return { TokenKind::Arrow, "->", location };
            case '.':
                if (source_.size() - position_ >= 3 && source_[position_ + 1] == '.' && source_[position_ + 2] == '.')
                {
                    Advance();
                    Advance();
                    Advance();
                    return { TokenKind::Ellipsis, "...", location };
                }
                kind = TokenKind::Dot;
                break;
            default:
                throw LexerError(location, "Unexpected byte: " + std::to_string(static_cast<unsigned char>(character)));
        }

        Advance();
        return { kind, std::string(1, character), location };
    }
}
