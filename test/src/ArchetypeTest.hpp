#pragma once

#include "test.hpp"

namespace EVA::ECS
{
    TEST(Archetype, CreateGetDestroy)
    {
        ComponentList cl({ Position::GetType(), StructComponentA::GetType() });
        size_t chunkSize = 4 * (sizeof(Entity) + sizeof(Position) + sizeof(StructComponentA));
        Archetype a(cl, chunkSize);

        EXPECT_EQ(a.EntityCount(), 0);
        EXPECT_EQ(a.ChunkCount(), 1);
        EXPECT_EQ(a.ActiveChunkIndex(), 0);

        for (size_t i = 0; i < 4; i++)
        {
            Entity e(10 + i);
            auto [ch, ci] = a.CreateEntity(e);
            EXPECT_EQ(ch, 0);
            EXPECT_EQ(ci, i);
        }
        for (size_t i = 0; i < 4; i++)
        {
            Entity e(20 + i);
            auto [ch, ci] = a.CreateEntity(e);
            EXPECT_EQ(ch, 1);
            EXPECT_EQ(ci, i);
        }

        EXPECT_EQ(a.EntityCount(), 8);
        EXPECT_EQ(a.ChunkCount(), 2);
        EXPECT_EQ(a.ActiveChunkIndex(), 1);

        EXPECT_EQ(a.GetEntity(0, 2).id, 12);
        EXPECT_EQ(a.GetEntity(0, 3).id, 13);
        EXPECT_EQ(a.GetEntity(1, 0).id, 20);
        EXPECT_EQ(a.GetEntity(1, 3).id, 23);

        a.DestroyEntity(0, 2);
        EXPECT_EQ(a.GetEntity(0, 2).id, 23);
        EXPECT_EQ(a.EntityCount(), 7);

        for (size_t i = 0; i < 6; i++)
            a.DestroyEntity(0, 0);

        EXPECT_EQ(a.EntityCount(), 1);
        EXPECT_EQ(a.ChunkCount(), 2);
        EXPECT_EQ(a.ActiveChunkIndex(), 0);

        EXPECT_EQ(a.GetEntity(0, 0).id, 11);

        for (size_t i = 0; i < 16; i++)
            a.CreateEntity(Entity(i));

        EXPECT_EQ(a.EntityCount(), 17);
        EXPECT_EQ(a.ChunkCount(), 5);
        EXPECT_EQ(a.ActiveChunkIndex(), 4);

        EXPECT_EQ(a.GetEntity(0, 0).id, 11);
        EXPECT_EQ(a.GetEntity(3, 2).id, 13);
    }

    TEST(Archetype, GetComponentChunkIndex)
    {
        ComponentList cl({ Position::GetType(), StructComponentA::GetType() });
        size_t chunkSize = 4 * (sizeof(Entity) + sizeof(Position) + sizeof(StructComponentA));
        Archetype a(cl, chunkSize);

        for (size_t i = 0; i < 20; i++)
            a.CreateEntity(Entity(i));

        // Chunk index & index in chunk
        auto pos1_3_0 = a.GetComponent(Position::GetType(), 1, 3);
        auto pos1_3_1 = a.GetComponent(1, 1, 3);
        auto pos1_3_2 = reinterpret_cast<Byte*>(&a.GetComponent<Position>(1, 3));

        EXPECT_EQ(pos1_3_0, pos1_3_1);
        EXPECT_EQ(pos1_3_0, pos1_3_2);

        a.GetComponent<Position>(1, 3).x = 10;

        EXPECT_EQ(reinterpret_cast<Position*>(pos1_3_0)->x, 10);
        EXPECT_EQ(memcmp(pos1_3_0, pos1_3_1, sizeof(Position)), 0);
        EXPECT_EQ(memcmp(pos1_3_0, pos1_3_2, sizeof(Position)), 0);
    }

    TEST(Archetype, GetComponentArchetypeIndex)
    {
        ComponentList cl({ Position::GetType(), StructComponentA::GetType() });
        size_t chunkSize = 4 * (sizeof(Entity) + sizeof(Position) + sizeof(StructComponentA));
        Archetype a(cl, chunkSize);

        for (size_t i = 0; i < 20; i++)
            a.CreateEntity(Entity(i));

        // Index in archetype
        auto pos14_0 = a.GetComponent(Position::GetType(), 14);
        auto pos14_1 = a.GetComponent(1, 14);
        auto pos14_2 = reinterpret_cast<Byte*>(&a.GetComponent<Position>(14));

        EXPECT_EQ(pos14_0, pos14_1);
        EXPECT_EQ(pos14_0, pos14_2);

        a.GetComponent<Position>(14).x = 10;

        EXPECT_EQ(reinterpret_cast<Position*>(pos14_0)->x, 10);
        EXPECT_EQ(memcmp(pos14_0, pos14_1, sizeof(Position)), 0);
        EXPECT_EQ(memcmp(pos14_0, pos14_2, sizeof(Position)), 0);

        // Compare to chunk index & index in chunk
        auto pos2_3 = a.GetComponent(1, 2, 3);
        auto pos11  = a.GetComponent(1, 11);
        EXPECT_EQ(memcmp(pos2_3, pos11, sizeof(Position)), 0);
    }

    TEST(Archetype, Iterator)
    {
        ComponentList cl({ Position::GetType(), StructComponentA::GetType() });
        size_t chunkSize = 4 * (sizeof(Entity) + sizeof(Position) + sizeof(StructComponentA));
        Archetype a(cl, chunkSize);

        for (size_t i = 0; i < 20; i++)
            a.CreateEntity(Entity(i));

        EXPECT_EQ(std::distance(a.begin<Entity>(), a.end<Entity>()), 20);

        int v = 0;
        for (auto it = a.begin<Position>(); it != a.end<Position>(); ++it)
            it->x = 2 * v++;

        v = 0;
        for (auto it = a.begin<StructComponentA>(); it != a.end<StructComponentA>(); ++it)
            it->y = 3 * v++;

        Index i;
        for (i = 0; i < 20; i++)
            EXPECT_EQ(a.GetComponent<Position>(i).x, 2 * i);

        for (i = 0; i < 20; i++)
            EXPECT_EQ(a.GetComponent<StructComponentA>(i).y, 3 * i);
    }
} // namespace EVA::ECS
