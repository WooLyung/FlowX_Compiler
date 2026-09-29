#include "pch.h"
#include "L1Graph.h"
#include "SemanticAnalyzer.h"
#include "EntryNode.h"
#include "OperationNode.h"

namespace flowx::semantic
{
    L1Node* L1Graph::AddNode(L1NodeKind kind, SourceLocation location, const std::string& identifier)
    {
        nodes_.push_back({ kind, location, identifier, {}, {} });
        return &nodes_.back();
    }

    void L1Graph::Connect(L1Node* source, L1Node* target)
    {
        source->edges.push_back(target);
        target->isEntry = false;
    }

    const std::deque<L1Node>& L1Graph::GetNodes() const
    {
        return nodes_;
    }

    L1Graph::L1Graph(const parser::FunctionDeclarationNode& declaration, const SemanticModel& model, std::span<const std::string_view> reservedNames, std::span<const std::string_view> builtinFunctionNames)
    {
        // 사전 설정
        for (const auto name : reservedNames)
            reservedNames_.emplace_back(name);
        for (const auto name : builtinFunctionNames)
            builtinFunctionNames_.emplace_back(name);
        for (const auto& input : declaration.GetInputs())
            inputAliases.insert(input.identifier);

        // L1 그래프 구성
        for (const auto& expression : declaration.GetExpressions())
            BuildExpression(*expression, model);

        // 별칭 검사
        for (const auto* alias : requiredAliases)
            if (!inputAliases.contains(alias->identifier) && !aliases.contains(alias->identifier))
                throw SemanticError(alias->location, "Unknown alias '" + alias->identifier + "'");

        for (const auto& output : declaration.GetOutputs())
            if (!inputAliases.contains(output.identifier) && !aliases.contains(output.identifier))
                throw SemanticError(declaration.GetLocation(), "Output alias '" + output.identifier + "' is not defined in function '" + declaration.GetIdentifier() + "'");
    
        // 별칭 연결
        for (const auto& node : nodes_)
        {
            if (node.kind == L1NodeKind::AliasReference && !inputAliases.contains(node.identifier))
            {
                const auto& source = aliases.find(node.identifier)->second;
                for (const auto& next : node.edges)
                    Connect(source, next);
            }
        }

        // 사이클 및 도달 불가능한 노드 검사
        CheckCycle();
    }

    void L1Graph::ValidateName(const std::string& name, SourceLocation location) const
    {
        for (const auto& reserved : reservedNames_)
            if (name == reserved)
                throw SemanticError(location, "Reserved name '" + name + "' cannot be declared");

        for (const auto& builtin : builtinFunctionNames_)
            if (name == builtin)
                throw SemanticError(location, "Builtin function name '" + name + "' cannot be used as a declaration name");
    }

    void L1Graph::CheckCycle()
    {
        // 사이클 검사
        std::map<const L1Node*, VisitState> states;

        for (const auto& node : nodes_)
            states.emplace(&node, VisitState::Unvisited);

        for (const auto& node : nodes_)
            if (states[&node] == VisitState::Unvisited && 
                (node.kind == L1NodeKind::AliasReference && inputAliases.contains(node.identifier) 
                    || node.kind != L1NodeKind::AliasReference && node.isEntry))
                VisitNode(&node, states);

        // 도달 가능성 검사
        for (const auto& node : nodes_)
            if (states[&node] == VisitState::Unvisited && !node.isEntry)
                throw SemanticError(node.location, "Unreachable expression");
    }

    void L1Graph::VisitNode(const L1Node* node, std::map<const L1Node*, VisitState>& states)
    {
        states[node] = VisitState::Visiting;

        for (const auto& next : node->branches)
        {
            if (states[next] == VisitState::Visiting)
                throw SemanticError(node->location, "Exist cyclic expression");
            if (states[next] == VisitState::Unvisited)
                VisitNode(next, states);
        }

        for (const auto& next : node->edges)
        {
            if (states[next] == VisitState::Visiting)
                throw SemanticError(node->location, "Exist cyclic expression");
            if (states[next] == VisitState::Unvisited)
                VisitNode(next, states);
        }

        states[node] = VisitState::Complete;
    }

    L1Node* L1Graph::BuildEntry(const parser::EntryNode& entry, const SemanticModel& model)
    {
        const auto location = entry.GetLocation();

        if (entry.IsTerminal())
        {
            const auto& name = entry.GetIdentifier();
            ValidateName(name, location);
            auto* node = AddNode(L1NodeKind::AliasReference, location, name);
            requiredAliases.push_back(node);
            return node;
        }

        auto* merge = AddNode(L1NodeKind::Merge, location);
        for (const auto& expression : entry.GetExpressions())
            Connect(BuildExpression(*expression, model), merge);
        return merge;
    }

    L1Node* L1Graph::BuildCall(const std::string& name, SourceLocation location, L1Node* input, const SemanticModel& model)
    {
        bool found = name == "pass";
        for (const auto& builtin : builtinFunctionNames_)
            if (name == builtin)
                found = true;

        Symbol symbol;
        if (!found && model.symbols.Find(name, symbol))
            found = symbol.kind == SymbolKind::Function || symbol.kind == SymbolKind::Struct;
        if (!found)
            throw SemanticError(location, "Unknown callable '" + name + "'");

        auto* node = AddNode(L1NodeKind::Call, location, name);
        if (input)
            Connect(input, node);
        return node;
    }

    L1Node* L1Graph::BuildAlias(const std::string& name, SourceLocation location, L1Node* input)
    {
        ValidateName(name, location);
        if (inputAliases.contains(name) || aliases.contains(name))
            throw SemanticError(location, "Duplicate alias '" + name + "'");

        auto* node = AddNode(L1NodeKind::Alias, location, name);
        if (input)
            Connect(input, node);
        aliases.emplace(name, node);

        return node;
    }

    L1Node* L1Graph::BuildExpression(const parser::ExpressionNode& expression, const SemanticModel& model)
    {
        return BuildOperations(expression, BuildEntry(*expression.GetEntry(), model), model);
    }

    L1Node* L1Graph::BuildOperations(const parser::ExpressionNode& expression, L1Node* current, const SemanticModel& model)
    {
        for (const auto& operation : expression.GetOperations())
        {
            const auto location = operation->GetLocation();
            switch (operation->GetKind())
            {
                case parser::OperationKind::MemberAccess:
                {
                    ValidateName(operation->GetIdentifier(), location);
                    auto* node = AddNode(L1NodeKind::MemberAccess, location, operation->GetIdentifier());
                    Connect(current, node);
                    current = node;
                    break;
                }
                case parser::OperationKind::Call:
                    current = BuildCall(operation->GetIdentifier(), location, current, model);
                    break;
                case parser::OperationKind::Broadcast:
                case parser::OperationKind::Distribution:
                {
                    const bool distribute = operation->GetKind() == parser::OperationKind::Distribution;
                    auto* block = AddNode(distribute ? L1NodeKind::Distribution : L1NodeKind::Broadcast, location);
                    Connect(current, block);
                    for (const auto& branch : operation->GetGroup())
                    {
                        const auto& name = branch->GetEntry()->GetIdentifier();
                        L1Node* node;
                        if (name == "_")
                        {
                            if (!distribute || !branch->GetOperations().empty())
                                throw SemanticError(branch->GetLocation(), "'_' must be a standalone distribution item");
                            node = AddNode(L1NodeKind::Discard, branch->GetLocation(), name);
                        }
                        else
                            node = BuildCall(name, branch->GetLocation(), nullptr, model);

                        block->branches.push_back(node);
                        node->isEntry = false;
                        BuildOperations(*branch, node, model);
                    }
                    current = block;
                    break;
                }
                case parser::OperationKind::Alias:
                {
                    if (operation->GetTargetKind() == parser::OperationTargetKind::Identifier)
                        current = BuildAlias(operation->GetIdentifier(), location, current);
                    else
                    {
                        const auto& names = operation->GetIdentifiers();
                        auto* split = AddNode(L1NodeKind::Split, location);
                        Connect(current, split);
                        for (const auto& name : names)
                        {
                            L1Node* alias;
                            if (name == "_")
                                alias = AddNode(L1NodeKind::Discard, location, "_");
                            else
                                alias = BuildAlias(name, location, nullptr);
                            split->branches.push_back(alias);
                            alias->isEntry = false;
                        }
                        current = split;
                    }
                    break;
                }
            }
        }
        return current;
    }
}
