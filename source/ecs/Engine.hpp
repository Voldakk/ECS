#pragma once

#include <optional>
#include <stack>
#include <unordered_map>

#include "Archetype.hpp"
#include "Component.hpp"
#include "Core.hpp"
#include "System.hpp"

namespace EVA::ECS
{
    class System;
    class Engine
    {
      public:
        using ArchetypeMap = std::unordered_map<ComponentList, Index>;

        Engine();

        Entity CreateEntity();
        Entity CreateEntity(const ComponentList& components);
        Entity CreateEntity(const ComponentList& components, const Byte* data);

        template <typename... T> Entity CreateEntityFromComponents(const T&... components);

        void DeleteEntity(const Entity& entity);

        std::optional<Index> GetArchetypeIndex(const ComponentList& components) const;
        Archetype& GetArchetype(Index index);

        template <typename... T> std::vector<Archetype*> GetArchetypes(bool allowEmpty = false);

        std::vector<Archetype*> GetArchetypes(const ComponentList& components, bool allowEmpty = false);
        std::vector<Archetype*> GetArchetypes(const ComponentFilter& filter, bool allowEmpty = false);

        template <typename... T> inline EntityIterator<Entity, T...> GetEntityIterator();

        Index EntityCount() const { return m_EntityCount; }
        Index ArchetypeCount() { return m_Archetypes.size(); }

        template <typename T> void AddComponent(Entity& entity);
        void AddComponent(Entity& entity, const ComponentType type);

        template <typename T> void AddComponent(Entity& entity, const T& component);
        void AddComponent(Entity& entity, const ComponentType type, const Byte* data);

        template <typename T> void RemoveComponent(Entity& entity);
        void RemoveComponent(Entity& entity, const ComponentType type);

        template <typename T> T& GetComponent(Entity& entity);
        template <typename T> OptionalRef<T> TryGetComponent(Entity& entity);
        Byte* GetComponent(Entity& entity, const ComponentType type);

        template <typename T> T* AddSystem();

        void UpdateSystems();

      private:
        Index m_EntityIdCounter;
        Index m_EntityCount;

        std::vector<EntityLocation> m_EntityLocations;
        std::stack<Index> m_FreeEntetyLocationIndices;

        ArchetypeMap m_ArchetypeMap;
        std::vector<Archetype> m_Archetypes;

        std::vector<std::shared_ptr<System>> m_Systems;

        Entity GetNextEntity();

        Archetype& CreateArchetype(const ComponentList& components);
        std::pair<Index, Archetype&> GetOrCreateArchetype(const ComponentList& components);
    };

    template <typename... T> inline EntityIterator<Entity, T...> Engine::GetEntityIterator()
    {
        return EntityIterator<Entity, T...>(GetArchetypes(ComponentList::Create<T...>(), false));
    }

    template <typename... T> inline Entity Engine::CreateEntityFromComponents(const T&... components)
    {
        auto data = CombineBytesById(components...);
        return CreateEntity(ComponentList::Create<T...>(), &data[0]);
    }

    template <typename... T> inline std::vector<Archetype*> Engine::GetArchetypes(bool allowEmpty)
    {
        return GetArchetypes(ComponentFilter::Create<T...>(), allowEmpty);
    }

    template <typename T> inline void Engine::AddComponent(Entity& entity) { AddComponent(entity, T::GetType()); }

    template <typename T> inline void Engine::AddComponent(Entity& entity, const T& component)
    {
        AddComponent(entity, T::GetType(), ToBytes(component));
    }

    template <typename T> inline void Engine::RemoveComponent(Entity& entity) { RemoveComponent(entity, T::GetType()); }

    template <typename T> inline T& Engine::GetComponent(Entity& entity)
    {
        const auto& loc = m_EntityLocations[entity.index];
        ECS_ASSERT(entity.id == loc.entityId);
        return GetArchetype(loc.archetype).GetComponent<T>(loc.chunk, loc.position);
    }

    template <typename T> inline OptionalRef<T> Engine::TryGetComponent(Entity& entity)
    {
        const auto& loc = m_EntityLocations[entity.index];
        ECS_ASSERT(entity.id == loc.entityId);
        return GetArchetype(loc.archetype).TryGetComponent<T>(loc.chunk, loc.position);
    }

    template <typename T> inline T* Engine::AddSystem()
    {
        m_Systems.push_back(std::make_shared<T>());
        auto system      = (T*)m_Systems[m_Systems.size() - 1].get();
        system->m_Engine = this;
        system->Init();
        return system;
    }
} // namespace EVA::ECS
