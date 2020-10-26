#pragma once

#include <cstddef>

#include "Component.hpp"
#include "Core.hpp"

namespace EVA::ECS
{
    constexpr size_t chunkSize = 16384;
    typedef size_t Index;

    /* Data layout examples
     * E = Entity
     * A = ComponentB
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
        ComponentType type = 0;
        size_t size        = 0;
        size_t start       = 0;

        // Allow finding a ComponentInfo by the ComponentType
        struct Predicate
        {
            ComponentType t;
            explicit Predicate(ComponentType t) : t(t) {}
            inline bool operator()(const ComponentInfo& c) const
            {
                return c.type == t;
            }
        };
    };
    struct ArchetypeInfo
    {
        size_t entitySize       = 0;
        size_t entitiesPerChunk = 0;
        std::vector<ComponentInfo> componentInfo;

        ArchetypeInfo(const ComponentList& componentList);
    };

    class ArchetypeChunk
    {
      public:
        class Iterator;

      private:
        const ArchetypeInfo& m_ArchetypeInfo;
        byte* m_Data;
        Index m_Count;

      public:
        ArchetypeChunk(const ArchetypeInfo& archetypeInfo);
        ~ArchetypeChunk();

        Index CreateEntity(const Entity& entity);
        void CopyEntity(const Index intoIndex, ArchetypeChunk& fromChunk, const Index fromIndex);
        Entity& GetEntity(const Index index);
        void RemoveLast();

        Index GetComponentIndex(const ComponentType type) const;
        byte* GetComponent(const ComponentType type, const Index index);
        byte* GetComponent(const Index archetypeComponentIndex, const Index index);

        template <typename T> inline T& GetComponent(const Index index)
        {
            return *reinterpret_cast<T*>(GetComponent(T::GetType(), index));
        }

        Index Count() const
        {
            return m_Count;
        }
    };

} // namespace EVA::ECS
