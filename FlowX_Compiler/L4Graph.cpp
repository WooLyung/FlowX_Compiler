#include "pch.h"
#include "L4Graph.h"
#include "L3Graph.h"
#include <functional>

namespace flowx::semantic
{
    L4Graph::L4Graph(unsigned int functionIndex, unsigned int declarationIndex)
        : functionIndex_(functionIndex), declarationIndex_(declarationIndex)
    {
    }

    const std::deque<L4Node>& L4Graph::GetNodes() const
    {
        return nodes_;
    }

    unsigned int L4Graph::GetFunctionIndex() const
    {
        return functionIndex_;
    }

    unsigned int L4Graph::GetDeclarationIndex() const
    {
        return declarationIndex_;
    }

    L4Node* L4Graph::AddNode(L4NodeKind kind, SourceLocation location, const std::string& id, std::optional<TypeReference> type, std::size_t index)
    {
        nodes_.push_back({ kind, location, id, {}, std::move(type), index });
        return &nodes_.back();
    }

    void L4Graph::Build(const L3Graph& graph, const std::map<const L3Graph*, L4Graph*>& functions)
    {
        std::map<const L3Node*, std::vector<std::vector<L4Node*>>> values;
        std::map<const L3Node*, std::map<std::size_t, std::pair<const L3Node*, std::size_t>>> incoming;
        std::map<const L3Node*, std::size_t> remaining;
        std::deque<const L3Node*> ready;

        for (const auto& node : graph.GetNodes())
            remaining.emplace(&node, 0);

        for (const auto& node : graph.GetNodes())
        {
            for (const auto& edge : node.edges)
            {
                incoming[edge.target].emplace(edge.inputIndex, std::make_pair(&node, edge.outputIndex));
                ++remaining.at(edge.target);
            }
        }
          
        for (const auto& node : graph.GetNodes())
            if (remaining.at(&node) == 0)
                ready.push_back(&node);

        std::function<void(const TypeTuple&, std::vector<const TypeReference*>&)> flattenTypes;
        flattenTypes = [&](const TypeTuple& tuple, std::vector<const TypeReference*>& types)
        {
            for (const auto& element : tuple.elements)
                if (const auto* type = std::get_if<TypeReference>(&element))
                    types.push_back(type);
                else
                    flattenTypes(std::get<TypeTuple>(element), types);
        };

        while (!ready.empty())
        {
            const auto* node = ready.front();
            ready.pop_front();
            std::vector<L4Node*> inputs;
            for (const auto& [index, source] : incoming[node])
            {
                if (source.first->kind == L3NodeKind::Discard)
                    continue;
                const auto& port = values.at(source.first).at(source.second);
                inputs.insert(inputs.end(), port.begin(), port.end());
            }
            std::vector<const TypeReference*> types;
            flattenTypes(node->types, types);
            std::vector<L4Node*> results;
            switch (node->kind)
            {
                case L3NodeKind::Constant:
                {
                    auto* value = AddNode(L4NodeKind::Constant, node->location, node->id, *types.at(0));
                    value->constant = node->constant;
                    results.push_back(value);
                    break;
                }
                case L3NodeKind::Input:
                case L3NodeKind::Output:
                    for (std::size_t index = 0; index < types.size(); ++index)
                    {
                        auto* value = AddNode(node->kind == L3NodeKind::Input ? L4NodeKind::Input : L4NodeKind::Output,
                            node->location, node->id, *types[index], index);
                        if (node->kind == L3NodeKind::Output)
                            inputs.at(index)->edges.push_back({ value, 0 });
                        results.push_back(value);
                    }
                    break;
                case L3NodeKind::Call:
                case L3NodeKind::BuiltinCall:
                {
                    auto* call = AddNode(node->kind == L3NodeKind::Call ? L4NodeKind::Call : L4NodeKind::BuiltinCall,
                        node->location, node->id);
                    call->builtinIndex = node->builtinIndex;
                    if (node->function)
                        call->function = functions.at(node->function);
                    for (std::size_t index = 0; index < inputs.size(); ++index)
                        inputs[index]->edges.push_back({ call, index });
                    for (std::size_t index = 0; index < types.size(); ++index)
                    {
                        auto* value = AddNode(L4NodeKind::Value, node->location, node->id, *types[index], index);
                        call->edges.push_back({ value, 0 });
                        results.push_back(value);
                    }
                    break;
                }
                case L3NodeKind::MemberAccess:
                case L3NodeKind::Construct:
                {
                    auto* value = AddNode(node->kind == L3NodeKind::Construct ? L4NodeKind::Construct : L4NodeKind::MemberAccess,
                        node->location, node->id, *types.at(0));
                    for (std::size_t index = 0; index < inputs.size(); ++index)
                        inputs[index]->edges.push_back({ value, index });
                    results.push_back(value);
                    break;
                }
                case L3NodeKind::EmptyInput:
                case L3NodeKind::Discard:
                    break;
                case L3NodeKind::Merge:
                case L3NodeKind::Split:
                case L3NodeKind::Broadcast:
                case L3NodeKind::Distribution:
                    results = std::move(inputs);
                    break;
            }

            auto& ports = values[node];
            std::size_t offset = 0;
            for (const auto& element : node->types.elements)
            {
                std::size_t count = 1;
                if (const auto* tuple = std::get_if<TypeTuple>(&element))
                {
                    std::vector<const TypeReference*> leaves;
                    flattenTypes(*tuple, leaves);
                    count = leaves.size();
                }

                auto& port = ports.emplace_back();
                for (std::size_t index = 0; index < count; ++index)
                    port.push_back(results.at(offset++));
            }

            for (const auto& edge : node->edges)
                if (--remaining.at(edge.target) == 0)
                    ready.push_back(edge.target);
        }
    }
}
