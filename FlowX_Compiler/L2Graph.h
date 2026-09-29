#pragma once
#include "pch.h"
#include "Token.h"
#include <deque>

namespace flowx::parser
{
    class FunctionDeclarationNode;
}

namespace flowx::semantic
{
    class L1Graph;
    struct L1Node;
    struct L1Edge;

    enum class L2NodeKind
    {
        Input, Output, EmptyInput, Call, Merge, Split, Broadcast, Distribution, MemberAccess, Discard
    };

    struct L2Node;

    struct L2Edge
    {
        L2Node* target;
        std::size_t inputIndex;
        std::size_t outputIndex;
    };

    struct L2Node
    {
        L2NodeKind kind;
        SourceLocation location;

        std::string id;
        std::vector<L2Edge> edges;
        std::size_t outputCount = 0;
    };

    class L2Graph
    {
    private:
        std::deque<L2Node> nodes_;

        L2Node* AddNode(L2NodeKind kind, SourceLocation location, const std::string& id);
        void Connect(L2Node* source, L2Node* target, std::size_t inputIndex, std::size_t outputIndex);
        void ConnectEdge(L2Node* source, const L1Edge& edge, std::size_t outputIndex,
            const std::map<const L1Node*, L2Node*>& nodes,
            const parser::FunctionDeclarationNode& declaration, L2Node* output,
            std::unordered_set<const L1Node*>& visited);

    public:
        L2Graph(const L1Graph& graph, const parser::FunctionDeclarationNode& declaration);
        L2Graph(const L2Graph&) = delete;
        L2Graph& operator=(const L2Graph&) = delete;
        L2Graph(L2Graph&&) = delete;
        L2Graph& operator=(L2Graph&&) = delete;

        const std::deque<L2Node>& GetNodes() const;
    };
}
