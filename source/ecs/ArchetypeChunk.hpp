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
        template <typename T> inline T& GetComponent(const Index index) { return *FromBytes<T>(GetComponent(T::GetType(), index)); }

        Index AddEntityAddComponent(ComponentType newType, const ArchetypeChunk& chunk, Index indexInChunk, const Byte* data);
        Index AddEntityRemoveComponent(ComponentType removeType, const ArchetypeChunk& chunk, Index indexInChunk);

        Index Count() const { return m_Count; }
        bool Empty() const { return m_Count == 0; }
        bool Full() const { return m_Count == m_ArchetypeInfo.entitiesPerChunk; }

        template <typename T> Iterator<T> begin(std::optional<Index> i)
        {
            if (i.has_value())
                return Iterator<T>(&m_Data[m_ArchetypeInfo.componentInfo[i.value()].start], 0);
            else
                return Iterator<T>(nullptr, 0);
        }

        template <typename T> Iterator<T> end(std::optional<Index> i)
        {
            if (i.has_value())
                return Iterator<T>(
                &m_Data[0] + m_ArchetypeInfo.componentInfo[i.value()].start + m_ArchetypeInfo.componentInfo[i.value()].size * m_Count, m_Count);
            else
                return Iterator<T>(nullptr, m_Count);
        }

        template <typename T> Iterator<T> begin()
        {
            const auto i = m_ArchetypeInfo.GetComponentIndex(optional_inner_type_t<T>::GetType());
            return begin<T>(i);
        }

        template <typename T> Iterator<T> end()
        {
            const auto i = m_ArchetypeInfo.GetComponentIndex(optional_inner_type_t<T>::GetType());
            return end<T>(i);
        }

      private:
        ArchetypeInfo m_ArchetypeInfo;
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
            explicit Iterator(Byte* ptr, Index index) : m_Ptr(FromBytes<T>(ptr)) {}

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

        template <typename T> class Iterator<std::optional<T>>
        {
          public:
            using value_type        = OptionalRef<T>;
            using pointer           = value_type*;
            using reference         = value_type&;
            using difference_type   = Index;
            using iterator_category = std::forward_iterator_tag;

            Iterator() : m_Ptr(nullptr), m_Index(0) {}
            explicit Iterator(Byte* ptr, Index index) : m_Ptr(FromBytes<T>(ptr)), m_Index(index) {}

            bool operator==(const Iterator& other) { return m_Ptr == other.m_Ptr && m_Index == other.m_Index; }
            bool operator!=(const Iterator& other) { return !(*this == other); }

            Iterator& operator++()
            {
                if (m_Ptr != nullptr)
                {
                    m_Ptr++;
                }
                m_Index++;
                return *this;
            }

            const Iterator operator++(int)
            {
                Iterator temp(*this);
                operator++();
                return temp;
            }

            pointer operator->()
            {
                m_Value = OptionalRef<T>(m_Ptr);
                return &m_Value;
            }
            reference operator*()
            {
                m_Value = OptionalRef<T>(m_Ptr);
                return m_Value;
            }

          private:
            Index m_Index;
            T* m_Ptr;
            value_type m_Value;
        };
    };
} // namespace EVA::ECS
