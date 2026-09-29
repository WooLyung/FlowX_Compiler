#include "pch.h"
#include "L2Graph.h"
#include "L1Graph.h"
#include "SemanticAnalyzer.h"

namespace flowx::semantic
{
    L2Node* L2Graph::AddNode(L2NodeKind kind, SourceLocation location, const std::string& id)
    {
        nodes_.push_back({ kind, location, id, {} });
        return &nodes_.back();
    }

    void L2Graph::Connect(L2Node* source, L2Node* target, std::size_t inputIndex, std::size_t outputIndex)
    {
        source->edges.push_back({ target, inputIndex, outputIndex });
    }

    const std::deque<L2Node>& L2Graph::GetNodes() const
    {
        return nodes_;
    }

    void L2Graph::ConnectEdge(L2Node* source, const L1Edge& edge, std::size_t outputIndex, const std::map<const L1Node*, L2Node*>& nodes, const parser::FunctionDeclarationNode& declaration, L2Node* output, std::unordered_set<const L1Node*>& visited)
    {
        const auto* target = edge.target;
        if (target->kind == L1NodeKind::Merge && !target->isEntry && target->edges.empty())
            return;

        if (target->kind == L1NodeKind::Alias ||
            (target->kind == L1NodeKind::Call && target->identifier == "pass"))
        {
            if (!visited.insert(target).second)
                return;

            if (target->kind == L1NodeKind::Alias)
            {
                const auto& outputs = declaration.GetOutputs();
                for (std::size_t index = 0; index < outputs.size(); ++index)
                    if (outputs[index].identifier == target->identifier)
                        Connect(source, output, index, outputIndex);
            }

            for (const auto& next : target->edges)
                ConnectEdge(source, next, outputIndex, nodes, declaration, output, visited);
            return;
        }

        auto* node = nodes.at(target);
        Connect(source, node, edge.inputIndex, outputIndex);
        if (!visited.insert(target).second)
            return;

        for (std::size_t index = 0; index < target->edges.size(); ++index)
        {
            const auto nextOutputIndex = target->kind == L1NodeKind::Split || target->kind == L1NodeKind::Distribution ? index : 0;
            ConnectEdge(node, target->edges[index], nextOutputIndex, nodes, declaration, output, visited);
        }
    }

    L2Graph::L2Graph(const L1Graph& graph, const parser::FunctionDeclarationNode& declaration)
    {
        std::map<const L1Node*, L2Node*> nodes;
        std::unordered_set<const L1Node*> visited;
        std::vector<const L1Node*> emptyInputs;
        auto* input = AddNode(L2NodeKind::Input, declaration.GetLocation(), "");
        auto* output = AddNode(L2NodeKind::Output, declaration.GetLocation(), "");

        for (const auto& node : graph.GetNodes())
        {
            if (node.kind == L1NodeKind::Alias || (node.kind == L1NodeKind::Merge && !node.isEntry && node.edges.empty()))
                continue;

            if (node.kind == L1NodeKind::AliasReference ||
                (node.kind == L1NodeKind::Call && node.identifier == "pass"))
                continue;

            L2NodeKind kind;
            switch (node.kind)
            {
                case L1NodeKind::Call: kind = L2NodeKind::Call; break;
                case L1NodeKind::Merge: kind = node.isEntry ? L2NodeKind::EmptyInput : L2NodeKind::Merge; break;
                case L1NodeKind::Split: kind = L2NodeKind::Split; break;
                case L1NodeKind::Broadcast: kind = L2NodeKind::Broadcast; break;
                case L1NodeKind::Distribution: kind = L2NodeKind::Distribution; break;
                case L1NodeKind::MemberAccess: kind = L2NodeKind::MemberAccess; break;
                case L1NodeKind::Discard: kind = L2NodeKind::Discard; break;
                default: throw SemanticError(node.location, "Invalid L1 node kind");
            }
            auto* created = AddNode(kind, node.location, node.identifier);
            if (kind == L2NodeKind::Split || kind == L2NodeKind::Distribution)
                created->outputCount = node.edges.size();
            nodes.emplace(&node, created);
            if (kind == L2NodeKind::EmptyInput)
                emptyInputs.push_back(&node);
        }

        const auto& inputs = declaration.GetInputs();
        const auto& outputs = declaration.GetOutputs();
        for (std::size_t inputIndex = 0; inputIndex < inputs.size(); ++inputIndex)
        {
            for (const auto* reference : graph.requiredAliases)
                if (reference->identifier == inputs[inputIndex].identifier)
                    for (const auto& edge : reference->edges)
                        ConnectEdge(input, edge, inputIndex, nodes, declaration, output, visited);

            for (std::size_t outputIndex = 0; outputIndex < outputs.size(); ++outputIndex)
                if (inputs[inputIndex].identifier == outputs[outputIndex].identifier)
                    Connect(input, output, outputIndex, inputIndex);
        }

        for (const auto* node : emptyInputs)
        {
            visited.insert(node);
            for (const auto& edge : node->edges)
                ConnectEdge(nodes.at(node), edge, 0, nodes, declaration, output, visited);
        }
    }
}