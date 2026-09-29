#include "pch.h"
#include "L3Graph.h"
#include "SemanticAnalyzer.h"

namespace flowx::semantic
{
    L3Graph::L3Graph(unsigned int functionIndex, unsigned int declarationIndex, const std::vector<TypeReference>& inputs)
        : functionIndex_(functionIndex), declarationIndex_(declarationIndex), inputs_(inputs)
    {
    }

    const std::deque<L3Node>& L3Graph::GetNodes() const { return nodes_; }
    const std::vector<TypeReference>& L3Graph::GetInputs() const { return inputs_; }
    const std::vector<TypeReference>& L3Graph::GetOutputs() const { return outputs_; }
    unsigned int L3Graph::GetFunctionIndex() const { return functionIndex_; }
    unsigned int L3Graph::GetDeclarationIndex() const { return declarationIndex_; }
    bool L3Graph::IsComplete() const { return complete_; }

    void L3Graph::Build(const L2Graph& graph, const FunctionOverloadDefinition& definition, SemanticAnalyzer& analyzer, SemanticModel& model)
    {
        std::map<const L2Node*, L3Node*> nodes;
        std::map<const L2Node*, std::size_t> remaining;
        std::map<const L2Node*, std::map<std::size_t, std::pair<const L2Node*, std::size_t>>> incoming;
        std::deque<const L2Node*> ready;

        for (const auto& node : graph.GetNodes())
        {
            nodes_.push_back({ static_cast<L3NodeKind>(node.kind), node.location, node.id, {}, {} });
            nodes.emplace(&node, &nodes_.back());
            remaining.emplace(&node, 0);
        }

        for (const auto& node : graph.GetNodes())
        {
            for (const auto& edge : node.edges)
            {
                nodes.at(&node)->edges.push_back({ nodes.at(edge.target), edge.inputIndex, edge.outputIndex });
                if (!incoming[edge.target].emplace(edge.inputIndex, std::make_pair(&node, edge.outputIndex)).second)
                    throw SemanticError(edge.target->location, "Multiple sources for the same input slot");
                
                ++remaining.at(edge.target);
            }
        }

        for (const auto& node : graph.GetNodes())
            if (remaining.at(&node) == 0)
                ready.push_back(&node);

        std::size_t processed = 0;
        while (!ready.empty())
        {
            const auto* original = ready.front();
            ready.pop_front();

            TypeTuple inputs;
            for (const auto& [index, source] : incoming[original])
            {
                const auto* previous = nodes.at(source.first);
                if (previous->kind == L3NodeKind::Discard && original->kind == L2NodeKind::Merge)
                    continue;

                if (source.second >= previous->types.elements.size())
                    throw SemanticError(original->location, "Invalid source output slot");

                if (original->kind != L2NodeKind::Merge && index != inputs.elements.size())
                    throw SemanticError(original->location, "Missing input slot");

                inputs.elements.push_back(previous->types.elements[source.second]);
            }

            InferNode(*nodes.at(original), *original, std::move(inputs), definition, analyzer, model);
            ++processed;

            for (const auto& edge : original->edges)
                if (--remaining.at(edge.target) == 0)
                    ready.push_back(edge.target);
        }
        if (processed != nodes_.size())
            throw SemanticError(definition.location, "Cannot infer types in a cyclic graph");
    }

    void L3Graph::InferNode(L3Node& node, const L2Node& original, TypeTuple inputs, const FunctionOverloadDefinition& definition, SemanticAnalyzer& analyzer, SemanticModel& model)
    {
        if (node.kind == L3NodeKind::Input)
        {
            for (const auto& type : inputs_)
                node.types.elements.emplace_back(type);
            return;
        }

        if (node.kind == L3NodeKind::EmptyInput)
        {
            node.types.elements.emplace_back(TypeTuple{});
            return;
        }

        if (node.kind == L3NodeKind::Output)
        {
            if (inputs.elements.size() != definition.outputs.size())
                throw SemanticError(node.location, "Incorrect number of function outputs");

            for (std::size_t index = 0; index < inputs.elements.size(); ++index)
            {
                const auto* type = std::get_if<TypeReference>(&inputs.elements[index]);
                if (!type || !analyzer.AcceptsType(definition.outputs[index].type, *type, model))
                    throw SemanticError(node.location, "Type mismatch for output '" + definition.outputs[index].identifier + "'");
                outputs_.push_back(*type);
            }

            node.types = std::move(inputs);
            return;
        }

        if (node.kind == L3NodeKind::Merge)
        {
            node.types.elements.emplace_back(std::move(inputs));
            return;
        }

        if (inputs.elements.size() != 1)
            throw SemanticError(node.location, "Operation requires one input value or tuple");

        const auto& input = inputs.elements.front();
        if (node.kind == L3NodeKind::Discard)
            return;

        if (node.kind == L3NodeKind::Broadcast)
        {
            node.types = std::move(inputs);
            return;
        }

        if (node.kind == L3NodeKind::Split || node.kind == L3NodeKind::Distribution)
        {
            if (const auto* tuple = std::get_if<TypeTuple>(&input))
                node.types = TypeTuple(*tuple);
            else
                node.types = std::move(inputs);
            if (node.types.elements.size() != original.outputCount)
                throw SemanticError(node.location, "Tuple size does not match the number of branches");
            return;
        }

        if (node.kind == L3NodeKind::MemberAccess)
        {
            const auto* type = std::get_if<TypeReference>(&input);
            Symbol symbol;
            if (!type || type->kind != TypeReferenceKind::Named || !model.symbols.Find(type->lexeme, symbol) || symbol.kind != SymbolKind::Struct)
                throw SemanticError(node.location, "Member access requires a structure value");
            
            if (type->modifier != TypeModifierKind::None)
                throw SemanticError(node.location, "Member access on a nullable or errorable structure is not defined");
           
            for (const auto& field : model.structs[symbol.definitionIndex].fields)
            {
                if (field.name == node.id)
                {
                    node.types.elements.emplace_back(field.type);
                    return;
                }
            }

            throw SemanticError(node.location, "Unknown member '" + node.id + "' in structure '" + type->lexeme + "'");
        }

        std::vector<TypeReference> arguments;
        if (const auto* tuple = std::get_if<TypeTuple>(&input))
        {
            for (const auto& item : tuple->elements)
            {
                const auto* type = std::get_if<TypeReference>(&item);
                if (!type)
                    throw SemanticError(node.location, "A function parameter cannot receive a nested tuple");

                arguments.push_back(*type);
            }
        }
        else
            arguments.push_back(std::get<TypeReference>(input));

        Symbol symbol;
        if (!model.symbols.Find(node.id, symbol))
            throw SemanticError(node.location, "Unknown callable '" + node.id + "'");

        if (symbol.kind == SymbolKind::Struct)
        {
            const auto& structure = model.structs[symbol.definitionIndex];
            if (arguments.size() != structure.fields.size())
                throw SemanticError(node.location, "Incorrect argument count for structure '" + node.id + "'");
           
            for (std::size_t index = 0; index < arguments.size(); ++index)
                if (!analyzer.AcceptsType(structure.fields[index].type, arguments[index], model))
                    throw SemanticError(node.location, "Type mismatch for field '" + structure.fields[index].name + "'");
           
            node.kind = L3NodeKind::Construct;
            node.types.elements.emplace_back(TypeReference{ TypeReferenceKind::Named, node.id, TypeModifierKind::None });
            return;
        }

        if (symbol.kind != SymbolKind::Function)
            throw SemanticError(node.location, "Type is not callable '" + node.id + "'");
        
        const auto& overload = analyzer.ResolveOverload(model.functions[symbol.definitionIndex], arguments, node.location, model);
        TypeTuple results;
        if (overload.declarationIndex)
        {
            const auto& function = analyzer.GenerateL3(symbol.definitionIndex, overload, arguments, node.location, model);
            node.function = &function;
            for (const auto& type : function.GetOutputs())
                results.elements.emplace_back(type);
        }
        else
        {
            node.kind = L3NodeKind::BuiltinCall;
            for (const auto& parameter : overload.outputs)
                results.elements.emplace_back(parameter.type);
        }

        if (results.elements.size() == 1)
            node.types.elements.push_back(results.elements.front());
        else
            node.types.elements.emplace_back(std::move(results));
    }
}
