#pragma once
#include "stdafx.h"
#include "Token.h"

namespace flowx
{
    class LexerError : public std::runtime_error
    {
    private:
        SourceLocation location_;

    public:
        LexerError(SourceLocation location, const std::string& message);
        SourceLocation Location() const noexcept;
    };

    class Lexer
    {
    private:
        bool AtEnd() const noexcept;
        char Peek() const noexcept;
        void Advance();
        void SkipWhitespace();
        Token ReadIdentifier();
        Token NextToken();

        std::string source_;
        std::size_t position_ = 0;
        SourceLocation location_;

    public:
        explicit Lexer(std::string source);

        std::vector<Token> Tokenize();
    };
}
