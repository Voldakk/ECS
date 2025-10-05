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
        virtual ~System() = default;
        virtual inline void Init() {}
        virtual inline void Update() = 0;
        virtual inline void OnEntityCreated(Entity e) {}
        virtual inline void OnEntityDestroyed(Entity e) {}

      protected:
        Engine& GetEngine();

        template <typename Tuple, typename F, typename... Extra> decltype(auto) unpack_tuple_types(F&& f, Extra&&... extra)
        {
            return []<typename... Ts>(std::tuple<Ts...>*, F&& fn, Extra&&... xs)
            { return std::forward<F>(fn).template operator()<Ts...>(std::forward<Extra>(xs)...); }(
            (Tuple*)nullptr, std::forward<F>(f), std::forward<Extra>(extra)...);
        }

        struct Builder
        {
            template <typename... Ts> auto operator()(const std::vector<Archetype*>& a) const { return EntityIterator<Entity, Ts...>(a); }
        };

        template <typename... T> auto GetEntityIterator()
        {
            const auto filter     = ComponentFilter::Create<T...>();
            const auto archetypes = GetArchetypes(filter);

            using Cleaned = remove_nots_t<T...>;

            return unpack_tuple_types<Cleaned>(Builder{}, archetypes);
        }

      private:
        std::vector<Archetype*> GetArchetypes(const ComponentFilter& filter);

        Engine* m_Engine = nullptr;
    };
} // namespace EVA::ECS
