#pragma once

#include <cstddef>

#include "Component.hpp"
#include "Core.hpp"

namespace EVA::ECS
{
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
        Index GetComponentIndex(ComponentType type) const;
    };

    class ArchetypeChunk
    {
      public:
        template <typename> class Iterator;

      
        explicit ArchetypeChunk(const ArchetypeInfo& archetypeInfo);

        Index CreateEntity(const Entity& entity);
        void CopyEntity(Index intoIndex, ArchetypeChunk& fromChunk, Index fromIndex);
        Entity& GetEntity(Index index);
        void RemoveLast();

        Byte* GetComponent(ComponentType type, Index index);
        Byte* GetComponent(Index archetypeComponentIndex, Index index);
        template <typename T> inline T& GetComponent(const Index index) { return *reinterpret_cast<T*>(GetComponent(T::GetType(), index)); }

        Index Count() const { return m_Count; }
        bool Empty() const { return m_Count == 0; }
        bool Full() const { return m_Count == m_ArchetypeInfo.entitiesPerChunk; }

        template <typename T> Iterator<T> begin()
        {
            Index i = m_ArchetypeInfo.GetComponentIndex(T::GetType());
            return Iterator<T>(&m_Data[0] + m_ArchetypeInfo.componentInfo[i].start);
        }
        template <typename T> Iterator<T> end()
        {
            Index i = m_ArchetypeInfo.GetComponentIndex(T::GetType());
            return Iterator<T>(&m_Data[0] + m_ArchetypeInfo.componentInfo[i].start + m_ArchetypeInfo.componentInfo[i].size * m_Count);
        }

      private:
        const ArchetypeInfo& m_ArchetypeInfo;
        Index m_Count;
        std::vector<Byte> m_Data;

      public:
        template <typename T> class Iterator
        {
          public:
            using value_type        = T;
            using pointer           = value_type*;
            using reference         = value_type&;
            using difference_type   = Index;
            using iterator_category = std::forward_iterator_tag;

            Iterator() : m_Ptr(nullptr) {}
            explicit Iterator(Byte* ptr) : m_Ptr(reinterpret_cast<T*>(ptr)) {}

            bool operator==(const Iterator& other) { return m_Ptr == other.m_Ptr; }

            bool operator!=(const Iterator& other) { return !(*this == other); }

            Iterator& operator++()
            {
                m_Ptr++;
                return *this;
            }

            const Iterator operator++(int)
            {
                Iterator temp(*this);
                operator++();
                return temp;
            }

            pointer operator->() { return m_Ptr; }

            reference operator*() { return *m_Ptr; }

          private:
            pointer m_Ptr;
        };
    };
} // namespace EVA::ECS
