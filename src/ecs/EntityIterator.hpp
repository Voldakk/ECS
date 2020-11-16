#pragma once

#include <vector>

#include "Archetype.hpp"
#include "Component.hpp"
#include "Core.hpp"

namespace EVA::ECS
{
    using ArchetypeContainer = std::vector<Archetype*>;
    using ArchetypeIterator  = ArchetypeContainer::iterator;

    template <typename... T> class EntityIterator
    {
      public:
        class Iterator;

        explicit EntityIterator(const ArchetypeContainer& archetypes)
        : m_Components(ComponentList::Create<T...>()), m_Archetypes(archetypes)
        {
        }

        Iterator begin() { return Iterator(m_Archetypes.begin(), m_Archetypes.end()); }

        Iterator end() { return Iterator(m_Archetypes.end()); }

      private:
        ComponentList m_Components;
        ArchetypeContainer m_Archetypes;

      public:
        class Iterator
        {
          public:
            using value_type        = std::tuple<T&...>;
            using pointer           = value_type*;
            using reference         = value_type&;
            using difference_type   = Index;
            using iterator_category = std::forward_iterator_tag;

            explicit Iterator(ArchetypeIterator end)
            : m_ArchetypeIterator(end), m_ArchetypeEnd(end), m_Iterators(std::tuple<Archetype::Iterator<T>...>((Archetype::Iterator<T>())...))
            {
            }

            Iterator(ArchetypeIterator begin, ArchetypeIterator end)
            : m_ArchetypeIterator(begin), m_ArchetypeEnd(end), m_Iterators(std::tuple<Archetype::Iterator<T>...>(((*begin)->begin<T>())...)),
              m_IteratorsEnd(std::tuple<Archetype::Iterator<T>...>(((*begin)->end<T>())...))
            {
            }

            bool operator==(const Iterator& other)
            {
                return m_ArchetypeIterator == other.m_ArchetypeIterator && std::get<0>(m_Iterators) == std::get<0>(other.m_Iterators);
            }
            bool operator!=(const Iterator& other) { return !(*this == other); }

            Iterator& operator++()
            {
                (++(std::get<Archetype::Iterator<T>>(m_Iterators)), ...);

                if (std::get<0>(m_Iterators) == std::get<0>(m_IteratorsEnd))
                {
                    ++m_ArchetypeIterator;
                    if (m_ArchetypeIterator == m_ArchetypeEnd)
                    {
                        m_Iterators = std::tuple<Archetype::Iterator<T>...>((Archetype::Iterator<T>())...);
                    }
                    else
                    {
                        m_Iterators    = std::tuple<Archetype::Iterator<T>...>(((*m_ArchetypeIterator)->begin<T>())...);
                        m_IteratorsEnd = std::tuple<Archetype::Iterator<T>...>(((*m_ArchetypeIterator)->end<T>())...);
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

            value_type operator*() { return value_type(((*(std::get<Archetype::Iterator<T>>(m_Iterators))))...); }

          private:
            ArchetypeIterator m_ArchetypeIterator;
            ArchetypeIterator m_ArchetypeEnd;
            std::tuple<Archetype::Iterator<T>...> m_Iterators;
            std::tuple<Archetype::Iterator<T>...> m_IteratorsEnd;
        };
    };
} // namespace EVA::ECS
