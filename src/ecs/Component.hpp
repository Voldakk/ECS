#pragma once

#include "Core.hpp"

#include <algorithm>
#include <map>
#include <memory>
#include <set>
#include <string.h>
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
    static EVA::ECS::ComponentType GetType()                                                                                               \
    {                                                                                                                                      \
        return s_Type;                                                                                                                     \
    }                                                                                                                                      \
    static size_t GetSize()                                                                                                                \
    {                                                                                                                                      \
        return s_Size;                                                                                                                     \
    }
// #define REGISTER_COMPONENT(NAME)


namespace EVA::ECS
{
    typedef size_t ComponentType;
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

        inline static ComponentType s_IdCounter = 0;
        inline static std::vector<ComponentInfo> s_Info;

        template <typename T> static std::shared_ptr<Component> CreateT()
        {
            return std::make_shared<T>();
        }

        inline static void CreateComponent(ComponentType type, void* data)
        {
            memcpy(data, s_Info[type].defaultData.get(), s_Info[type].size);
        }

        inline static Component* DefaultData(ComponentType type)
        {
            return s_Info[type].defaultData.get();
        }
    };

    class ComponentList
    {
      public:
        std::set<ComponentType> types;

        ComponentList() = default;

        ComponentList(const std::vector<ComponentType>& _types)
        {
            for (auto& t : _types)
            {
                types.insert(t);
            }
        }

        ComponentList& Add(ComponentType type)
        {
            ECS_ASSERT(std::find(types.begin(), types.end(), type) == types.end());
            types.insert(type);
            return *this;
        }
        template <typename T> ComponentList& Add()
        {
            static_assert(std::is_base_of<Component, T>::value, "T must be derived from component");
            return Add(T::GetType());
        }

        ComponentList& Remove(ComponentType type)
        {
            ECS_ASSERT(std::find(types.begin(), types.end(), type) != types.end());
            types.erase(type);
            return *this;
        }
        template <typename T> ComponentList& Remove()
        {
            static_assert(std::is_base_of<Component, T>::value, "T must be derived from component");
            return Remove(T::GetType());
        }

        bool operator==(const ComponentList& other) const
        {
            return types == other.types;
        }

        bool Contains(const ComponentList& other) const
        {
            for (auto& i : other.types)
            {
                if (std::find(types.begin(), types.end(), i) == types.end())
                    return false;
            }
            return true;
        }
    };

    struct ComponentListHash
    {
      public:
        std::size_t operator()(const ComponentList& l) const
        {
            size_t value = 0;
            for (auto& i : l.types)
            {
                value = value * 3 + std::hash<ComponentType>()(i);
            }
            return value;
        }
    };

    struct Component
    {
        REGISTER_COMPONENT(Component);
        Component() = default;
    };
} // namespace EVA::ECS
