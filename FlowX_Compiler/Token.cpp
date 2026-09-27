#include "stdafx.h"
#include "Token.h"

namespace flowx
{
    std::string_view TokenKindName(TokenKind kind) noexcept
    {
        switch (kind)
        {
            case TokenKind::StructKeyword: 
                return "StructKeyword";
            case TokenKind::PrimitiveType: 
                return "PrimitiveType";
            case TokenKind::Identifier:    
                return "Identifier";
            case TokenKind::LeftBrace:     
                return "LeftBrace";
            case TokenKind::RightBrace:    
                return "RightBrace";
            case TokenKind::Colon:         
                return "Colon";
            case TokenKind::Comma:         
                return "Comma";
            case TokenKind::QuestionBang:
                return "QuestionBang";
            case TokenKind::Question:
                return "Question";
            case TokenKind::Bang:
                return "Bang";
            case TokenKind::ClassKeyword:
                return "ClassKeyword";
            case TokenKind::FnKeyword:
                return "FnKeyword";
            case TokenKind::LessThan:
                return "LessThan";
            case TokenKind::GreaterThan:
                return "GreaterThan";
            case TokenKind::LeftParen:
                return "LeftParen";
            case TokenKind::RightParen:
                return "RightParen";
            case TokenKind::Semicolon:
                return "Semicolon";
            case TokenKind::Underscore:
                return "Underscore";
            case TokenKind::AsKeyword:
                return "AsKeyword";
            case TokenKind::Arrow:
                return "Arrow";
            case TokenKind::Ellipsis:
                return "Ellipsis";
            case TokenKind::Dot:
                return "Dot";
            case TokenKind::EndOfFile:     
                return "EndOfFile";
        }
        return "Unknown";
    }

    void PrintTokens(std::span<const Token> tokens, std::ostream& output)
    {
        for (const auto& token : tokens)
        {
            output << token.location.line << ':' << token.location.column
                << ' ' << TokenKindName(token.kind)
                << " \"" << token.lexeme << "\"\n";
        }
    }

    bool IsIdentifierStart(char character)
    {
        return (character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z') || character == '_';
    }

    bool IsIdentifierContinuation(char character)
    {
        return IsIdentifierStart(character) || (character >= '0' && character <= '9');
    }
}