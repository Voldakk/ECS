#pragma once

#include "Component.hpp"
#include "Core.hpp"
#include "EntityIterator.hpp"

namespace EVA::ECS
{
    class Engine;

    class System
    {
        friend Engine;

      public:
        virtual inline void Init() {}
        virtual inline void Update() = 0;

      protected:
        Engine& GetEngine();
        template <typename... T> inline EntityIterator<Entity, T...> GetEntityIterator()
        {
            return EntityIterator<Entity, T...>(GetArchetypes(ComponentList::Create<T...>()));
        }

      private:
        ArchetypeContainer GetArchetypes(const ComponentList& components);
        Engine* m_Engine = nullptr;
    };
} // namespace EVA::ECS
