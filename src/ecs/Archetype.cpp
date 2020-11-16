#include "Archetype.hpp"

namespace EVA::ECS
{
    Archetype::Archetype(const ComponentList& components, size_t chunkSize)
    : m_Components(components), m_ArchetypeInfo(components, chunkSize), m_EntityCount(0)
    {
        AddChunk();
    }

    void Archetype::AddChunk()
    {
        m_Chunks.push_back(std::make_unique<ArchetypeChunk>(m_ArchetypeInfo));
        m_CurrentChunk = m_Chunks.end() - 1;
    }

    std::pair<Index, Index> Archetype::CreateEntity(const Entity& entity)
    {
        if ((*m_CurrentChunk)->Full())
        {
            ++m_CurrentChunk;
            if (m_CurrentChunk == m_Chunks.end())
            {
                AddChunk();
            }
        }
        m_EntityCount++;

        auto indexInChunk = (*m_CurrentChunk)->CreateEntity(entity);
        return std::make_pair(ActiveChunkIndex(), indexInChunk);
    }

    Entity Archetype::DestroyEntity(const Index chunk, const Index indexInChunk)
    {
        ECS_ASSERT(m_EntityCount > 0);

        Entity& entity = m_Chunks[chunk]->GetEntity(indexInChunk);

        m_Chunks[chunk]->CopyEntity(indexInChunk, *(*m_CurrentChunk), (*m_CurrentChunk)->Count() - 1);
        (*m_CurrentChunk)->RemoveLast();

        if ((*m_CurrentChunk)->Empty() && m_CurrentChunk != m_Chunks.begin())
        {
            --m_CurrentChunk;
        }

        m_EntityCount--;
        return entity;
    }

    Entity& Archetype::GetEntity(const Index chunk, const Index indexInChunk)
    {
        ECS_ASSERT(m_EntityCount > 0);
        ECS_ASSERT(chunk < m_Chunks.size());
        return m_Chunks[chunk]->GetEntity(indexInChunk);
    }

    Byte* Archetype::GetComponent(const Index archetypeComponentIndex, const Index chunk, const Index indexInChunk)
    {
        ECS_ASSERT(chunk <= ActiveChunkIndex());
        return m_Chunks[chunk]->GetComponent(archetypeComponentIndex, indexInChunk);
    }

    std::pair<Index, Index>
    Archetype::AddEntityAddComponent(Archetype& otherArchetype, Index otherChunk, Index otherIndexInChunk, ComponentType newType, Byte* data)
    {
        if ((*m_CurrentChunk)->Full())
        {
            ++m_CurrentChunk;
            if (m_CurrentChunk == m_Chunks.end())
            {
                AddChunk();
            }
        }
        m_EntityCount++;

        auto indexInChunk = (*m_CurrentChunk)->AddEntityAddComponent(newType, *otherArchetype.m_Chunks[otherChunk], otherIndexInChunk, data);
        return std::make_pair(ActiveChunkIndex(), indexInChunk);
    }

    std::pair<Index, Index> Archetype::AddEntityRemoveComponent(Archetype& otherArchetype, Index otherChunk, Index otherIndexInChunk, ComponentType removeType)
    {
        if ((*m_CurrentChunk)->Full())
        {
            ++m_CurrentChunk;
            if (m_CurrentChunk == m_Chunks.end())
            {
                AddChunk();
            }
        }
        m_EntityCount++;

        auto indexInChunk = (*m_CurrentChunk)->AddEntityRemoveComponent(removeType, *otherArchetype.m_Chunks[otherChunk], otherIndexInChunk);
        return std::make_pair(ActiveChunkIndex(), indexInChunk);
    }

    Byte* Archetype::GetComponent(const ComponentType type, const Index chunk, const Index indexInChunk)
    {
        ECS_ASSERT(chunk <= ActiveChunkIndex());
        return m_Chunks[chunk]->GetComponent(type, indexInChunk);
    }

    Byte* Archetype::GetComponent(const Index archetypeComponentIndex, const Index index)
    {
        Index chunkIndex   = index / m_ArchetypeInfo.entitiesPerChunk;
        Index indexInChunk = index % m_ArchetypeInfo.entitiesPerChunk;
        ECS_ASSERT(chunkIndex <= ActiveChunkIndex());
        return m_Chunks[chunkIndex]->GetComponent(archetypeComponentIndex, indexInChunk);
    }

    Byte* Archetype::GetComponent(const ComponentType type, const Index index)
    {
        Index chunkIndex   = index / m_ArchetypeInfo.entitiesPerChunk;
        Index indexInChunk = index % m_ArchetypeInfo.entitiesPerChunk;
        ECS_ASSERT(chunkIndex <= ActiveChunkIndex());
        return m_Chunks[chunkIndex]->GetComponent(type, indexInChunk);
    }

} // namespace EVA::ECS
