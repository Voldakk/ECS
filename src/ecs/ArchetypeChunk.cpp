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
        for (const auto& t : componentList)
        {
            componentInfo[i].type = t;
            componentInfo[i].size = ComponentMap::s_Info[t.Get()].size;
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

    ArchetypeChunk::ArchetypeChunk(ArchetypeInfo archetypeInfo)
    : m_ArchetypeInfo(std::move(archetypeInfo)), m_Count(0), m_Data(m_ArchetypeInfo.chunkSize)
    {
        ECS_ASSERT(memset(&m_Data[0], 0, m_ArchetypeInfo.chunkSize) != nullptr);
    }

    Index ArchetypeChunk::CreateEntity(const Entity& entity)
    {
        ECS_ASSERT(m_Count < m_ArchetypeInfo.entitiesPerChunk);

        // Copy the entity component
        std::memmove(&m_Data[m_Count * sizeof(Entity)], &entity, sizeof(Entity));

        // Starting at 1 to skip Entity
        for (size_t i = 1; i < m_ArchetypeInfo.componentInfo.size(); i++)
        {
            const auto& c = m_ArchetypeInfo.componentInfo[i];
            std::memmove(&m_Data[c.start + m_Count * c.size], ComponentMap::DefaultData(c.type), c.size);
        }

        m_Count++;
        return m_Count - 1;
    }

    void ArchetypeChunk::CopyEntity(Index intoIndex, ArchetypeChunk& fromChunk, Index fromIndex)
    {
        ECS_ASSERT(intoIndex < m_ArchetypeInfo.entitiesPerChunk);
        for (const auto& c : m_ArchetypeInfo.componentInfo)
        {
            std::memmove(&m_Data[c.start + intoIndex * c.size], &fromChunk.m_Data[c.start + fromIndex * c.size], c.size);
        }
    }

    Entity& ArchetypeChunk::GetEntity(const Index index)
    {
        ECS_ASSERT(index < m_Count);
        return *reinterpret_cast<Entity*>(&m_Data[index * sizeof(Entity)]);
    }

    void ArchetypeChunk::RemoveLast()
    {
        ECS_ASSERT(m_Count > 0);
        m_Count--;
    }

    Byte* ArchetypeChunk::GetComponent(const Index archetypeComponentIndex, const Index index)
    {
        ECS_ASSERT(index < m_Count);
        return &m_Data[m_ArchetypeInfo.componentInfo[archetypeComponentIndex].start +
        index * m_ArchetypeInfo.componentInfo[archetypeComponentIndex].size];
    }

    Index ArchetypeChunk::AddEntityAddComponent(ComponentType newType, const ArchetypeChunk& chunk, Index indexInChunk, const Byte* data)
    {
        ECS_ASSERT(m_Count < m_ArchetypeInfo.entitiesPerChunk);

        size_t offset = 0;
        for (size_t i = 0; i < m_ArchetypeInfo.componentInfo.size(); i++)
        {
            const auto& comp = m_ArchetypeInfo.componentInfo[i];
            const auto size  = comp.size;
            if (comp.type == newType)
            {
                std::memmove(&m_Data[comp.start + m_Count * size], data, size);
                offset--;
            }
            else
            {
                ECS_ASSERT(comp.type == chunk.m_ArchetypeInfo.componentInfo[i + offset].type);
                std::memmove(&m_Data[comp.start + m_Count * size],
                &chunk.m_Data[chunk.m_ArchetypeInfo.componentInfo[i + offset].start + indexInChunk * size], size);
            }
        }

        return m_Count++;
    }

    Index ArchetypeChunk::AddEntityRemoveComponent(ComponentType removeType, const ArchetypeChunk& chunk, Index indexInChunk)
    {
        ECS_ASSERT(m_Count < m_ArchetypeInfo.entitiesPerChunk);

        size_t offset = 0;
        for (size_t i = 0; i < chunk.m_ArchetypeInfo.componentInfo.size(); i++)
        {
            const auto& comp = chunk.m_ArchetypeInfo.componentInfo[i];
            const auto size  = comp.size;
            if (comp.type == removeType)
            {
                offset--;
            }
            else
            {
                ECS_ASSERT(m_ArchetypeInfo.componentInfo[i + offset].type == comp.type);
                std::memmove(&m_Data[m_ArchetypeInfo.componentInfo[i + offset].start + m_Count * size],
                &chunk.m_Data[comp.start + indexInChunk * size], size);
            }
        }

        return m_Count++;
    }

    Byte* ArchetypeChunk::GetComponent(const ComponentType type, const Index index)
    {
        ECS_ASSERT(index < m_Count);
        Index i = m_ArchetypeInfo.GetComponentIndex(type);
        return &m_Data[m_ArchetypeInfo.componentInfo[i].start + index * m_ArchetypeInfo.componentInfo[i].size];
    }

} // namespace EVA::ECS
