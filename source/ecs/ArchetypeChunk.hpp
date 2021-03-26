#pragma once

#include "Component.hpp"
#include "Core.hpp"

namespace EVA::ECS
{
    template <typename... T> class ArchetypeChunk
    {
      public:
        template <typename> class Iterator;
        
        explicit ArchetypeChunk(Index size) : m_Size(0), m_Count(0) 
        { 
            std::get<std::vector<Entity>>(m_Components).resize(m_Size);
            (std::get<std::vector<T>>(m_Components).resize(m_Size), ...); 
        }

        Index CreateEntity(const Entity& entity);
        Index CreateEntity(const Entity& entity, const std::tuple<T...> components);
        Entity& GetEntity(Index index);
        void RemoveLast();

        template <typename U> U& GetComponent(const Index index);


        template <typename... U>
        void CopyFrom(const ArchetypeChunk<U...>& fromChunk, Index fromIndex, Index toIndex);

        template <typename... U> void CopyTo(ArchetypeChunk<U...>& toChunk, Index toIndex, Index fromIndex);

        template <typename U> Index AddEntityAddComponent(const ArchetypeChunk& fromChunk, const Index fromIndex, const U& newComponent);
        Index AddEntityRemoveComponent(const ArchetypeChunk& fromChunk, const Index fromIndex);

        Index Count() const { return m_Count; }
        bool Empty() const { return m_Count == 0; }
        bool Full() const { return m_Count == m_Size; }

        /*template <typename T> Iterator<T> begin()
        {
            Index i = m_ArchetypeInfo.GetComponentIndex(T::GetType());
            return Iterator<T>(&m_Data[m_ArchetypeInfo.componentInfo[i].start]);
        }
        template <typename T> Iterator<T> end()
        {
            Index i = m_ArchetypeInfo.GetComponentIndex(T::GetType());
            return Iterator<T>(&m_Data[0] + m_ArchetypeInfo.componentInfo[i].start + m_ArchetypeInfo.componentInfo[i].size * m_Count);
        }

        template <typename T> Iterator<T> begin(Index i) { return Iterator<T>(&m_Data[m_ArchetypeInfo.componentInfo[i].start]); }
        template <typename T> Iterator<T> end(Index i)
        {
            return Iterator<T>(&m_Data[0] + m_ArchetypeInfo.componentInfo[i].start + m_ArchetypeInfo.componentInfo[i].size * m_Count);
        }*/

      private:
        Index m_Count;
        Index m_Size;
        std::tuple<std::vector<Entity>, std::vector<T>...> m_Components;

      public:
        template <typename U> class Iterator
        {
          public:
            using value_type        = U;
            using pointer           = value_type*;
            using reference         = value_type&;
            using difference_type   = Index;
            using iterator_category = std::forward_iterator_tag;

            Iterator() : m_Ptr(nullptr) {}
            explicit Iterator(Byte* ptr) : m_Ptr(FromBytes<value_type>(ptr)) {}

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
