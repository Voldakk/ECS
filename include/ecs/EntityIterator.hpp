#pragma once

#include <execution>
#include <thread>
#include <vector>

#include "Archetype.hpp"
#include "CommandQueue.hpp"
#include "Component.hpp"
#include "Core.hpp"

#ifndef EVA_ECS_PROFILE_FUNCTION
#define EVA_ECS_PROFILE_FUNCTION()
#endif

#ifndef EVA_ECS_PROFILE_SCOPE
#define EVA_ECS_PROFILE_SCOPE(NAME)
#endif

namespace EVA::ECS
{
    template <typename T> struct ArchtypeCompIndex
    {
        std::optional<Index> index;
    };

    template <typename T> struct index_transform
    {
        using type = ArchtypeCompIndex<T>;
    };

    template <typename T> using index_transform_t = typename index_transform<T>::type;

    using ArchetypeIterator = std::vector<Archetype*>::iterator;

    template <typename... T> class EntityIterator
    {
        using CompIndices = std::tuple<index_transform_t<T>...>;
        using value_type  = std::tuple<optional_ref_transform_t<T>...>;

        struct ChunkInfo
        {
            Index begin;
            Index end;
            Archetype* a;
            ArchetypeChunk* c;
            CompIndices comp_indices;
        };

        std::vector<ChunkInfo> m_Chunks;
        std::vector<Archetype*> m_Archetypes;

      public:
        explicit EntityIterator(const std::vector<Archetype*>& archetypes) : m_Archetypes(archetypes)
        {
            Index count = 0;
            CompIndices comp_indices;

            for (auto a : m_Archetypes)
            {
                ((std::get<index_transform_t<T>>(comp_indices).index = a->GetInfo().GetComponentIndex(optional_inner_type_t<T>::GetType())), ...);

                for (auto c : a->m_Chunks)
                {
                    if (c->Empty())
                        break;

                    Index begin = count;
                    count += c->Count();

                    m_Chunks.emplace_back(begin, count, a, c.get(), comp_indices);
                }
            }
        }

        Index ArchetypeCount() { return m_Archetypes.size(); }
        Index Count() const
        {
            Index count = 0;
            for (auto& a : m_Archetypes)
            {
                count += a->EntityCount();
            }
            return count;
        }

        bool Empty() const
        {
            for (auto& a : m_Archetypes)
            {
                if (a->EntityCount() > 0)
                    return false;
            }
            return true;
        }

        value_type operator[](Index i) const
        {
            const ChunkInfo* ci = nullptr;
            for (const ChunkInfo& info : m_Chunks)
            {
                if (info.end > i)
                {
                    ci = &info;
                    break;
                }
            }

            const Index index_in_chunk = i - ci->begin;
            return value_type(ci->c->GetComponent<T>(std::get<index_transform_t<T>>(ci->comp_indices).index, index_in_chunk)...);
        }


        class Iterator;
        Iterator begin() { return Iterator(0, m_Chunks); }
        Iterator end() { return Iterator(Count(), m_Chunks); }

        auto Split(size_t num_chunks)
        {
            std::vector<std::pair<Iterator, Iterator>> iterators;
            size_t n          = Count();
            size_t chunk_size = (n + num_chunks - 1) / num_chunks;

            for (size_t i = 0; i < n; i += chunk_size)
            {
                auto b = Iterator(i, m_Chunks);
                auto e = Iterator(i + std::min(chunk_size, n - i), m_Chunks);
                iterators.emplace_back(b, e);
            }

            return iterators;
        }

        template <typename Func> void Process(size_t num_chunks, Func&& func)
        {
            auto iterators = Split(num_chunks);
            std::for_each(std::execution::par_unseq, iterators.begin(), iterators.end(),
            [&](auto& range)
            {
                EVA_ECS_PROFILE_SCOPE("Parallel");
                for (auto it = range.first; it != range.second; ++it)
                {
                    func(*it);
                }
            });
        }

        template <typename Func> void ProcessWithCQ(size_t num_chunks, Engine& engine, Func&& func)
        {
            auto iterators = Split(num_chunks);
            std::vector<CommandQueue> queues(iterators.size());

            std::for_each(std::execution::par_unseq, iterators.begin(), iterators.end(),
            [&](auto& range)
            {
                EVA_ECS_PROFILE_SCOPE("Parallel");
                size_t idx = &range - &iterators[0];

                CommandQueue cq;
                for (auto it = range.first; it != range.second; ++it)
                {
                    func(idx, cq, *it);
                }
                queues[idx] = std::move(cq);
            });

            {
                EVA_ECS_PROFILE_SCOPE("Execure CQ");
                for (auto& cq : queues)
                {
                    cq.Execute(engine);
                }
            }
        }

        class Iterator
        {
            Index m_Index;
            const ChunkInfo* m_CI;
            const std::vector<ChunkInfo>& m_Chunks;

          public:
            const Index& Pos() { return m_Index; }

            void UpdateCI()
            {
                m_CI = nullptr;
                for (const ChunkInfo& info : m_Chunks)
                {
                    if (info.end > m_Index)
                    {
                        m_CI = &info;
                        break;
                    }
                }
            }

            using value_type        = std::tuple<optional_ref_transform_t<T>...>;
            using pointer           = value_type*;
            using reference         = value_type&;
            using difference_type   = Index;
            using iterator_category = std::bidirectional_iterator_tag;

            Iterator(Index index, const std::vector<ChunkInfo>& chunks) : m_Index(index), m_Chunks(chunks) { UpdateCI(); }

            inline bool operator==(const Iterator& other) const { return m_Index == other.m_Index; }
            inline bool operator!=(const Iterator& other) const { return m_Index != other.m_Index; }
            inline bool operator<(const Iterator& other) const { return m_Index < other.m_Index; }
            inline bool operator>(const Iterator& other) const { return m_Index > other.m_Index; }
            inline bool operator<=(const Iterator& other) const { return m_Index <= other.m_Index; }
            inline bool operator>=(const Iterator& other) const { return m_Index >= other.m_Index; }

            inline Iterator& operator++()
            {
                m_Index++;
                if (m_Index >= m_CI->end)
                {
                    m_CI++;
                }

                return *this;
            }

            inline const Iterator operator++(int)
            {
                Iterator temp(*this);
                operator++();
                return temp;
            }

            inline Iterator& operator--()
            {
                m_Index--;
                if (m_Index < m_CI->begin)
                {
                    m_CI--;
                }
                return *this;
            }

            inline Iterator operator--(int)
            {
                Iterator temp(*this);
                operator--();
                return temp;
            }

            inline Iterator& operator+=(difference_type n)
            {
                m_Index += n;
                UpdateCI();
                return *this;
            }

            inline Iterator& operator-=(difference_type n)
            {
                m_Index -= n;
                UpdateCI();
                return *this;
            }

            inline Iterator operator+(difference_type n) const { return Iterator(m_Index + n, m_Chunks); }
            inline Iterator operator-(difference_type n) const { return Iterator(m_Index - n, m_Chunks); }

            inline difference_type operator-(const Iterator& other) const { return m_Index - other.m_Index; }

            inline EntityIterator::value_type operator*() const
            {
                const Index index_in_chunk = m_Index - m_CI->begin;
                return value_type(m_CI->c->GetComponent<T>(std::get<index_transform_t<T>>(m_CI->comp_indices).index, index_in_chunk)...);
            }
        };
    };
} // namespace EVA::ECS
