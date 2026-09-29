#pragma once
#include "pch.h"
#include "L2Graph.h"
#include "TypeTuple.h"

namespace flowx::semantic
{
    class SemanticAnalyzer;
    struct SemanticModel;
    struct FunctionOverloadDefinition;
    class L3Graph;
    struct L3Node;

    enum class L3NodeKind
    {
        Input, Output, EmptyInput, Call, Merge, Split, Broadcast, Distribution, MemberAccess, Discard, BuiltinCall, Construct
    };

    struct L3Edge
    {
        L3Node* target;
        std::size_t inputIndex;
        std::size_t outputIndex;
    };

    struct L3Node
    {
        L3NodeKind kind;
        SourceLocation location;
        std::string id;
        std::vector<L3Edge> edges;
        TypeTuple types;
        const L3Graph* function = nullptr;
    };

    class L3Graph
    {
        friend class SemanticAnalyzer;

    private:
        std::deque<L3Node> nodes_;
        unsigned int functionIndex_;
        unsigned int declarationIndex_;
        std::vector<TypeReference> inputs_;
        std::vector<TypeReference> outputs_;
        bool complete_ = false;

        void Build(const L2Graph& graph, const FunctionOverloadDefinition& definition, SemanticAnalyzer& analyzer, SemanticModel& model);
        void InferNode(L3Node& node, const L2Node& original, TypeTuple inputs, const FunctionOverloadDefinition& definition, SemanticAnalyzer& analyzer, SemanticModel& model);

    public:
        L3Graph(unsigned int functionIndex, unsigned int declarationIndex, const std::vector<TypeReference>& inputs);
        L3Graph(const L3Graph&) = delete;
        L3Graph& operator=(const L3Graph&) = delete;
        L3Graph(L3Graph&&) = delete;
        L3Graph& operator=(L3Graph&&) = delete;

        const std::deque<L3Node>& GetNodes() const;
        const std::vector<TypeReference>& GetInputs() const;
        const std::vector<TypeReference>& GetOutputs() const;
        unsigned int GetFunctionIndex() const;
        unsigned int GetDeclarationIndex() const;
        bool IsComplete() const;
    };
}
