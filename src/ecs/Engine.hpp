#pragma once

#include <optional>
#include <stack>
#include <unordered_map>

#include "Archetype.hpp"
#include "Component.hpp"
#include "Core.hpp"

namespace EVA::ECS
{
    class Engine
    {
      public:
        using ArchetypeMap = std::unordered_map<ComponentList, Index, ComponentList::Hash>;

        Engine();

        Entity CreateEntity();
        Entity CreateEntity(const ComponentList& components);

        void DeleteEntity(const Entity& entity);

        std::optional<Index> GetArchetypeIndex(const ComponentList& components) const;
        Archetype& GetArchetype(Index index);

        std::vector<Archetype*> GetArchetypes(const ComponentList& components, bool allowEmpty);

        Index EntityCount() const { return m_EntityCount; }
        Index ArchetypeCount() { return m_Archetypes.size(); }

      private:
        Index m_EntityIdCounter;
        Index m_EntityCount;

        std::vector<EntityLocation> m_EntityLocations;
        std::stack<Index> m_FreeEntetyLocationIndices;

        ArchetypeMap m_ArchetypeMap;
        std::vector<Archetype> m_Archetypes;

        Entity GetNextEntity();

        Archetype& CreateArchetype(const ComponentList& components);
        std::pair<Index, Archetype&> GetOrCreateArchetype(const ComponentList& components);
    };
} // namespace EVA::ECS
