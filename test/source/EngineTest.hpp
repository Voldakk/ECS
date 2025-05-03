#pragma once

#include "test.hpp"

namespace EVA::ECS
{
    TEST(Engine, CreateEntity)
    {
        Engine engine;

        EXPECT_EQ(engine.EntityCount(), 0);
        EXPECT_EQ(engine.ArchetypeCount(), 0);

        auto cl1      = ComponentList::Create<Position>();
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
        auto cl1      = ComponentList::Create<Position>();
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

    TEST(Engine, GetComponent)
    {
        Engine engine;

        std::vector<Entity> entities;
        for (size_t i = 0; i < 10; i++)
            entities.push_back(engine.CreateEntity(ComponentList::Create<IntComp>()));

        EntityIterator<Entity, IntComp> it(engine.GetArchetypes<IntComp>());
        for (auto [e, i] : it)
        {
            i.value = (int)e.id + 10000;
        }

        EXPECT_EQ(engine.GetComponent<IntComp>(entities[2]).value, 2 + 10000);

        IntComp value = { 10000 + 5 };
        EXPECT_EQ(memcmp(engine.GetComponent(entities[5], IntComp::GetType()), &value, sizeof(IntComp)), 0);
    }

    TEST(Engine, CreateWithData)
    {
        Engine engine;
        auto cl = ComponentList::Create<Position, Velocity, StructComponentA>();

        Position p         = Position(123, 456);
        Velocity v         = Velocity(321, 654);
        StructComponentA s = StructComponentA(11, 22, 33, 22, true, false);
        auto data          = CombineBytes(p, v, s);

        Entity e1 = engine.CreateEntity(cl, &data[0]);
        Entity e2 = engine.CreateEntityFromComponents(p, v, s);

        EXPECT_EQ(p, engine.GetComponent<Position>(e1));
        EXPECT_EQ(v, engine.GetComponent<Velocity>(e1));
        EXPECT_EQ(s, engine.GetComponent<StructComponentA>(e1));

        EXPECT_EQ(p, engine.GetComponent<Position>(e2));
        EXPECT_EQ(v, engine.GetComponent<Velocity>(e2));
        EXPECT_EQ(s, engine.GetComponent<StructComponentA>(e2));
    }

    TEST(Engine, GetArchetypes)
    {
        Engine engine;

        ComponentList cl[3] = { ComponentList::Create<Position>(), ComponentList::Create<Velocity>(), ComponentList::Create<Position, Velocity>() };
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

        EXPECT_EQ(engine.ArchetypeCount(), 3);
        EXPECT_EQ(engine.EntityCount(), countTotal);

        {
            EntityIterator<Entity> it(engine.GetArchetypes(ComponentList::Create<Position>(), false));
            EXPECT_EQ(it.Count(), countPos);
            EXPECT_FALSE(it.Empty());
            EXPECT_EQ(it.ArchetypeCount(), 2);
        }
        {
            EntityIterator<Entity> it(engine.GetArchetypes(ComponentList::Create<Velocity>(), false));
            EXPECT_EQ(it.Count(), countVel);
            EXPECT_FALSE(it.Empty());
            EXPECT_EQ(it.ArchetypeCount(), 2);
        }
        {
            EntityIterator<Entity> it(engine.GetArchetypes(ComponentList::Create<Position, Velocity>(), false));
            EXPECT_EQ(it.Count(), countPosVel);
            EXPECT_FALSE(it.Empty());
            EXPECT_EQ(it.ArchetypeCount(), 1);
        }
    }
    TEST(Engine, AddComponentSimple)
    {
        Engine engine;
        Entity entity = engine.CreateEntity();
        IntComp iData = { 10 };
        engine.AddComponent(entity, iData);

        {
            EntityIterator<Entity, IntComp> it(engine.GetArchetypes<IntComp>());
            EXPECT_EQ(it.Count(), 1);
            for (auto [e, i] : it)
            {
                EXPECT_EQ(i, iData);
                i.value = 1000;
            }
        }

        Position pData = Position(20, 30);
        engine.AddComponent(entity, pData);

        {
            EntityIterator<Entity, IntComp, Position> it(engine.GetArchetypes<IntComp, Position>());
            EXPECT_EQ(it.Count(), 1);
            for (auto [e, i, p] : it)
            {
                EXPECT_EQ(i.value, 1000);
                EXPECT_EQ(p, pData);
            }
        }
    }

    TEST(Engine, AddComponent)
    {
        Engine engine;

        size_t noneCount   = 100;
        size_t posCount    = 550;
        size_t velCount    = 350;
        size_t entityCount = noneCount + posCount + velCount;
        size_t posvelCount = 200;

        size_t posIndex = noneCount;
        size_t velIndex = posIndex + posCount;

        std::vector<Entity> entities;
        entities.reserve(entityCount);
        auto cl = ComponentList::Create<IntComp>();
        for (size_t i = 0; i < entityCount; i++)
        {
            entities.push_back(engine.CreateEntity(cl));
        }
        {
            EntityIterator<Entity, IntComp> it(engine.GetArchetypes<IntComp>());
            EXPECT_EQ(it.Count(), entityCount);
            EXPECT_EQ(it.ArchetypeCount(), 1);
            for (auto [e, i] : it)
            {
                i.value = (int)e.id + 10000;
            }
        }
        {
            EntityIterator<Entity, IntComp> it(engine.GetArchetypes<IntComp>());
            EXPECT_EQ(it.Count(), entityCount);
            EXPECT_EQ(it.ArchetypeCount(), 1);
            for (auto [e, i] : it)
            {
                ASSERT_EQ(i.value, (int)e.id + 10000);
            }
        }

        for (size_t i = posIndex; i < posIndex + posCount; i++)
        {
            engine.AddComponent<Position>(entities[i]);
        }

        for (size_t i = velIndex; i < velIndex + velCount; i++)
        {
            engine.AddComponent<Velocity>(entities[i]);
        }

        {
            EntityIterator<Entity, IntComp, Position> it(engine.GetArchetypes<IntComp, Position>());
            EXPECT_EQ(it.Count(), posCount);
            EXPECT_EQ(it.ArchetypeCount(), 1);
            for (auto [e, i, p] : it)
            {
                ASSERT_EQ(i.value, (int)e.id + 10000);
                p.x = (int)e.id * 2;
                p.y = i.value * 3;
            }
        }
        {
            EntityIterator<Entity, IntComp, Velocity> it(engine.GetArchetypes<IntComp, Velocity>());
            EXPECT_EQ(it.Count(), velCount);
            EXPECT_EQ(it.ArchetypeCount(), 1);
            for (auto [e, i, v] : it)
            {
                ASSERT_EQ(i.value, (int)e.id + 10000);
                v.x = (int)e.id * 4;
                v.y = i.value * 5;
            }
        }

        Velocity vData(1, 2);
        for (size_t i = posIndex; i < posIndex + posvelCount; i++)
        {
            engine.AddComponent<Velocity>(entities[i], vData);
        }

        Position pData(3, 4);
        for (size_t i = velIndex; i < velIndex + posvelCount; i++)
        {
            engine.AddComponent<Position>(entities[i], pData);
        }

        EXPECT_EQ(engine.EntityCount(), entityCount);
        EXPECT_EQ(engine.ArchetypeCount(), 4);

        {
            EntityIterator<Entity, IntComp> it(engine.GetArchetypes<IntComp>());
            EXPECT_EQ(it.Count(), entityCount);
            EXPECT_EQ(it.ArchetypeCount(), 4);
            int c = 0;
            for (auto [e, i] : it)
            {
                c++;

                ASSERT_EQ(i.value, e.id + 10000);
            }
        }
        {
            EntityIterator<Entity, IntComp, Position> it(engine.GetArchetypes<IntComp, Position>());
            EXPECT_EQ(it.Count(), posCount + posvelCount);
            EXPECT_EQ(it.ArchetypeCount(), 2);
            size_t count = 0;
            for (auto [e, i, p] : it)
            {
                ASSERT_EQ(i.value, e.id + 10000);
                if (p == pData)
                {
                    count++;
                }
                else
                {
                    ASSERT_EQ(p.x, e.id * 2);
                    ASSERT_EQ(p.y, i.value * 3);
                }
            }
            EXPECT_EQ(count, posvelCount);
        }
        {
            EntityIterator<Entity, IntComp, Velocity> it(engine.GetArchetypes<IntComp, Velocity>());
            EXPECT_EQ(it.Count(), velCount + posvelCount);
            EXPECT_EQ(it.ArchetypeCount(), 2);
            size_t count = 0;
            for (auto [e, i, v] : it)
            {
                ASSERT_EQ(i.value, e.id + 10000);
                if (v == vData)
                {
                    count++;
                }
                else
                {
                    ASSERT_EQ(v.x, e.id * 4);
                    ASSERT_EQ(v.y, i.value * 5);
                }
            }
            EXPECT_EQ(count, posvelCount);
        }
        {
            EntityIterator<Entity, IntComp, Position, Velocity> it(engine.GetArchetypes<IntComp, Position, Velocity>());
            EXPECT_EQ(it.Count(), posvelCount * 2);
            EXPECT_EQ(it.ArchetypeCount(), 1);
            size_t count = 0;
            for (auto [e, i, p, v] : it)
            {
                ASSERT_EQ(i.value, e.id + 10000);
                if (p == pData)
                {
                    count++;
                }
                else
                {
                    ASSERT_EQ(p.x, e.id * 2);
                    ASSERT_EQ(p.y, i.value * 3);
                }
                if (v == vData)
                {
                    count++;
                }
                else
                {
                    ASSERT_EQ(v.x, e.id * 4);
                    ASSERT_EQ(v.y, i.value * 5);
                }
            }
            EXPECT_EQ(count, posvelCount * 2);
        }

        size_t addPosCount = noneCount;

        for (size_t i = 0; i < noneCount; i++)
        {
            engine.DeleteEntity(entities[i]);
        }
        for (size_t i = 0; i < addPosCount; i++)
        {
            entities[i] = engine.CreateEntity(cl);
            engine.AddComponent<Position>(entities[i], pData);
        }
        {
            EntityIterator<Entity, IntComp> it(engine.GetArchetypes<IntComp>());
            for (auto [e, i] : it)
            {
                i.value = (int)e.id + 10000;
            }
        }
        {
            EntityIterator<Entity, IntComp, Position> it(engine.GetArchetypes<IntComp, Position>());
            EXPECT_EQ(it.Count(), posCount + posvelCount + noneCount);
            EXPECT_EQ(it.ArchetypeCount(), 2);
            size_t count = 0;
            for (auto [e, i, p] : it)
            {
                ASSERT_EQ(i.value, e.id + 10000);
                if (p == pData)
                {
                    count++;
                }
                else
                {
                    ASSERT_EQ(p.x, e.id * 2);
                    ASSERT_EQ(p.y, i.value * 3);
                }
            }
            EXPECT_EQ(count, posvelCount + noneCount);
        }
    }

    TEST(Engine, RemoveComponentSimple)
    {
        Engine engine;
        Entity entity = engine.CreateEntity(ComponentList::Create<IntComp, Position, Velocity>());

        Position pData = Position(10, 20);
        Velocity vData = Velocity(30, 40);

        {
            EntityIterator<Entity, IntComp, Position, Velocity> it(engine.GetArchetypes<>());
            EXPECT_EQ(it.Count(), 1);
            for (auto [e, i, p, v] : it)
            {
                i.value = 1000;
                p       = pData;
                v       = vData;
            }
        }

        {
            EntityIterator<Entity, IntComp, Position, Velocity> it(engine.GetArchetypes<>());
            EXPECT_EQ(it.Count(), 1);
            for (auto [e, i, p, v] : it)
            {
                EXPECT_EQ(i.value, 1000);
                EXPECT_EQ(p, pData);
                EXPECT_EQ(v, vData);
            }
        }

        engine.RemoveComponent<Velocity>(entity);

        {
            EntityIterator<> it2(engine.GetArchetypes<Velocity>());
            EXPECT_TRUE(it2.Empty());

            EntityIterator<Entity, IntComp, Position> it(engine.GetArchetypes<>());
            EXPECT_EQ(it.Count(), 1);
            for (auto [e, i, p] : it)
            {
                EXPECT_EQ(i.value, 1000);
                EXPECT_EQ(p, pData);
            }
        }

        engine.RemoveComponent<Position>(entity);

        {
            EntityIterator<> it2(engine.GetArchetypes<Position>());
            EXPECT_TRUE(it2.Empty());

            EntityIterator<Entity, IntComp> it(engine.GetArchetypes<>());
            EXPECT_EQ(it.Count(), 1);
            for (auto [e, i] : it)
            {
                EXPECT_EQ(i.value, 1000);
            }
        }

        engine.RemoveComponent<IntComp>(entity);

        {
            EntityIterator<> it2(engine.GetArchetypes<IntComp>());
            EXPECT_TRUE(it2.Empty());

            EntityIterator<Entity> it(engine.GetArchetypes<>());
            EXPECT_EQ(it.Count(), 1);
            for (auto [e] : it)
            {
                EXPECT_EQ(e.id, 0);
            }
        }
    }

    TEST(Engine, RemoveComponent)
    {
        Engine engine;

        size_t noneCount   = 100;
        size_t posCount    = 550;
        size_t velCount    = 350;
        size_t posvelCount = 200;
        size_t entityCount = noneCount + posCount + velCount + posvelCount;

        size_t noneIndex   = 0;
        size_t posIndex    = noneIndex + noneCount;
        size_t velIndex    = posIndex + posCount;
        size_t posvelIndex = velIndex + velCount;

        std::vector<Entity> entities;
        entities.reserve(entityCount);
        auto cl = ComponentList::Create<Position, Velocity>();
        for (size_t i = 0; i < entityCount; i++)
        {
            entities.push_back(engine.CreateEntity(cl));
        }

        {
            EntityIterator<Entity, Position, Velocity> it(engine.GetArchetypes<>());
            EXPECT_EQ(it.Count(), entityCount);
            EXPECT_EQ(it.ArchetypeCount(), 1);
            for (auto [e, p, v] : it)
            {
                p.x = 1 + (int)e.id * 2;
                p.y = 1 + (int)e.id * 3;
                v.x = 1 + (int)e.id * 5;
                v.y = 1 + (int)e.id * 7;
            }
        }
        {
            EntityIterator<Entity, Position, Velocity> it(engine.GetArchetypes<>());
            EXPECT_EQ(it.Count(), entityCount);
            for (auto [e, p, v] : it)
            {
                ASSERT_EQ(p.x, 1 + e.id * 2);
                ASSERT_EQ(p.y, 1 + e.id * 3);
                ASSERT_EQ(v.x, 1 + e.id * 5);
                ASSERT_EQ(v.y, 1 + e.id * 7);
            }
        }
        for (size_t i = noneIndex; i < velIndex; i++)
        {
            engine.RemoveComponent<Velocity>(entities[i]);
        }
        for (size_t i = noneIndex; i < posIndex; i++)
        {
            engine.RemoveComponent<Position>(entities[i]);
        }
        for (size_t i = velIndex; i < posvelIndex; i++)
        {
            engine.RemoveComponent<Position>(entities[i]);
        }
        {
            EntityIterator<Entity, Position, Velocity> it(engine.GetArchetypes<Position, Velocity>());
            EXPECT_EQ(it.Count(), posvelCount);
            for (auto [e, p, v] : it)
            {
                ASSERT_EQ(p.x, 1 + e.id * 2);
                ASSERT_EQ(p.y, 1 + e.id * 3);
                ASSERT_EQ(v.x, 1 + e.id * 5);
                ASSERT_EQ(v.y, 1 + e.id * 7);
            }
        }
        {
            EntityIterator<Entity, Position> it(engine.GetArchetypes<Position>());
            EXPECT_EQ(it.Count(), posCount + posvelCount);
            for (auto [e, p] : it)
            {
                ASSERT_EQ(p.x, 1 + e.id * 2);
                ASSERT_EQ(p.y, 1 + e.id * 3);
            }
        }
        {
            EntityIterator<Entity, Velocity> it(engine.GetArchetypes<Velocity>());
            EXPECT_EQ(it.Count(), velCount + posvelCount);
            for (auto [e, v] : it)
            {
                ASSERT_EQ(v.x, 1 + e.id * 5);
                ASSERT_EQ(v.y, 1 + e.id * 7);
            }
        }
        {
            EntityIterator<Entity> it(engine.GetArchetypes<>());
            EXPECT_EQ(it.Count(), entityCount);
            for (auto [e] : it)
            {
                ASSERT_EQ(e.id, e.index);
            }
        }
    }
} // namespace EVA::ECS
