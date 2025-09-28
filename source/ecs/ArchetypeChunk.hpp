#pragma once

#include "Component.hpp"
#include "Core.hpp"
#include "OptionalRef.hpp"

namespace EVA::ECS
{
    /* Data layout examples
     * E = Entity
     * A = ComponentA
     * B = ComponentB
     * - = Unused
     *
     * Data: [EEEEEEEEAAAAAAAABBBBBBBB]
     *
     * Data: [EEEEE---AAAAA---BBBBB---]
     *        ^componentInfo[0].start
     *                ^componentInfo[1].start
     *                        ^componentInfo[2].start
     */

    struct ComponentInfo
    {
        ComponentType type{ 0 };
        size_t size{ 0 };
        size_t start{ 0 };

        // Allow finding a ComponentInfo by the ComponentType
        struct Predicate
        {
            ComponentType t;
            explicit Predicate(ComponentType _t) : t(_t) {}
            inline bool operator()(const ComponentInfo& c) const { return c.type == t; }
        };
    };

    struct ArchetypeInfo
    {
        size_t chunkSize{ 0 };
        size_t entitySize{ 0 };
        size_t entitiesPerChunk{ 0 };
        std::vector<ComponentInfo> componentInfo;

        explicit ArchetypeInfo(const ComponentList& componentList, size_t _chunkSize = DefaultChunkSize);
        std::optional<Index> GetComponentIndex(ComponentType type) const;
    };

    class ArchetypeChunk
    {
      public:
        template <typename> class Iterator;

        explicit ArchetypeChunk(ArchetypeInfo archetypeInfo);

        Index CreateEntity(const Entity& entity);
        Index CreateEntity(const Entity& entity, const Byte* data);
        void CopyEntity(Index intoIndex, ArchetypeChunk& fromChunk, Index fromIndex);
        Entity& GetEntity(Index index);
        void RemoveLast();

        Byte* GetComponent(ComponentType type, Index index);
        Byte* GetComponent(Index archetypeComponentIndex, Index index);

        template <typename T>
        inline auto GetComponent(std::optional<Index> archetypeComponentIndex, const Index index) -> optional_ref_transform_t<T>
        {
            if constexpr (is_std_optional_v<T>)
            {
                using U = typename T::value_type;

                if (!archetypeComponentIndex.has_value())
                    return std::nullopt;
                return OptionalRef<U>(FromBytes<U>(GetComponent(archetypeComponentIndex.value(), index)));
            }
            else
            {
                return *FromBytes<T>(GetComponent(archetypeComponentIndex.value(), index));
            }
        }

        Index AddEntityAddComponent(ComponentType newType, const ArchetypeChunk& chunk, Index indexInChunk, const Byte* data);
        Index AddEntityRemoveComponent(ComponentType removeType, const ArchetypeChunk& chunk, Index indexInChunk);

        inline Index Count() const { return m_Count; }
        inline bool Empty() const { return m_Count == 0; }
        inline bool Full() const { return m_Count == m_ArchetypeInfo.entitiesPerChunk; }

      private:
        ArchetypeInfo m_ArchetypeInfo;
        Index m_Count;
        std::vector<Byte> m_Data;
    };
} // namespace EVA::ECS
