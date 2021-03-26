#include "ArchetypeChunk.hpp"

#include <cstring>

namespace EVA::ECS
{
    // ArchetypeChunk 

    template <typename... T> Index ArchetypeChunk<T...>::CreateEntity(const Entity& entity)
    {
        ECS_ASSERT(m_Count < m_Size);
        // Copy the entity component
        std::get<std::vector<Entity>>(m_Components)[m_Count] = entity;
        // Reset the other components
        ((std::get<std::vector<T>>(m_Components)[m_Count] = T()), ...);

        m_Count++;
        return m_Count - 1;
    }

    template <typename... T> 
    Index ArchetypeChunk<T...>::CreateEntity(const Entity& entity, const std::tuple<T...> components)
    {
        ECS_ASSERT(m_Count < m_Size);
        // Copy the entity component
        std::get<std::vector<Entity>>(m_Components)[m_Count] = entity;
        // Copy the other components
        ((std::get<std::vector<T>>(m_Components)[m_Count] = std::get<T>(components)), ...);

        m_Count++;
        return m_Count - 1;
    }

    template <typename... T> 
    Entity& ArchetypeChunk<T...>::GetEntity(const Index index)
    {
        ECS_ASSERT(index < m_Count);
        return std::get<std::vector<Entity>>(m_Components)[index];
    }

    template <typename... T> 
    void ArchetypeChunk<T...>::RemoveLast()
    {
        ECS_ASSERT(m_Count > 0);
        m_Count--;
    }

    template <typename... T> template <typename U> inline U& ArchetypeChunk<T...>::GetComponent(const Index index)
    {
        return std::get<std::vector<U>>(m_Components)[index];
    }

    template <typename... T>
    template <typename... U>
    void ArchetypeChunk<T...>::CopyFrom(const ArchetypeChunk<U...>& fromChunk, Index fromIndex, Index toIndex)
    {
        ECS_ASSERT(intoIndex < m_Size);
        // Copy the entity component
        std::get<std::vector<Entity>>(m_Components)[toIndex] = std::get<std::vector<Entity>>(fromChunk.m_Components)[fromIndex];
        // Copy the other components
        ((std::get<std::vector<T>>(m_Components)[toIndex] = std::get<std::vector<T>>(fromChunk.m_Components)[fromIndex]), ...);
    }

    template <typename... T>
    template <typename... U>
    void ArchetypeChunk<T...>::CopyTo(ArchetypeChunk<U...>& toChunk, Index toIndex, Index fromIndex)
    {
        ECS_ASSERT(intoIndex < m_Size);
        // Copy the entity component
        std::get<std::vector<Entity>>(toChunk.m_Components)[toIndex] = std::get<std::vector<Entity>>(m_Components)[fromIndex];
        // Copy the other components
        ((std::get<std::vector<T>>(toChunk.m_Components)[toIndex] = std::get<std::vector<T>>(m_Components)[fromIndex]), ...);
    }

    template <typename... T>
    template <typename U>
    Index ArchetypeChunk<T...>::AddEntityAddComponent(const ArchetypeChunk& fromChunk, const Index fromIndex, const U& newComponent)
    {
        ECS_ASSERT(m_Count < m_Size);

        fromChunk.CopyTo(&this, m_Count, fromIndex);
        std::get<std::vector<U>>(m_Components)[m_Count] = newComponent;

        return m_Count++;
    }

    template <typename... T> 
    Index ArchetypeChunk<T...>::AddEntityRemoveComponent(const ArchetypeChunk& fromChunk, const Index fromIndex)
    {
        ECS_ASSERT(m_Count < m_Size);

        CopyFrom(fromChunk, fromIndex, m_Count);

        return m_Count++;
    }

} // namespace EVA::ECS
