#include "pch.h"
#include "Lexer.h"
#include "PrimitiveType.h"

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
        else if (lexeme == "_")
            kind = TokenKind::Underscore;
        else if (IsPrimitiveType(lexeme))
            kind = TokenKind::PrimitiveType;

        return { kind, std::move(lexeme), location };
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
