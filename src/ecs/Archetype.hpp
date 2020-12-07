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
        Entity DestroyEntity(const Index chunk, const Index indexInChunk);
        Entity& GetEntity(const Index chunk, const Index indexInChunk);

        Index EntityCount() const { return m_EntityCount; }
        Index ChunkCount() const { return m_Chunks.size(); }
        Index ActiveChunkIndex() const { return m_ActiveChunkIndex; }
        const ArchetypeInfo& GetInfo() const { return m_ArchetypeInfo; }
        const ComponentList& GetComponents() const { return m_Components; }

        std::pair<Index, Index>
        AddEntityAddComponent(Archetype& otherArchetype, const Index otherChunk, const Index otherIndexInChunk, const ComponentType newType, const Byte* data);
        std::pair<Index, Index>
        AddEntityRemoveComponent(Archetype& otherArchetype, const Index otherChunk, const Index otherIndexInChunk, const ComponentType removeType);

        Byte* GetComponent(const ComponentType type, const Index chunk, const Index indexInChunk);
        Byte* GetComponent(const Index archetypeComponentIndex, const Index chunk, const Index indexInChunk);
        template <typename T> inline T& GetComponent(const Index chunk, const Index indexInChunk)
        {
            return *FromBytes<T>(GetComponent(T::GetType(), chunk, indexInChunk));
        }

        Byte* GetComponent(const ComponentType type, const Index index);
        Byte* GetComponent(const Index archetypeComponentIndex, const Index index);
        template <typename T> inline T& GetComponent(const Index index) { return *FromBytes<T>(GetComponent(T::GetType(), index)); }

        template <typename T> Iterator<T> begin()
        {
            Index i = m_ArchetypeInfo.GetComponentIndex(T::GetType());
            return Iterator<T>(m_Chunks.begin(), m_Chunks.begin() + m_ActiveChunkIndex + 1, i);
        }
        template <typename T> Iterator<T> end()
        {
            Index i = m_ArchetypeInfo.GetComponentIndex(T::GetType());
            return Iterator<T>(m_Chunks.begin() + m_ActiveChunkIndex + 1, i);
        }

      private:
        ComponentList m_Components;
        ArchetypeInfo m_ArchetypeInfo;
        Index m_EntityCount;

        ChunkVector m_Chunks;
        ChunkVector::difference_type m_ActiveChunkIndex;

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
