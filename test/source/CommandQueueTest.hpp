#pragma once

#include "test.hpp"

namespace EVA::ECS
{
    TEST(CommandQueue, CreateEntity)
    {
        Engine engine;
        CommandQueue cq;

        auto cl1 = ComponentList::Create<Position, Velocity>();
        auto cl2 = ComponentList::Create<Position, Velocity, IntComp>();

        for (size_t i = 0; i < 5; i++)
        {
            cq.CreateEntity(cl1);
        }

        for (size_t i = 0; i < 5; i++)
        {
            cq.CreateEntity(cl2);
        }

        EXPECT_EQ(engine.EntityCount(), 0);
        EXPECT_EQ(engine.ArchetypeCount(), 0);

        cq.Execute(engine);

        EXPECT_EQ(engine.EntityCount(), 10);
        EXPECT_EQ(engine.ArchetypeCount(), 2);

        EXPECT_EQ(engine.GetArchetype(0).GetComponents(), cl1);
        EXPECT_EQ(engine.GetArchetype(1).GetComponents(), cl2);
    }

    TEST(CommandQueue, CreateEntityFromComponents)
    {
        Engine engine;
        CommandQueue cq;

        auto cl1 = ComponentList::Create<Position, Velocity>();
        auto cl2 = ComponentList::Create<Position, IntComp>();

        for (size_t i = 0; i < 5; i++)
        {
            Position p(i, i * 2);
            Velocity v(i * 3, i * 4);
            cq.CreateEntityFromComponents(p, v);
        }

        for (size_t i = 5; i < 10; i++)
        {
            Position p(i, i * 2);
            IntComp c(i * 5);
            cq.CreateEntityFromComponents(p, c);
        }

        EXPECT_EQ(engine.EntityCount(), 0);
        EXPECT_EQ(engine.ArchetypeCount(), 0);

        cq.Execute(engine);

        EXPECT_EQ(engine.EntityCount(), 10);
        EXPECT_EQ(engine.ArchetypeCount(), 2);

        EXPECT_EQ(engine.GetArchetype(0).GetComponents(), cl1);
        EXPECT_EQ(engine.GetArchetype(1).GetComponents(), cl2);

        {
            EntityIterator<Entity, Position, Velocity> it(engine.GetArchetypes(cl1, false));
            EXPECT_EQ(it.Count(), 5);
            EXPECT_EQ(it.ArchetypeCount(), 1);
            for (auto [e, p, v] : it)
            {
                EXPECT_EQ(p.x, e.id);
                EXPECT_EQ(p.y, e.id * 2);
                EXPECT_EQ(v.x, e.id * 3);
                EXPECT_EQ(v.y, e.id * 4);
            }
        }
        {
            EntityIterator<Entity, Position, IntComp> it(engine.GetArchetypes(cl2, false));
            EXPECT_EQ(it.Count(), 5);
            EXPECT_EQ(it.ArchetypeCount(), 1);
            for (auto [e, p, i] : it)
            {
                EXPECT_EQ(p.x, e.id);
                EXPECT_EQ(p.y, e.id * 2);
                EXPECT_EQ(i.value, e.id * 5);
            }
        }
    }

    TEST(CommandQueue, DestroyEntity)
    {
        Engine engine;
        CommandQueue cq;

        auto cl = ComponentList::Create<Position, Velocity>();
        std::vector<Entity> entities;
        for (size_t i = 0; i < 10; i++)
        {
            entities.push_back(engine.CreateEntity(cl));
        }

        for (size_t i = 0; i < 5; i++)
        {
            cq.DestroyEntity(entities[i]);
        }

        EXPECT_EQ(engine.EntityCount(), 10);

        cq.Execute(engine);

        EXPECT_EQ(engine.EntityCount(), 5);
    }

    TEST(CommandQueue, BigQueue)
    {
        Engine engine;
        CommandQueue cq;

        auto cl1 = ComponentList::Create<Position, Velocity>();

        for (size_t i = 0; i < 10000; i++)
        {
            Position p(i, i * 2);
            Velocity v(i * 3, i * 4);
            cq.CreateEntityFromComponents(p, v);
        }
        EXPECT_EQ(engine.EntityCount(), 0);
        EXPECT_EQ(engine.ArchetypeCount(), 0);

        cq.Execute(engine);

        EXPECT_EQ(engine.EntityCount(), 10000);
        EXPECT_EQ(engine.ArchetypeCount(), 1);

        EXPECT_EQ(engine.GetArchetype(0).GetComponents(), cl1);

        {
            EntityIterator<Entity, Position, Velocity> it(engine.GetArchetypes(cl1, false));
            EXPECT_EQ(it.Count(), 10000);
            EXPECT_EQ(it.ArchetypeCount(), 1);
            for (auto [e, p, v] : it)
            {
                EXPECT_EQ(p.x, e.id);
                EXPECT_EQ(p.y, e.id * 2);
                EXPECT_EQ(v.x, e.id * 3);
                EXPECT_EQ(v.y, e.id * 4);
            }
        }
    }
} // namespace EVA::ECS
