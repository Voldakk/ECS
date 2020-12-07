#pragma once

#include "Core.hpp"

#include <algorithm>
#include <cstring>
#include <map>
#include <memory>
#include <set>
#include <vector>

#define EVA_ECS_REGISTER_COMPONENT(TYPE)                                                                                                   \
  private:                                                                                                                                 \
    inline static EVA::ECS::ComponentType s_Type;                                                                                          \
                                                                                                                                           \
    friend class StaticConstructor;                                                                                                        \
    struct StaticConstructor                                                                                                               \
    {                                                                                                                                      \
        StaticConstructor() { TYPE::s_Type = EVA::ECS::ComponentMap::Add<TYPE>(); }                                                        \
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

        template <typename T> inline static ComponentType Add()
        {
            ComponentType type = ComponentType(s_IdCounter++);

            if (type.Get() >= s_Info.size())
                s_Info.resize(type.Get() + 1);

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

        size_t size() const { return m_Types.size(); }

        std::set<ComponentType>::iterator begin() { return m_Types.begin(); }
        std::set<ComponentType>::iterator end() { return m_Types.end(); }
        std::set<ComponentType>::const_iterator begin() const { return m_Types.begin(); }
        std::set<ComponentType>::const_iterator end() const { return m_Types.end(); }

        struct Hash
        {
          public:
            std::size_t operator()(const ComponentList& list) const
            {
                size_t value = 0;
                for (const auto& type : list)
                {
                    value = value * 3 + std::hash<ComponentType::ValueType>()(type.Get());
                }
                return value;
            }
        };
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
