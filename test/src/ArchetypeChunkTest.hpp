#pragma once

#include "test.hpp"

namespace EVA::ECS
{
    TEST(ArchetypeInfo, Sizes)
    {
        ComponentList cl({ Position::GetType(), StructComponentA::GetType() });
        ArchetypeInfo ai(cl);

        EXPECT_EQ(ai.entitySize, sizeof(Entity) + sizeof(Position) + sizeof(StructComponentA));
        EXPECT_EQ(ai.entitiesPerChunk, ai.chunkSize / ai.entitySize);
        EXPECT_EQ(ai.componentInfo.size(), 3);
    }

    TEST(ArchetypeInfo, ComponentInfo)
    {
        ComponentList cl({ Position::GetType(), StructComponentA::GetType() });
        ArchetypeInfo ai(cl);

        EXPECT_EQ(ai.componentInfo[0].start, 0);
        EXPECT_EQ(ai.componentInfo[0].type, Entity::GetType());
        EXPECT_EQ(ai.componentInfo[0].size, sizeof(Entity));

        EXPECT_EQ(ai.componentInfo[1].start, ai.entitiesPerChunk * sizeof(Entity));
        EXPECT_EQ(ai.componentInfo[1].type, Position::GetType());
        EXPECT_EQ(ai.componentInfo[1].size, sizeof(Position));

        EXPECT_EQ(ai.componentInfo[2].start, ai.entitiesPerChunk * (sizeof(Entity) + sizeof(Position)));
        EXPECT_EQ(ai.componentInfo[2].type, StructComponentA::GetType());
        EXPECT_EQ(ai.componentInfo[2].size, sizeof(StructComponentA));
    }

    TEST(ArchetypeInfo, GetComponentIndex)
    {
        ComponentList cl({ Position::GetType(), StructComponentA::GetType() });
        ArchetypeInfo ai(cl);

        EXPECT_EQ(ai.GetComponentIndex(Entity::GetType()), 0);
        EXPECT_EQ(ai.GetComponentIndex(Position::GetType()), 1);
        EXPECT_EQ(ai.GetComponentIndex(StructComponentA::GetType()), 2);
    }

    TEST(ArchetypeChunk, CreateEntity)
    {
        ComponentList cl({ Position::GetType(), StructComponentA::GetType() });
        size_t chunkSize = 4 * (sizeof(Entity) + sizeof(Position) + sizeof(StructComponentA));
        ArchetypeInfo ai(cl, chunkSize);
        ArchetypeChunk ac(ai);

        Entity e1(11);
        Entity e2(22);
        Entity e3(33);
        Entity e4(44);

        EXPECT_EQ(ac.Count(), 0);
        EXPECT_TRUE(ac.Empty());
        EXPECT_FALSE(ac.Full());

        ac.CreateEntity(e1);

        EXPECT_EQ(ac.Count(), 1);
        EXPECT_FALSE(ac.Empty());
        EXPECT_FALSE(ac.Full());

        ac.CreateEntity(e2);

        EXPECT_EQ(ac.Count(), 2);

        ac.CreateEntity(e3);

        EXPECT_EQ(ac.Count(), 3);

        ac.CreateEntity(e4);

        EXPECT_EQ(ac.Count(), 4);
        EXPECT_FALSE(ac.Empty());
        EXPECT_TRUE(ac.Full());

        Entity& get1 = ac.GetEntity(0);
        Entity& get3 = ac.GetEntity(2);

        EXPECT_EQ(get1.id, e1.id);
        EXPECT_EQ(get3.id, e3.id);
    }

    TEST(ArchetypeChunk, CopyEntity)
    {
        ComponentList cl({ Position::GetType(), StructComponentA::GetType() });
        ArchetypeInfo ai(cl);

        ArchetypeChunk ac1(ai);
        ArchetypeChunk ac2(ai);

        for (size_t i = 0; i < 20; i++)
        {
            ac1.CreateEntity(Entity(i));
            ac2.CreateEntity(Entity(100 + i));
        }

        ac1.CopyEntity(5, ac2, 15);

        Entity& get0 = ac1.GetEntity(5);
        Entity& get1 = ac2.GetEntity(15);

        EXPECT_EQ(get0.id, 115);
        EXPECT_EQ(get1.id, 115);
    }

    TEST(ArchetypeChunk, GetComponent)
    {
        ComponentList cl({ Position::GetType(), StructComponentA::GetType() });
        ArchetypeInfo ai(cl);
        ArchetypeChunk ac(ai);

        for (size_t i = 0; i < 20; i++)
            ac.CreateEntity(Entity(i));

        auto pos14_0 = ac.GetComponent(Position::GetType(), 14);
        auto pos14_1 = ac.GetComponent(1, 14);
        auto pos14_2 = ToBytes(ac.GetComponent<Position>(14));

        EXPECT_EQ(pos14_0, pos14_1);
        EXPECT_EQ(pos14_0, pos14_2);

        ac.GetComponent<Position>(14).x = 10;

        EXPECT_EQ(FromBytes<Position>(pos14_0)->x, 10);
        EXPECT_EQ(memcmp(pos14_0, pos14_1, sizeof(Position)), 0);
        EXPECT_EQ(memcmp(pos14_0, pos14_2, sizeof(Position)), 0);
    }

    TEST(ArchetypeChunk, Iterator)
    {
        ComponentList cl({ Position::GetType(), StructComponentA::GetType() });
        ArchetypeInfo ai(cl);
        ArchetypeChunk ac(ai);

        for (size_t i = 0; i < 20; i++)
            ac.CreateEntity(Entity(i));

        EXPECT_EQ(std::distance(ac.begin<Entity>(), ac.end<Entity>()), 20);

        int v = 0;
        for (auto it = ac.begin<Position>(); it != ac.end<Position>(); ++it)
            it->x = 2 * v++;

        v = 0;
        for (auto it = ac.begin<StructComponentA>(); it != ac.end<StructComponentA>(); ++it)
            it->y = 3 * v++;

        Index i;
        for (i = 0; i < 20; i++)
            EXPECT_EQ(ac.GetComponent<Position>(i).x, 2 * i);

        for (i = 0; i < 20; i++)
            EXPECT_EQ(ac.GetComponent<StructComponentA>(i).y, 3 * i);
    }

    TEST(ArchetypeChunk, AddComponent)
    {
        ComponentList cl1({ Position::GetType(), StructComponentA::GetType() });
        ArchetypeInfo ai1(cl1);
        ArchetypeChunk ac1(ai1);

        for (size_t i = 0; i < 10; i++)
        {
            ac1.CreateEntity(Entity(i));
            ac1.GetComponent<Position>(i).x = i * 10;
        }

        ComponentList cl2({ Position::GetType(), Velocity::GetType(), StructComponentA::GetType() });
        ArchetypeInfo ai2(cl2);
        ArchetypeChunk ac2(ai2);

        for (size_t i = 0; i < 10; i++)
        {
            size_t id = 10 + 1;
            ac2.CreateEntity(Entity(id));
            ac2.GetComponent<Position>(i).x = id * 10;
            ac2.GetComponent<Velocity>(i).x = id * 100;
        }

        EXPECT_EQ(ac1.Count(), 10);
        EXPECT_EQ(ac2.Count(), 10);

        for (size_t i = 2; i <= 5; i++)
        {
            Velocity v{ (int)i * 100, 0 };
            ac2.AddEntityAddComponent(Velocity::GetType(), ac1, i, ToBytes(v));
        }

        EXPECT_EQ(ac1.Count(), 10);
        EXPECT_EQ(ac2.Count(), 14);

        for (size_t i = 0; i < ac1.Count(); i++)
        {
            Entity e = ac1.GetComponent<Entity>(i);
            EXPECT_EQ(ac1.GetComponent<Position>(i).x, e.id * 10);
        }

        for (size_t i = 0; i < ac2.Count(); i++)
        {
            Entity e = ac2.GetComponent<Entity>(i);
            EXPECT_EQ(ac2.GetComponent<Position>(i).x, e.id * 10);
            EXPECT_EQ(ac2.GetComponent<Velocity>(i).x, e.id * 100);
        }
    }

    TEST(ArchetypeChunk, RemoveComponent)
    {
        ComponentList cl1({ Position::GetType(), StructComponentA::GetType() });
        ArchetypeInfo ai1(cl1);
        ArchetypeChunk ac1(ai1);

        for (size_t i = 0; i < 10; i++)
        {
            ac1.CreateEntity(Entity(i));
            ac1.GetComponent<Position>(i).x = i * 10;
        }

        ComponentList cl2({ Position::GetType(), Velocity::GetType(), StructComponentA::GetType() });
        ArchetypeInfo ai2(cl2);
        ArchetypeChunk ac2(ai2);

        for (size_t i = 0; i < 10; i++)
        {
            size_t id = 10 + 1;
            ac2.CreateEntity(Entity(id));
            ac2.GetComponent<Position>(i).x = id * 10;
            ac2.GetComponent<Velocity>(i).x = id * 100;
        }

        EXPECT_EQ(ac1.Count(), 10);
        EXPECT_EQ(ac2.Count(), 10);

        for (size_t i = 2; i <= 5; i++)
        {
            ac1.AddEntityRemoveComponent(Velocity::GetType(), ac2, i);
        }

        EXPECT_EQ(ac1.Count(), 14);
        EXPECT_EQ(ac2.Count(), 10);

        for (size_t i = 0; i < ac1.Count(); i++)
        {
            Entity e = ac1.GetComponent<Entity>(i);
            EXPECT_EQ(ac1.GetComponent<Position>(i).x, e.id * 10);
        }

        for (size_t i = 0; i < ac2.Count(); i++)
        {
            Entity e = ac2.GetComponent<Entity>(i);
            EXPECT_EQ(ac2.GetComponent<Position>(i).x, e.id * 10);
            EXPECT_EQ(ac2.GetComponent<Velocity>(i).x, e.id * 100);
        }
    }
} // namespace EVA::ECS
