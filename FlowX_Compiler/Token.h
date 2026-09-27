#pragma once
#include "stdafx.h"

namespace flowx
{
    enum class TokenKind
    {
        StructKeyword,
        ClassKeyword,
        FnKeyword,
        AsKeyword,
        PrimitiveType,
        Identifier,
        Underscore,
        LeftBrace,
        RightBrace,
        LessThan,
        GreaterThan,
        LeftParen,
        RightParen,
        Semicolon,
        Arrow,
        Ellipsis,
        Dot,
        Colon,
        Comma,
        QuestionBang,
        Question,
        Bang,
        EndOfFile
    };

    struct SourceLocation
    {
        std::size_t line = 1;
        std::size_t column = 1;
    };

    struct Token
    {
        TokenKind kind;
        std::string lexeme;
        SourceLocation location;
    };

    std::string_view TokenKindName(TokenKind kind) noexcept;
    void PrintTokens(std::span<const Token> tokens, std::ostream& output);
    bool IsIdentifierStart(char character);
    bool IsIdentifierContinuation(char character);
}