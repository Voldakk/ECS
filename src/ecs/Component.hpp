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
    inline static size_t s_Size;                                                                                                           \
    BEGIN_STATIC_CONSTRUCTOR(NAME)                                                                                                         \
    {                                                                                                                                      \
        NAME::s_Type = EVA::ECS::ComponentMap::s_IdCounter++;                                                                              \
        NAME::s_Size = sizeof(NAME);                                                                                                       \
        if (NAME::s_Type >= EVA::ECS::ComponentMap::s_Info.size())                                                                         \
            EVA::ECS::ComponentMap::s_Info.resize(NAME::s_Type + 1);                                                                       \
        EVA::ECS::ComponentMap::s_Info[NAME::s_Type].size        = NAME::s_Size;                                                           \
        EVA::ECS::ComponentMap::s_Info[NAME::s_Type].func        = &EVA::ECS::ComponentMap::CreateT<NAME>;                                 \
        EVA::ECS::ComponentMap::s_Info[NAME::s_Type].defaultData = std::make_unique<NAME>();                                               \
    }                                                                                                                                      \
    END_STATIC_CONSTRUCTOR(NAME)                                                                                                           \
  public:                                                                                                                                  \
    static EVA::ECS::ComponentType GetType() { return s_Type; }                                                                            \
    static size_t GetSize() { return s_Size; }
// #define REGISTER_COMPONENT(NAME)


namespace EVA::ECS
{
    struct ComponentType
    {
        typedef size_t ValueType;
        ComponentType() = default;
        ComponentType(ValueType value) : m_Value(value) {}
        operator ValueType() const { return m_Value; }

      private:
        ValueType m_Value = 0;
    };

    struct Component;

    class ComponentMap
    {
      public:
        struct ComponentInfo
        {
            size_t size                            = 0;
            std::shared_ptr<Component> (*func)()   = nullptr;
            std::unique_ptr<Component> defaultData = nullptr;
        };

        inline static ComponentType::ValueType s_IdCounter = 0;
        inline static std::vector<ComponentInfo> s_Info;

        template <typename T> static std::shared_ptr<Component> CreateT() { return std::make_shared<T>(); }

        inline static void CreateComponent(ComponentType type, void* data)
        {
            std::memmove(data, s_Info[type].defaultData.get(), s_Info[type].size);
        }

        inline static Component* DefaultData(ComponentType type) { return s_Info[type].defaultData.get(); }
    };

    class ComponentList
    {
        std::set<ComponentType> m_Types;

      public:
        ComponentList() = default;

        ComponentList(const std::initializer_list<ComponentType>& _types)
        {
            for (auto& t : _types)
            {
                m_Types.insert(t);
            }
        }

        ComponentList& Add(ComponentType type)
        {
            ECS_ASSERT(std::find(m_Types.begin(), m_Types.end(), type) == m_Types.end());
            m_Types.insert(type);
            return *this;
        }
        template <typename T> ComponentList& Add()
        {
            static_assert(std::is_base_of<Component, T>::value, "T must be derived from component");
            return Add(T::GetType());
        }

        ComponentList& Remove(ComponentType type)
        {
            ECS_ASSERT(std::find(m_Types.begin(), m_Types.end(), type) != m_Types.end());
            m_Types.erase(type);
            return *this;
        }
        template <typename T> ComponentList& Remove()
        {
            static_assert(std::is_base_of<Component, T>::value, "T must be derived from component");
            return Remove(T::GetType());
        }

        bool operator==(const ComponentList& other) const { return m_Types == other.m_Types; }

        bool Contains(const ComponentList& other) const
        {
            for (auto& i : other.m_Types)
            {
                if (std::find(m_Types.begin(), m_Types.end(), i) == m_Types.end())
                    return false;
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
                for (auto& type : list)
                {
                    value = value * 3 + std::hash<ComponentType::ValueType>()(type);
                }
                return value;
            }
        };
    };

    struct Component
    {
        REGISTER_COMPONENT(Component);
        Component() = default;
    };

    struct Entity : public Component
    {
        REGISTER_COMPONENT(Entity);

        size_t id    = 0;
        size_t index = 0;

        Entity() = default;
        Entity(size_t _id) : id(_id) {}

        bool operator==(const Entity& other) const { return id == other.id; }
        bool operator!=(const Entity& other) const { return !(*this == other); }
    };
} // namespace EVA::ECS
