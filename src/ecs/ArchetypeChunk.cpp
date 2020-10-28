#include "ArchetypeChunk.hpp"

#include <cstring>

namespace EVA::ECS
{
    // ArchetypeInfo

    ArchetypeInfo::ArchetypeInfo(const ComponentList& componentList, size_t _chunkSize) : chunkSize(_chunkSize)
    {
        componentInfo.resize(componentList.size() + 1); // +1 for the required entity component

        componentInfo[0].type = Entity::GetType();
        componentInfo[0].size = sizeof(Entity);
        entitySize            = sizeof(Entity);

        size_t i = 1;
        for (auto& t : componentList)
        {
            componentInfo[i].type = t;
            componentInfo[i].size = ComponentMap::s_Info[t].size;
            entitySize += componentInfo[i].size;
            i++;
        }

        entitiesPerChunk = chunkSize / entitySize;

        size_t offset = 0;
        for (auto& c : componentInfo)
        {
            c.start = offset;
            offset += c.size * entitiesPerChunk;
        }
    }

    Index ArchetypeInfo::GetComponentIndex(const ComponentType type) const
    {
        auto it = std::find_if(componentInfo.begin(), componentInfo.end(), ComponentInfo::Predicate(type));
        ECS_ASSERT(it != componentInfo.end());
        return static_cast<Index>(std::distance(componentInfo.begin(), it));
    }

    // ArchetypeChunk

    ArchetypeChunk::ArchetypeChunk(const ArchetypeInfo& archetypeInfo) : m_ArchetypeInfo(archetypeInfo)
    {
        m_Count = 0;
        m_Data  = reinterpret_cast<byte*>(malloc(m_ArchetypeInfo.chunkSize));

        ECS_ASSERT(m_Data != nullptr);
        ECS_ASSERT(memset(m_Data, 0, m_ArchetypeInfo.chunkSize) != nullptr);
    }

    ArchetypeChunk::~ArchetypeChunk() { free(m_Data); }

    Index ArchetypeChunk::CreateEntity(const Entity& entity)
    {
        ECS_ASSERT(m_Count < m_ArchetypeInfo.entitiesPerChunk);

        // Copy the entity component
        std::memmove(m_Data + m_Count * sizeof(Entity), &entity, sizeof(Entity));

        // Starting at 1 to skip Entity
        for (size_t i = 1; i < m_ArchetypeInfo.componentInfo.size(); i++)
        {
            auto c = m_ArchetypeInfo.componentInfo[i];
            std::memmove(m_Data + c.start + m_Count * c.size, ComponentMap::DefaultData(c.type), c.size);
        }

        m_Count++;
        return m_Count - 1;
    }

    void ArchetypeChunk::CopyEntity(Index intoIndex, ArchetypeChunk& fromChunk, Index fromIndex)
    {
        ECS_ASSERT(intoIndex < m_ArchetypeInfo.entitiesPerChunk);
        for (auto& c : m_ArchetypeInfo.componentInfo)
        {
            std::memmove(m_Data + c.start + intoIndex * c.size, fromChunk.m_Data + c.start + fromIndex * c.size, c.size);
        }
    }

    Entity& ArchetypeChunk::GetEntity(const Index index)
    {
        ECS_ASSERT(index < m_Count);
        return *reinterpret_cast<Entity*>(m_Data + index * sizeof(Entity));
    }

    void ArchetypeChunk::RemoveLast()
    {
        ECS_ASSERT(m_Count > 0);
        m_Count--;
    }

    byte* ArchetypeChunk::GetComponent(const Index archetypeComponentIndex, const Index index)
    {
        ECS_ASSERT(index < m_Count);
        return m_Data + m_ArchetypeInfo.componentInfo[archetypeComponentIndex].start +
        index * m_ArchetypeInfo.componentInfo[archetypeComponentIndex].size;
    }

    byte* ArchetypeChunk::GetComponent(const ComponentType type, const Index index)
    {
        ECS_ASSERT(index < m_Count);
        Index i = m_ArchetypeInfo.GetComponentIndex(type);
        return m_Data + m_ArchetypeInfo.componentInfo[i].start + index * m_ArchetypeInfo.componentInfo[i].size;
    }

} // namespace EVA::ECS
