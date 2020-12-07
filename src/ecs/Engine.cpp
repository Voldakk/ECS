#include "Engine.hpp"

namespace EVA::ECS
{
    Engine::Engine() : m_EntityIdCounter(0), m_EntityCount(0) {}

    Entity Engine::CreateEntity() { return CreateEntity(ComponentList()); }

    Entity Engine::CreateEntity(const ComponentList& components)
    {
        auto [archetypeIndex, archetype] = GetOrCreateArchetype(components);

        size_t index = m_EntityLocations.size();
        if (!m_FreeEntetyLocationIndices.empty())
        {
            index = m_FreeEntetyLocationIndices.top();
            m_FreeEntetyLocationIndices.pop();
        }

        auto entity            = GetNextEntity();
        entity.index           = index;
        auto [chunk, position] = archetype.CreateEntity(entity);

        if (index == m_EntityLocations.size())
        {
            m_EntityLocations.emplace_back(EntityLocation(archetypeIndex, chunk, position, entity.id));
        }
        else
        {
            m_EntityLocations[index] = EntityLocation(archetypeIndex, chunk, position, entity.id);
        }

        return entity;
    }

    void Engine::DeleteEntity(const Entity& entity)
    {
        const auto& loc = m_EntityLocations[entity.index];
        ECS_ASSERT(entity.id == loc.entityId);
        Archetype& archetype = GetArchetype(loc.archetype);
        auto moved           = archetype.DestroyEntity(loc.chunk, loc.position);
        m_FreeEntetyLocationIndices.push(entity.index);
        m_EntityCount--;
        m_EntityLocations[moved.index] = EntityLocation(loc.archetype, loc.chunk, loc.position, moved.id);
    }

    std::optional<Index> Engine::GetArchetypeIndex(const ComponentList& components) const
    {
        auto it = m_ArchetypeMap.find(components);
        if (it != m_ArchetypeMap.end())
        {
            return (*it).second;
        }
        else
        {
            return std::nullopt;
        }
    }

    Archetype& Engine::GetArchetype(Index index) { return m_Archetypes[index]; }

    std::vector<Archetype*> Engine::GetArchetypes(const ComponentList& components, bool allowEmpty)
    {
        std::vector<Archetype*> archetypes;
        for (auto [key, value] : m_ArchetypeMap)
        {
            if (key.Contains(components) && (allowEmpty || m_Archetypes[value].EntityCount() > 0))
            {
                archetypes.push_back(&m_Archetypes[value]);
            }
        }
        return archetypes;
    }

    void Engine::AddComponent(Entity& entity, ComponentType type) { AddComponent(entity, type, ComponentMap::DefaultData(type)); }

    void Engine::AddComponent(Entity& entity, ComponentType type, const Byte* data)
    {
        const auto& loc = m_EntityLocations[entity.index];
        ECS_ASSERT(entity.id == loc.entityId);
        Archetype& oldArchetype = GetArchetype(loc.archetype);

        ComponentList types = oldArchetype.GetComponents();
        types.Add(type);
        auto [newArchetypeIndex, newArchetype] = GetOrCreateArchetype(types);

        Archetype& archetype = GetArchetype(loc.archetype); // GetOrCreateArchetype may invalidate the refference to archetype

        auto [newChunk, newPosition] = newArchetype.AddEntityAddComponent(archetype, loc.chunk, loc.position, type, data);

        auto moved = archetype.DestroyEntity(loc.chunk, loc.position);

        m_EntityLocations[moved.index]  = EntityLocation(loc.archetype, loc.chunk, loc.position, moved.id);
        m_EntityLocations[entity.index] = EntityLocation(newArchetypeIndex, newChunk, newPosition, entity.id);
    }

    void Engine::RemoveComponent(Entity& entity, ComponentType type)
    {
        const auto& loc = m_EntityLocations[entity.index];
        ECS_ASSERT(entity.id == loc.entityId);
        Archetype& oldArchetype = GetArchetype(loc.archetype);

        ComponentList types = oldArchetype.GetComponents();
        types.Remove(type);
        auto [newArchetypeIndex, newArchetype] = GetOrCreateArchetype(types);

        Archetype& archetype = GetArchetype(loc.archetype); // GetOrCreateArchetype may invalidate the refference to archetype

        auto [newChunk, newPosition] = newArchetype.AddEntityRemoveComponent(archetype, loc.chunk, loc.position, type);

        auto moved = archetype.DestroyEntity(loc.chunk, loc.position);

        m_EntityLocations[moved.index]  = EntityLocation(loc.archetype, loc.chunk, loc.position, moved.id);
        m_EntityLocations[entity.index] = EntityLocation(newArchetypeIndex, newChunk, newPosition, entity.id);
    }

    void Engine::UpdateSystems()
    {
        for (const auto& s : m_Systems)
        {
            s->Update();
        }
    }

    Entity Engine::GetNextEntity()
    {
        m_EntityCount++;
        return Entity(m_EntityIdCounter++);
    }

    Archetype& Engine::CreateArchetype(const ComponentList& components)
    {
        m_Archetypes.emplace_back(components);
        m_ArchetypeMap.emplace(components, m_Archetypes.size() - 1);
        return m_Archetypes[m_Archetypes.size() - 1];
    }

    std::pair<Index, Archetype&> Engine::GetOrCreateArchetype(const ComponentList& components)
    {
        auto index = GetArchetypeIndex(components);
        if (index)
        {
            return std::make_pair(*index, std::reference_wrapper<Archetype>(m_Archetypes[*index]));
        }
        else
        {
            return std::make_pair(m_Archetypes.size() - 1, std::reference_wrapper<Archetype>(CreateArchetype(components)));
        }
    }
} // namespace EVA::ECS
