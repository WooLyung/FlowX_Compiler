#pragma once
#include "pch.h"
#include "Token.h"

namespace flowx::semantic
{
    enum class SymbolKind 
    { 
        Struct, Class, Function 
    };

    struct Symbol
    {
        SymbolKind kind;
        unsigned int definitionIndex;
        SourceLocation location;
    };

    class SymbolTable
    {
    private:
        std::map<std::string, Symbol, std::less<>> symbols_;

    public:
        bool Define(const std::string& name, const Symbol symbol);
        bool Find(const std::string_view name, Symbol& outSymbol) const;
    };
}
