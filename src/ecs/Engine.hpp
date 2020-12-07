#pragma once

#include <optional>
#include <stack>
#include <unordered_map>

#include "Archetype.hpp"
#include "Component.hpp"
#include "Core.hpp"
#include "EntityIterator.hpp"

namespace EVA::ECS
{
    class System;
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

        template <typename... T> std::vector<Archetype*> GetArchetypes(bool allowEmpty = false);
        std::vector<Archetype*> GetArchetypes(const ComponentList& components, bool allowEmpty = false);

        Index EntityCount() const { return m_EntityCount; }
        Index ArchetypeCount() { return m_Archetypes.size(); }

        template <typename T> void AddComponent(Entity& entity);
        void AddComponent(Entity& entity, const ComponentType type);

        template <typename T> void AddComponent(Entity& entity, const T& component);
        void AddComponent(Entity& entity, const ComponentType type, const Byte* data);

        template <typename T> void RemoveComponent(Entity& entity);
        void RemoveComponent(Entity& entity, const ComponentType type);

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

    template <typename... T> inline std::vector<Archetype*> Engine::GetArchetypes(bool allowEmpty)
    {
        return GetArchetypes(ComponentList::Create<T...>(), allowEmpty);
    }

    template <typename T> inline void Engine::AddComponent(Entity& entity) { AddComponent(entity, T::GetType()); }

    template <typename T> inline void Engine::AddComponent(Entity& entity, const T& component)
    {
        AddComponent(entity, T::GetType(), ToBytes(component));
    }

    template <typename T> inline void Engine::RemoveComponent(Entity& entity) { RemoveComponent(entity, T::GetType()); }

    template <typename T> inline T* Engine::AddSystem()
    {
        m_Systems.push_back(std::make_shared<T>());
        auto system      = (T*)m_Systems[m_Systems.size() - 1].get();
        system->m_Engine = this;
        system->Init();
        return system;
    }

    class System
    {
        friend Engine;

      public:
        virtual inline void Init() {}
        virtual inline void Update() {}

      protected:
        Engine* m_Engine;

        template <typename... T> inline EntityIterator<Entity, T...> GetEntityIterator()
        {
            return EntityIterator<Entity, T...>(m_Engine->GetArchetypes(ComponentList::Create<T...>(), false));
        }
    };
} // namespace EVA::ECS
