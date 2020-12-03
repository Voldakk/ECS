#pragma once

#include <memory>
#include <utility>

#include "ArchetypeChunk.hpp"
#include "Component.hpp"

namespace EVA::ECS
{
    class Archetype
    {
      public:
        using ChunkVector = std::vector<std::shared_ptr<ArchetypeChunk>>;
        template <typename> class Iterator;

        explicit Archetype(const ComponentList& components, size_t chunkSize = DefaultChunkSize);

        std::pair<Index, Index> CreateEntity(const Entity& entity);
        Entity DestroyEntity(Index chunk, Index indexInChunk);
        Entity& GetEntity(Index chunk, Index indexInChunk);

        Index EntityCount() const { return m_EntityCount; }
        Index ChunkCount() { return m_Chunks.size(); }
        Index ActiveChunkIndex() { return static_cast<Index>(std::distance(m_Chunks.begin(), m_CurrentChunk)); }

        std::pair<Index, Index>
        AddEntityAddComponent(Archetype& otherArchetype, Index otherChunk, Index otherIndexInChunk, ComponentType newType, Byte* data);
        std::pair<Index, Index> AddEntityRemoveComponent(Archetype& otherArchetype, Index otherChunk, Index otherIndexInChunk, ComponentType removeType);

        Byte* GetComponent(ComponentType type, Index chunk, Index indexInChunk);
        Byte* GetComponent(Index archetypeComponentIndex, Index chunk, Index indexInChunk);
        template <typename T> inline T& GetComponent(const Index chunk, const Index indexInChunk)
        {
            return *reinterpret_cast<T*>(GetComponent(T::GetType(), chunk, indexInChunk));
        }

        Byte* GetComponent(ComponentType type, Index index);
        Byte* GetComponent(Index archetypeComponentIndex, Index index);
        template <typename T> inline T& GetComponent(const Index index) { return *reinterpret_cast<T*>(GetComponent(T::GetType(), index)); }

        template <typename T> Iterator<T> begin()
        {
            Index i = m_ArchetypeInfo.GetComponentIndex(T::GetType());
            return Iterator<T>(m_Chunks.begin(), m_Chunks.end(), i);
        }
        template <typename T> Iterator<T> end()
        {
            Index i = m_ArchetypeInfo.GetComponentIndex(T::GetType());
            return Iterator<T>(m_Chunks.end(), i);
        }

      private:
        ComponentList m_Components;
        ArchetypeInfo m_ArchetypeInfo;
        Index m_EntityCount;

        ChunkVector m_Chunks;
        ChunkVector::iterator m_CurrentChunk;

        void AddChunk();

      public:
        template <typename T> class Iterator
        {
          public:
            using value_type        = T;
            using pointer           = value_type*;
            using reference         = value_type&;
            using difference_type   = Index;
            using iterator_category = std::forward_iterator_tag;

            Iterator()
            : m_Index(0), m_ChunksIt(ChunkVector::iterator()), m_ChunksEnd(ChunkVector::iterator()),
              m_CompIt(ArchetypeChunk::Iterator<T>()), m_CompsEnd(ArchetypeChunk::Iterator<T>())
            {
            }

            Iterator(ChunkVector::iterator chunksEnd, Index index) : m_Index(index), m_ChunksIt(chunksEnd), m_ChunksEnd(chunksEnd) {}

            Iterator(ChunkVector::iterator chunkIt, ChunkVector::iterator chunksEnd, Index index)
            : m_Index(index), m_ChunksIt(chunkIt), m_ChunksEnd(chunksEnd), m_CompIt((*m_ChunksIt)->begin<T>(m_Index)),
              m_CompsEnd((*m_ChunksIt)->end<T>(m_Index))
            {
            }

            bool operator==(const Iterator& other) { return m_ChunksIt == other.m_ChunksIt && m_CompIt == other.m_CompIt; }

            bool operator!=(const Iterator& other) { return !(*this == other); }

            Iterator& operator++()
            {
                ++m_CompIt;
                if (m_CompIt == m_CompsEnd)
                {
                    ++m_ChunksIt;
                    if (m_ChunksIt != m_ChunksEnd)
                    {
                        m_CompIt   = (*m_ChunksIt)->begin<T>(m_Index);
                        m_CompsEnd = (*m_ChunksIt)->end<T>(m_Index);
                    }
                    else
                    {
                        m_CompIt   = ArchetypeChunk::Iterator<T>();
                        m_CompsEnd = ArchetypeChunk::Iterator<T>();
                    }
                }
                return *this;
            }

            const Iterator operator++(int)
            {
                Iterator temp(*this);
                operator++();
                return temp;
            }

            pointer operator->() { return &(*m_CompIt); }

            reference operator*() { return *m_CompIt; }

          private:
            Index m_Index;
            ChunkVector::iterator m_ChunksIt;
            ChunkVector::iterator m_ChunksEnd;
            ArchetypeChunk::Iterator<T> m_CompIt;
            ArchetypeChunk::Iterator<T> m_CompsEnd;
        };
    };
} // namespace EVA::ECS
