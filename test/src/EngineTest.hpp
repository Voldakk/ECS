#pragma once

#include "test.hpp"

namespace EVA::ECS
{
    TEST(Engine, CreateEntity)
    {
        Engine engine;

        EXPECT_EQ(engine.EntityCount(), 0);
        EXPECT_EQ(engine.ArchetypeCount(), 0);

        auto cl1      = ComponentList().Add<Position>();
        size_t count0 = 100;
        size_t count1 = 200;

        for (size_t i = 0; i < count0; i++)
        {
            auto e = engine.CreateEntity();
            EXPECT_EQ(e.id, i);
        }

        EXPECT_EQ(engine.EntityCount(), count0);
        EXPECT_EQ(engine.ArchetypeCount(), 1);

        for (size_t i = 0; i < count1; i++)
        {
            auto e = engine.CreateEntity(cl1);
            EXPECT_EQ(e.id, count0 + i);
        }

        EXPECT_EQ(engine.EntityCount(), count0 + count1);
        EXPECT_EQ(engine.ArchetypeCount(), 2);
    }

    TEST(Engine, GetEntity)
    {
        Engine engine;

        auto cl0      = ComponentList();
        auto cl1      = ComponentList().Add<Position>();
        size_t count0 = 100;
        size_t count1 = 200;

        for (size_t i = 0; i < count0; i++)
            engine.CreateEntity(cl0);

        for (size_t i = 0; i < count1; i++)
            engine.CreateEntity(cl1);

        auto index0 = engine.GetArchetypeIndex(cl0);
        auto index1 = engine.GetArchetypeIndex(cl1);
        EXPECT_TRUE(index0.has_value());
        EXPECT_TRUE(index1.has_value());
        EXPECT_NE(index0.value(), index1.value());

        auto& a0 = engine.GetArchetype(index0.value());
        auto& a1 = engine.GetArchetype(index1.value());
        EXPECT_NE(&a0, &a1);

        EXPECT_EQ(a0.EntityCount(), count0);
        EXPECT_EQ(a1.EntityCount(), count1);

        EntityIterator<Entity> it({ &a0, &a1 });
        EXPECT_EQ(it.Count(), count0 + count1);

        EntityId id = 0;
        for (auto [entity] : it)
        {
            if (id != 0)
            {
                EXPECT_EQ(entity.id, id + 1);
            }
            id = entity.id;
        }
    }

    TEST(Engine, DeleteEntity)
    {
        Engine engine;

        auto cl0 = ComponentList();

        std::vector<Entity> entities;
        for (size_t i = 0; i < 10; i++)
            entities.push_back(engine.CreateEntity(cl0));

        EntityId ids[7] = { 0, 7, 2, 3, 8, 5, 6 };
        engine.DeleteEntity(entities[9]);
        engine.DeleteEntity(entities[4]);
        engine.DeleteEntity(entities[1]);

        EntityIterator<Entity> it({ &engine.GetArchetype(0) });
        EXPECT_EQ(it.Count(), 7);

        int i = 0;
        for (auto [entity] : it)
        {
            EXPECT_EQ(entity.id, ids[i++]);
        }
    }

    TEST(Engine, GetArchetypes)
    {
        Engine engine;

        ComponentList cl[3] = { ComponentList().Add<Position>(), ComponentList().Add<Velocity>(), ComponentList().Add<Position>().Add<Velocity>() };
        size_t count[3]  = { 200, 300, 400 };
        auto countPos    = count[0] + count[2];
        auto countVel    = count[1] + count[2];
        auto countPosVel = count[2];
        auto countTotal  = count[0] + count[1] + count[2];
        std::vector<Entity> entities;
        for (size_t i = 0; i < 3; i++)
        {
            for (size_t n = 0; n < count[i]; n++)
                entities.push_back(engine.CreateEntity(cl[i]));
        }

        EXPECT_EQ(engine.EntityCount(), countTotal);
        EXPECT_EQ(engine.ArchetypeCount(), 3);

        {
            EntityIterator<Entity> it(engine.GetArchetypes(ComponentList().Add<Position>(), false));
            EXPECT_EQ(it.Count(), countPos);
            EXPECT_FALSE(it.Empty());
            EXPECT_EQ(it.ArchetypeCount(), 2);
        }
        {
            EntityIterator<Entity> it(engine.GetArchetypes(ComponentList().Add<Velocity>(), false));
            EXPECT_EQ(it.Count(), countVel);
            EXPECT_FALSE(it.Empty());
            EXPECT_EQ(it.ArchetypeCount(), 2);
        }
        {
            EntityIterator<Entity> it(engine.GetArchetypes(ComponentList().Add<Position>().Add<Velocity>(), false));
            EXPECT_EQ(it.Count(), countPosVel);
            EXPECT_FALSE(it.Empty());
            EXPECT_EQ(it.ArchetypeCount(), 1);
        }
    }
} // namespace EVA::ECS
