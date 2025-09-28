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
        std::pair<Index, Index> CreateEntity(const Entity& entity, const Byte* data);
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

        template <typename T> inline OptionalRef<T> TryGetComponent(const Index chunk, const Index indexInChunk)
        {
            const auto i = m_ArchetypeInfo.GetComponentIndex(T::GetType());
            if (!i.has_value())
                return std::nullopt;
            return *FromBytes<T>(GetComponent(T::GetType(), chunk, indexInChunk));
        }

        Byte* GetComponent(const ComponentType type, const Index index);
        Byte* GetComponent(const Index archetypeComponentIndex, const Index index);
        template <typename T> inline T& GetComponent(const Index index) { return *FromBytes<T>(GetComponent(T::GetType(), index)); }

        ChunkVector m_Chunks;

      private:
        ComponentList m_Components;
        ArchetypeInfo m_ArchetypeInfo;
        Index m_EntityCount;

        ChunkVector::difference_type m_ActiveChunkIndex;

        void AddChunk();
        void ReserveChunk();
    };
} // namespace EVA::ECS
