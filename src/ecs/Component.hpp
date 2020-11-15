#pragma once

#include "Core.hpp"

#include <algorithm>
#include <cstring>
#include <map>
#include <memory>
#include <set>
#include <vector>

#define BEGIN_STATIC_CONSTRUCTOR(NAME)                                                                                                     \
    friend class __GeneratedConstructor##NAME;                                                                                             \
    struct __GeneratedConstructor##NAME                                                                                                    \
    {                                                                                                                                      \
        __GeneratedConstructor##NAME()
// #define BEGIN_STATIC_CONSTRUCTOR


#define END_STATIC_CONSTRUCTOR(NAME)                                                                                                       \
    }                                                                                                                                      \
    ;                                                                                                                                      \
    inline static __GeneratedConstructor##NAME cons;
// #define END_STATIC_CONSTRUCTOR


#define REGISTER_COMPONENT(NAME)                                                                                                           \
  private:                                                                                                                                 \
    inline static EVA::ECS::ComponentType s_Type;                                                                                          \
    BEGIN_STATIC_CONSTRUCTOR(NAME)                                                                                                         \
    {                                                                                                                                      \
        NAME::s_Type = EVA::ECS::ComponentType(EVA::ECS::ComponentMap::s_IdCounter++);                                                     \
        if (NAME::s_Type.Get() >= EVA::ECS::ComponentMap::s_Info.size())                                                                   \
            EVA::ECS::ComponentMap::s_Info.resize(NAME::s_Type.Get() + 1);                                                                 \
        EVA::ECS::ComponentMap::s_Info[NAME::s_Type.Get()].size        = sizeof(NAME);                                                     \
        EVA::ECS::ComponentMap::s_Info[NAME::s_Type.Get()].defaultData = std::make_unique<std::vector<EVA::ECS::Byte>>(sizeof(NAME));      \
        auto* instance                                                 = new NAME();                                                       \
        std::memmove(EVA::ECS::ComponentMap::s_Info[NAME::s_Type.Get()].defaultData->data(), instance, sizeof(NAME));                      \
        delete instance;                                                                                                                   \
    }                                                                                                                                      \
    END_STATIC_CONSTRUCTOR(NAME)                                                                                                           \
  public:                                                                                                                                  \
    static EVA::ECS::ComponentType GetType() { return s_Type; }                                                                            \
    // #define REGISTER_COMPONENT(NAME)


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
    };

    class ComponentList
    {
        std::set<ComponentType> m_Types;

      public:
        ComponentList() = default;
        ComponentList(const std::initializer_list<ComponentType>& types) : m_Types(types) {}

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
        REGISTER_COMPONENT(Entity);

        size_t id    = 0;
        size_t index = 0;

        Entity() = default;
        explicit Entity(size_t _id) : id(_id) {}

        bool operator==(const Entity& other) const { return id == other.id; }
        bool operator!=(const Entity& other) const { return !(*this == other); }
    };
} // namespace EVA::ECS
