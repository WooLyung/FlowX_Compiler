#pragma once
#include "pch.h"
#include "Token.h"
#include <deque>

namespace flowx::semantic
{
    enum class L2NodeKind
    {
        Input, Output, EmptyInput, Call, Merge, Split, Broadcast, Distribution, MemberAccess, Discard
    };

    struct L2Node
    {
        L2NodeKind kind;
        SourceLocation location;

        std::string id;
        std::vector<L2Node*> edges;
    };

    class L2Graph
    {
    private:
        std::deque<L2Node> nodes_;
    };
}
