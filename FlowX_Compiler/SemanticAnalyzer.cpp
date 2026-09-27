#include "pch.h"
#include "SemanticAnalyzer.h"

namespace flowx::semantic
{
    SemanticError::SemanticError(SourceLocation location, const std::string& message)
        : std::runtime_error(message), location_(location)
    {
    }

    SourceLocation SemanticError::Location() const noexcept
    {
        return location_;
    }

    const SemanticModel SemanticAnalyzer::Analyze(const parser::ProgramNode& program)
    {
        SemanticModel model;

        RegisterStructs(program, model);
        ResolveFields(program, model);
        OrderStructs(model);

        return model;
    }

    void SemanticAnalyzer::RegisterStructs(const parser::ProgramNode& program, SemanticModel& model)
    {
        for (const auto& declaration : program.GetStructDeclarations())
        {
            const auto& name = declaration->GetIdentifier();
            const auto location = declaration->GetLocation();

            Symbol exist;
            if (model.symbols.Find(name, exist))
            {
                throw SemanticError(location, "Duplicate declaration '" + name
                    + "' (first declared at " + std::to_string(exist.location.line)
                    + ":" + std::to_string(exist.location.column) + ")");
            }

            const unsigned int id = (unsigned int)model.structs.size();
            model.symbols.Define(name, { SymbolKind::Struct, id, location });
            model.structs.push_back({ name, location, {} });
        }
    }

    void SemanticAnalyzer::ResolveFields(const parser::ProgramNode& program, SemanticModel& model)
    {
        const auto& declarations = program.GetStructDeclarations();

        for (unsigned int id = 0; id < declarations.size(); id++)
        {
            auto& definition = model.structs[id];

            std::unordered_set<std::string> fieldNames;
            for (const auto& field : declarations[id]->GetFields())
            {
                const auto& name = field->GetIdentifier();
                const auto& type = field->GetTypeReference();
                const auto location = field->GetLocation();

                if (!fieldNames.insert(name).second)
                    throw SemanticError(location, "Duplicate field '" + definition.name + "." + name + "'");

                if (type.modifier != TypeModifierKind::None && type.modifier != TypeModifierKind::Nullable)
                    throw SemanticError(location, "Field '" + definition.name + "." + name + "' cannot use '!' or '?!'");

                std::optional<unsigned int> referencedStruct;
                if (type.kind == TypeReferenceKind::Struct)
                {
                    Symbol symbol{};
                    if (!model.symbols.Find(type.lexeme, symbol) || symbol.kind != SymbolKind::Struct)
                        throw SemanticError(location, "Unknown structure type '" + type.lexeme + "' in field '" + definition.name + "." + name + "'");
                    referencedStruct = symbol.definitionIndex;
                }

                definition.fields.push_back({ name, type, location, referencedStruct });
            }
        }
    }

    void SemanticAnalyzer::OrderStructs(SemanticModel& model)
    {
        std::vector<VisitState> states(model.structs.size(), VisitState::Unvisited);
        for (unsigned int id = 0; id < model.structs.size(); ++id)
        {
            if (states[id] == VisitState::Unvisited)
                VisitStruct(id, model, states);
        }
    }

    void SemanticAnalyzer::VisitStruct(unsigned int id, SemanticModel& model, std::vector<VisitState>& states)
    {
        states[id] = VisitState::Visiting;

        for (const auto& field : model.structs[id].fields)
        {
            if (!field.referencedStruct)
                continue;

            const unsigned int dependency = *field.referencedStruct;
            if (states[dependency] == VisitState::Visiting)
                throw SemanticError(field.location, "Exist cyclic structure for " + model.structs[id].name);
            if (states[dependency] == VisitState::Unvisited)
                VisitStruct(dependency, model, states);
        }

        states[id] = VisitState::Complete;
    }
}
