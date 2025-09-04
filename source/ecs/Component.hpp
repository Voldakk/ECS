#pragma once

#include "Core.hpp"
#include "OptionalRef.hpp"

#include <algorithm>
#include <cstring>
#include <map>
#include <memory>
#include <optional>
#include <ostream>
#include <set>
#include <type_traits>
#include <vector>

#define EVA_ECS_REGISTER_COMPONENT(TYPE)                                                                                                   \
  private:                                                                                                                                 \
    inline static EVA::ECS::ComponentType s_Type;                                                                                          \
                                                                                                                                           \
    friend class StaticConstructor;                                                                                                        \
    struct StaticConstructor                                                                                                               \
    {                                                                                                                                      \
        StaticConstructor() { TYPE::s_Type = EVA::ECS::ComponentMap::Add<TYPE>(#TYPE); }                                                   \
    };                                                                                                                                     \
    inline static StaticConstructor cons;                                                                                                  \
                                                                                                                                           \
  public:                                                                                                                                  \
    static EVA::ECS::ComponentType GetType() { return s_Type; }
// #define EVA_ECS_REGISTER_COMPONENT(TYPE)

namespace EVA::ECS
{
    struct ComponentType
    {
        using ValueType = size_t;
        ComponentType() = default;
        explicit ComponentType(ValueType value) : m_Value(value) {}
        inline ValueType Get() const { return m_Value; }
        explicit operator ValueType() const { return m_Value; }

      private:
        ValueType m_Value{ 0 };
    };

    inline std::ostream& operator<<(std::ostream& os, const ComponentType& ct) { return os << ct.Get(); }

    inline bool operator==(const ComponentType& lhs, const ComponentType& rhs) { return lhs.Get() == rhs.Get(); }
    inline bool operator!=(const ComponentType& lhs, const ComponentType& rhs) { return !operator==(lhs, rhs); }
    inline bool operator<(const ComponentType& lhs, const ComponentType& rhs) { return lhs.Get() < rhs.Get(); }
    inline bool operator>(const ComponentType& lhs, const ComponentType& rhs) { return operator<(rhs, lhs); }
    inline bool operator<=(const ComponentType& lhs, const ComponentType& rhs) { return !operator>(lhs, rhs); }
    inline bool operator>=(const ComponentType& lhs, const ComponentType& rhs) { return !operator<(lhs, rhs); }

    class ComponentMap
    {
      public:
        struct ComponentInfo
        {
            const char* name;
            size_t id{ 0 };
            size_t size{ 0 };
            std::unique_ptr<std::vector<Byte>> defaultData = nullptr;
        };

        inline static ComponentType::ValueType s_IdCounter{ 0 };
        inline static std::vector<ComponentInfo> s_Info;

        inline static void CreateComponent(ComponentType type, void* data)
        {
            std::memmove(data, s_Info[type.Get()].defaultData->data(), s_Info[type.Get()].size);
        }

        inline static Byte* DefaultData(ComponentType type) { return s_Info[type.Get()].defaultData->data(); }

        template <typename T> inline static ComponentType Add(const char* name)
        {
            ComponentType type = ComponentType(s_IdCounter++);

            if (type.Get() >= s_Info.size())
            {
                s_Info.resize(type.Get() + 1);
            }
            s_Info[type.Get()].name        = name;
            s_Info[type.Get()].id          = type.Get();
            s_Info[type.Get()].size        = sizeof(T);
            s_Info[type.Get()].defaultData = std::make_unique<std::vector<EVA::ECS::Byte>>(sizeof(T));

            auto* instance = new T();
            std::memmove(s_Info[type.Get()].defaultData->data(), instance, sizeof(T));
            delete instance;

            return type;
        }
    };

    class ComponentList
    {
        std::set<ComponentType> m_Types;

      public:
        ComponentList() = default;
        ComponentList(const std::initializer_list<ComponentType>& types) : m_Types(types) {}
        explicit ComponentList(std::set<ComponentType>& types) : m_Types(std::move(types)) {}

        template <typename... T> static inline ComponentList Create()
        {
            ComponentList cl;
            (cl.Add<T>(), ...);
            return cl;
        }

        ComponentList& Add(ComponentType type)
        {
            ECS_ASSERT(std::find(m_Types.begin(), m_Types.end(), type) == m_Types.end());
            m_Types.insert(type);
            return *this;
        }
        template <typename T> ComponentList& Add() { return Add(T::GetType()); }

        ComponentList& Remove(ComponentType type)
        {
            ECS_ASSERT(std::find(m_Types.begin(), m_Types.end(), type) != m_Types.end());
            m_Types.erase(type);
            return *this;
        }
        template <typename T> ComponentList& Remove() { return Remove(T::GetType()); }

        bool operator==(const ComponentList& other) const { return m_Types == other.m_Types; }
        bool operator!=(const ComponentList& other) const { return !(*this == other); }

        bool Contains(const ComponentList& other) const
        {
            for (const auto& i : other.m_Types)
            {
                if (std::find(m_Types.begin(), m_Types.end(), i) == m_Types.end())
                {
                    return false;
                }
            }
            return true;
        }

        bool Contains(const ComponentType& type) const { return std::find(m_Types.begin(), m_Types.end(), type) != m_Types.end(); }

        bool ContainsAny(const ComponentList& list) const
        {
            for (const auto comp : list)
            {
                if (Contains(comp))
                    return true;
            }
            return false;
        }

        size_t Count() const { return m_Types.size(); }

        const std::set<ComponentType>& GetTypes() const { return m_Types; }

        std::set<ComponentType>::iterator begin() { return m_Types.begin(); }
        std::set<ComponentType>::iterator end() { return m_Types.end(); }
        std::set<ComponentType>::const_iterator begin() const { return m_Types.begin(); }
        std::set<ComponentType>::const_iterator end() const { return m_Types.end(); }
    };

    // is_std_optional_v
    template <typename T> struct is_std_optional : std::false_type
    {
    };

    template <typename T> struct is_std_optional<std::optional<T>> : std::true_type
    {
    };

    template <typename T> inline constexpr bool is_std_optional_v = is_std_optional<T>::value;

    // optional_inner_type_t
    template <typename T> struct optional_inner_type
    {
        using type = T;
    };

    template <typename U> struct optional_inner_type<std::optional<U>>
    {
        using type = U;
    };

    template <typename T> using optional_inner_type_t = typename optional_inner_type<T>::type;

    // Not
    template <typename T> struct Not
    {
        using type = T;
    };

    template <typename T> struct is_not : std::false_type
    {
    };

    template <typename T> struct is_not<Not<T>> : std::true_type
    {
    };

    template <typename T> inline constexpr bool is_not_v = is_not<T>::value;

    template <template <typename> typename Pred, typename... Ts>
    using filter_tuple_t = decltype(std::tuple_cat(std::conditional_t<Pred<Ts>::value, std::tuple<>, std::tuple<Ts>>{}...));

    template <typename... Ts> using remove_nots_t = filter_tuple_t<is_not, Ts...>;

    // Optional ref
    template <typename T> struct optional_ref_transform
    {
        using type = T&;
    };

    template <typename U> struct optional_ref_transform<std::optional<U>>
    {
        using type = OptionalRef<U>;
    };

    template <typename T> using optional_ref_transform_t = typename optional_ref_transform<T>::type;

    class ComponentFilter
    {
        ComponentList compulsory;
        ComponentList optional;
        ComponentList excluded;

      public:
        ComponentFilter() = default;
        ComponentFilter(const std::initializer_list<ComponentType>& types) : compulsory(types) {}
        explicit ComponentFilter(std::set<ComponentType>& types) : compulsory(ComponentList(types)) {}

        template <typename... T> static inline ComponentFilter Create()
        {
            ComponentFilter filter;
            (filter.Add<T>(), ...);
            return filter;
        }

        ComponentFilter& AddCompulsory(ComponentType type)
        {
            compulsory.Add(type);
            return *this;
        }

        ComponentFilter& AddOptional(ComponentType type)
        {
            optional.Add(type);
            return *this;
        }

        ComponentFilter& AddExcluded(ComponentType type)
        {
            excluded.Add(type);
            return *this;
        }

        template <typename T> ComponentFilter& Add()
        {
            if constexpr (is_std_optional_v<T>)
            {
                return AddOptional(T::value_type::GetType());
            }
            else if constexpr (is_not_v<T>)
            {
                return AddExcluded(T::type::GetType());
            }
            else
            {
                return AddCompulsory(T::GetType());
            }
        }

        ComponentFilter& RemoveCompulsory(ComponentType type)
        {
            compulsory.Remove(type);
            return *this;
        }

        ComponentFilter& RemoveOptional(ComponentType type)
        {
            optional.Remove(type);
            return *this;
        }

        ComponentFilter& RemoveExcluded(ComponentType type)
        {
            excluded.Remove(type);
            return *this;
        }

        template <typename T> ComponentFilter& Remove()
        {
            if constexpr (is_std_optional_v<T>)
            {
                return RemoveOptional(T::value_type::GetType());
            }
            else if constexpr (is_not_v<T>)
            {
                return RemoveNot(T::type::GetType());
            }
            else
            {
                return RemoveCompulsory(T::GetType());
            }
        }

        bool operator==(const ComponentFilter& other) const
        {
            return compulsory == other.compulsory && optional == other.optional && excluded == other.excluded;
        }
        bool operator!=(const ComponentFilter& other) const { return !(*this == other); }

        bool Contains(const ComponentList& list) const
        {
            for (const auto& comp : excluded)
            {
                if (list.Contains(comp))
                {
                    return false;
                }
            }
            return compulsory.Contains(list);
        }

        bool Contains(const ComponentType& type) const { return compulsory.Contains(type); }

        size_t Count() const { return compulsory.Count(); }

        const ComponentList& GetCompulsory() const { return compulsory; }
        const ComponentList& GetOptional() const { return optional; }
        const ComponentList& GetExcluded() const { return excluded; }
    };

    struct Entity
    {
        EVA_ECS_REGISTER_COMPONENT(Entity);

        EntityId id = 0;
        Index index = 0;

        Entity() = default;
        explicit Entity(EntityId _id) : id(_id) {}

        bool operator==(const Entity& other) const { return id == other.id; }
        bool operator!=(const Entity& other) const { return !(*this == other); }
    };

    struct EntityLocation
    {
        Index archetype, chunk, position;
        EntityId entityId;

        explicit EntityLocation(Index archetype, Index chunk, Index position, EntityId entityId)
        : archetype(archetype), chunk(chunk), position(position), entityId(entityId)
        {
        }
    };
} // namespace EVA::ECS

namespace std
{
    template <> struct hash<EVA::ECS::ComponentList>
    {
      public:
        std::size_t operator()(const EVA::ECS::ComponentList& list) const
        {
            size_t value = 0;
            for (const auto& type : list.GetTypes())
            {
                std::size_t h = std::hash<size_t>{}(type.Get());
                value ^= h + 0x9e3779b9 + (value << 6) + (value >> 2);
            }
            return value;
        }
    };

    template <> struct hash<EVA::ECS::ComponentFilter>
    {
      public:
        std::size_t operator()(const EVA::ECS::ComponentFilter& filter) const
        {
            std::size_t h1 = std::hash<EVA::ECS::ComponentList>{}(filter.GetCompulsory());
            std::size_t h2 = std::hash<EVA::ECS::ComponentList>{}(filter.GetOptional());
            std::size_t h3 = std::hash<EVA::ECS::ComponentList>{}(filter.GetExcluded());

            std::size_t value = h1;
            value ^= h2 + 0x9e3779b9 + (value << 6) + (value >> 2);
            value ^= h3 + 0x9e3779b9 + (value << 6) + (value >> 2);
            return value;
        }
    };
} // namespace std
