#pragma once
#include "pch.h"
#include "Token.h"
#include "ConstantValue.h"
#include <deque>

namespace flowx::parser
{
    class FunctionDeclarationNode;
    class ExpressionNode;
    class EntryNode;
}

namespace flowx::semantic
{
    struct SemanticModel;

    enum class L1NodeKind
    {
        AliasReference, Call, Merge, Split, Broadcast, Distribution, MemberAccess, Alias, Discard, Constant
    };

    enum class VisitState 
    { 
        Unvisited, Visiting, Complete 
    };

    struct L1Node;

    struct L1Edge
    {
        L1Node* target;
        std::size_t inputIndex;
    };

    struct L1Node
    {
        L1NodeKind kind;
        SourceLocation location;

        std::string identifier;
        std::vector<L1Edge> edges;

        bool isEntry = true;
        ConstantValue constant;
    };

    class L1Graph
    {
    private:
        std::deque<L1Node> nodes_;
        std::vector<std::string> reservedNames_;
        std::vector<std::string> builtinFunctionNames_;

        void CheckCycle();
        void VisitNode(const L1Node* node, std::map<const L1Node*, VisitState>& states);
        void ValidateName(const std::string& name, SourceLocation location) const;
        L1Node* BuildEntry(const parser::EntryNode& entry, const SemanticModel& model);
        L1Node* BuildExpression(const parser::ExpressionNode& expression, const SemanticModel& model);
        L1Node* BuildOperations(const parser::ExpressionNode& expression, L1Node* current, const SemanticModel& model);
        L1Node* BuildCall(const std::string& name, SourceLocation location, L1Node* input, const SemanticModel& model);
        L1Node* BuildAlias(const std::string& name, SourceLocation location, L1Node* input);

    public:
        std::map<std::string, L1Node*, std::less<>> aliases;
        std::vector<L1Node*> requiredAliases;
        std::unordered_set<std::string> inputAliases;

        L1Graph(const parser::FunctionDeclarationNode& declaration, const SemanticModel& model, std::span<const std::string_view> reservedNames, std::span<const std::string_view> builtinFunctionNames);
        L1Graph(const L1Graph&) = delete;
        L1Graph& operator=(const L1Graph&) = delete;
        L1Graph(L1Graph&&) = delete;
        L1Graph& operator=(L1Graph&&) = delete;

        L1Node* AddNode(L1NodeKind kind, SourceLocation location, const std::string& identifier = "");
        void Connect(L1Node* source, L1Node* target, std::size_t inputIndex = 0);
        const std::deque<L1Node>& GetNodes() const;
    };
}
