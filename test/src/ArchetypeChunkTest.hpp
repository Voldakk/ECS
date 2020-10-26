#pragma once

#include "test.hpp"

namespace EVA::ECS
{
    TEST(ArchetypeInfo, Sizes)
    {
        ComponentList cl({ Position::GetType(), StructComponentA::GetType() });
        ArchetypeInfo ai(cl);

        EXPECT_EQ(ai.entitySize, sizeof(Entity) + sizeof(Position) + sizeof(StructComponentA));
        EXPECT_EQ(ai.entitiesPerChunk, chunkSize / ai.entitySize);
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

    TEST(ArchetypeChunk, GetComponentIndex)
    {
        ComponentList cl({ Position::GetType(), StructComponentA::GetType() });
        ArchetypeInfo ai(cl);
        ArchetypeChunk ac(ai);

        EXPECT_EQ(ac.GetComponentIndex(Entity::GetType()), 0);
        EXPECT_EQ(ac.GetComponentIndex(Position::GetType()), 1);
        EXPECT_EQ(ac.GetComponentIndex(StructComponentA::GetType()), 2);
    }

    TEST(ArchetypeChunk, CreateEntity)
    {
        ComponentList cl({ Position::GetType(), StructComponentA::GetType() });
        ArchetypeInfo ai(cl);
        ArchetypeChunk ac(ai);

        Entity e0(11);
        Entity e1(22);

        EXPECT_EQ(ac.Count(), 0);
        ac.CreateEntity(e0);
        EXPECT_EQ(ac.Count(), 1);
        ac.CreateEntity(e1);
        EXPECT_EQ(ac.Count(), 2);

        Entity& get0 = ac.GetEntity(0);
        Entity& get1 = ac.GetEntity(1);

        EXPECT_EQ(get0.id, e0.id);
        EXPECT_EQ(get1.id, e1.id);
    }

    TEST(ArchetypeChunk, CopyEntity)
    {
        ComponentList cl({ Position::GetType(), StructComponentA::GetType() });
        ArchetypeInfo ai(cl);

        ArchetypeChunk ac1(ai);
        ArchetypeChunk ac2(ai);

        for (size_t i = 0; i < 20; i++)
        {
            ac1.CreateEntity(i);
            ac2.CreateEntity(100 + i);
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
        {
            ac.CreateEntity(i);
        }

        auto pos14_0 = ac.GetComponent(Position::GetType(), 14);
        auto pos14_1 = ac.GetComponent(1, 14);
        auto pos14_2 = reinterpret_cast<byte*>(&ac.GetComponent<Position>(14));

        EXPECT_EQ(pos14_0, pos14_1);
        EXPECT_EQ(pos14_0, pos14_2);

        ac.GetComponent<Position>(14).x = 10;

        EXPECT_EQ(reinterpret_cast<Position*>(pos14_0)->x, 10);
        EXPECT_EQ(memcmp(pos14_0, pos14_1, sizeof(Position)), 0);
        EXPECT_EQ(memcmp(pos14_0, pos14_2, sizeof(Position)), 0);
    }
} // namespace EVA::ECS
