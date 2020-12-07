#pragma once

#include "test.hpp"

namespace EVA::ECS
{
    TEST(System, InitUpdate)
    {
        static size_t s_Init   = 0;
        static size_t s_Update = 0;
        class TestSystem : public System
        {
          public:
            virtual void Init() override { s_Init++; }
            virtual void Update() override { s_Update++; }
        };

        Engine engine;
        EXPECT_EQ(s_Init, 0);
        EXPECT_EQ(s_Update, 0);

        engine.AddSystem<TestSystem>();
        engine.AddSystem<TestSystem>();
        EXPECT_EQ(s_Init, 2);
        EXPECT_EQ(s_Update, 0);

        engine.UpdateSystems();
        engine.UpdateSystems();
        engine.UpdateSystems();
        EXPECT_EQ(s_Init, 2);
        EXPECT_EQ(s_Update, 6);
    }

    TEST(System, MovementSystem)
    {
        class MovementSystem : public System
        {
          public:
            virtual void Update() override
            {
                for (auto [ent, pos, vel] : GetEntityIterator<Position, Velocity>())
                {
                    pos.x += vel.x;
                    pos.y += vel.y;
                }
            }
        };

        Engine engine;
        for (size_t i = 0; i < 10; i++)
        {
            engine.CreateEntity(ComponentList::Create<Position, Velocity>());
        }

        for (auto [e, p, v] : EntityIterator<Entity, Position, Velocity>(engine.GetArchetypes<Position, Velocity>()))
        {
            p.x = 0;
            p.y = 0;
            v.x = (int)e.id * 10;
            v.y = (int)e.id * -10;
        }

        engine.AddSystem<MovementSystem>();

        for (size_t i = 0; i < 100; i++)
        {
            engine.UpdateSystems();
        }

        for (auto [e, p, v] : EntityIterator<Entity, Position, Velocity>(engine.GetArchetypes<Position, Velocity>()))
        {
            EXPECT_EQ(p.x, (int)e.id * 10 * 100);
            EXPECT_EQ(p.y, (int)e.id * -10 * 100);
        }
    }
} // namespace EVA::ECS
