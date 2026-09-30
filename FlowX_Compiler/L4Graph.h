#pragma once
#include "pch.h"
#include "TypeReference.h"
#include "Token.h"
#include "ConstantValue.h"

namespace flowx::semantic
{
    class L3Graph;
    class L4Graph;
    struct L4Node;

    enum class L4NodeKind
    {
        Input, Output, Call, MemberAccess, BuiltinCall, Construct, Value, Constant
    };

    struct L4Edge
    {
        L4Node* target;
        std::size_t inputIndex;
    };

    struct L4Node
    {
        L4NodeKind kind;
        SourceLocation location;

        std::string id;
        std::vector<L4Edge> edges;
        std::optional<TypeReference> type;
        std::size_t index = 0;

        const L4Graph* function = nullptr;
        ConstantValue constant;
        std::optional<std::size_t> builtinIndex;
    };

    class L4Graph
    {
        friend class SemanticAnalyzer;

    private:
        std::deque<L4Node> nodes_;
        unsigned int functionIndex_;
        unsigned int declarationIndex_;

        L4Node* AddNode(L4NodeKind kind, SourceLocation location, const std::string& id, std::optional<TypeReference> type = std::nullopt, std::size_t index = 0);
        void Build(const L3Graph& graph, const std::map<const L3Graph*, L4Graph*>& functions);

    public:
        L4Graph(unsigned int functionIndex, unsigned int declarationIndex);
        L4Graph(const L4Graph&) = delete;
        L4Graph& operator=(const L4Graph&) = delete;
        L4Graph(L4Graph&&) = delete;
        L4Graph& operator=(L4Graph&&) = delete;

        const std::deque<L4Node>& GetNodes() const;
        unsigned int GetFunctionIndex() const;
        unsigned int GetDeclarationIndex() const;
    };
}
