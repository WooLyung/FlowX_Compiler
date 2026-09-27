#include "pch.h"
#include "SymbolTable.h"

namespace flowx::semantic
{
    bool SymbolTable::Define(const std::string& name, const Symbol symbol)
    {
        return symbols_.emplace(name, symbol).second;
    }

    bool SymbolTable::Find(const std::string_view name, Symbol& outSymbol) const
    {
        const auto found = symbols_.find(name);

        if (found == symbols_.end())
            return false;

        outSymbol = found->second;
        return true;
    }
}
