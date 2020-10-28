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
        typedef std::vector<std::unique_ptr<ArchetypeChunk>> ChunkVector;
        template <typename> class Iterator;

        Archetype(const ComponentList& components, size_t chunkSize = defaultChunkSize);

        std::pair<Index, Index> CreateEntity(const Entity& entity);
        Entity DestroyEntity(const Index chunk, const Index indexInChunk);
        Entity& GetEntity(const Index chunk, const Index indexInChunk);

        size_t EntityCount() { return m_EntityCount; }
        size_t ChunkCount() { return m_Chunks.size(); }
        Index ActiveChunkIndex() { return static_cast<Index>(std::distance(m_Chunks.begin(), m_CurrentChunk)); }

        byte* GetComponent(const ComponentType type, const Index chunk, const Index indexInChunk);
        byte* GetComponent(const Index archetypeComponentIndex, const Index chunk, const Index indexInChunk);
        template <typename T> inline T& GetComponent(const Index chunk, const Index indexInChunk)
        {
            return *reinterpret_cast<T*>(GetComponent(T::GetType(), chunk, indexInChunk));
        }

        byte* GetComponent(const ComponentType type, const Index index);
        byte* GetComponent(const Index archetypeComponentIndex, const Index index);
        template <typename T> inline T& GetComponent(const Index index) { return *reinterpret_cast<T*>(GetComponent(T::GetType(), index)); }

        template <typename T> Iterator<T> begin() { return Iterator<T>(m_Chunks.begin(), m_Chunks.end()); }
        template <typename T> Iterator<T> end() { return Iterator<T>(m_Chunks.end(), m_Chunks.end()); }

      private:
        ComponentList m_Components;
        ArchetypeInfo m_ArchetypeInfo;

        size_t m_EntityCount;

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

            Iterator(ChunkVector::iterator chunkIt, ChunkVector::iterator chunksEnd)
            {
                m_ChunksIt  = chunkIt;
                m_ChunksEnd = chunksEnd;
                if (m_ChunksIt != m_ChunksEnd)
                {
                    m_CompIt   = (*m_ChunksIt)->begin<T>();
                    m_CompsEnd = (*m_ChunksIt)->end<T>();
                }
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
                        m_CompIt   = (*m_ChunksIt)->begin<T>();
                        m_CompsEnd = (*m_ChunksIt)->end<T>();
                    }
                    else
                    {
                        m_CompIt   = ArchetypeChunk::Iterator<T>();
                        m_CompsEnd = ArchetypeChunk::Iterator<T>();
                    }
                }
                return *this;
            }

            Iterator operator++(int)
            {
                Iterator temp(*this);
                operator++();
                return temp;
            }

            pointer operator->() { return &(*m_CompIt); }

            reference operator*() { return m_CompIt; }

          private:
            ChunkVector::iterator m_ChunksIt;
            ChunkVector::iterator m_ChunksEnd;
            ArchetypeChunk::Iterator<T> m_CompIt;
            ArchetypeChunk::Iterator<T> m_CompsEnd;
        };
    };
} // namespace EVA::ECS
