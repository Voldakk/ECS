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
        auto pos1_3_2 = ToBytes(a.GetComponent<Position>(1, 3));

        EXPECT_EQ(pos1_3_0, pos1_3_1);
        EXPECT_EQ(pos1_3_0, pos1_3_2);

        a.GetComponent<Position>(1, 3).x = 10;

        EXPECT_EQ(FromBytes<Position>(pos1_3_0)->x, 10);
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
        auto pos14_2 = ToBytes(a.GetComponent<Position>(14));

        EXPECT_EQ(pos14_0, pos14_1);
        EXPECT_EQ(pos14_0, pos14_2);

        a.GetComponent<Position>(14).x = 10;

        EXPECT_EQ(FromBytes<Position>(pos14_0)->x, 10);
        EXPECT_EQ(memcmp(pos14_0, pos14_1, sizeof(Position)), 0);
        EXPECT_EQ(memcmp(pos14_0, pos14_2, sizeof(Position)), 0);

        // Compare to chunk index & index in chunk
        auto pos2_3 = a.GetComponent(1, 2, 3);
        auto pos11  = a.GetComponent(1, 11);
        EXPECT_EQ(memcmp(pos2_3, pos11, sizeof(Position)), 0);
    }

    TEST(Archetype, CreateWithData)
    {
        Archetype a(ComponentList::Create<Position, Velocity, StructComponentA>());

        Position p         = Position(123, 456);
        Velocity v         = Velocity(321, 654);
        StructComponentA s = StructComponentA(11, 22, 33, 22, true, false);
        auto data          = CombineBytesById(p, v, s);

        Entity e(111);
        auto [ch, ci] = a.CreateEntity(e, &data[0]);

        EXPECT_EQ(p, a.GetComponent<Position>(0));
        EXPECT_EQ(v, a.GetComponent<Velocity>(0));
        EXPECT_EQ(s, a.GetComponent<StructComponentA>(0));
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

    TEST(Archetype, AddComponent)
    {
        ComponentList cl1({ Position::GetType(), StructComponentA::GetType() });
        size_t chunkSize1 = 5 * (sizeof(Entity) + sizeof(Position) + sizeof(StructComponentA));
        Archetype a1(cl1, chunkSize1);

        ComponentList cl2({ Position::GetType(), Velocity::GetType(), StructComponentA::GetType() });
        size_t chunkSize2 = 5 * (sizeof(Entity) + sizeof(Position) + sizeof(Velocity) + sizeof(StructComponentA));
        Archetype a2(cl2, chunkSize2);

        for (size_t i = 0; i < 25; i++)
        {
            a1.CreateEntity(Entity(i));
            a1.GetComponent<Position>(i).x = (int)i * 10;
        }

        for (size_t i = 0; i < 25; i++)
        {
            size_t id = 20 + 1;
            a2.CreateEntity(Entity(id));
            a2.GetComponent<Position>(i).x = (int)id * 10;
            a2.GetComponent<Velocity>(i).x = (int)id * 100;
        }

        EXPECT_EQ(a1.EntityCount(), 25);
        EXPECT_EQ(a2.EntityCount(), 25);

        for (size_t i = 0; i < a1.EntityCount(); i++)
        {
            Entity e = a1.GetComponent<Entity>(i);
            EXPECT_EQ(a1.GetComponent<Position>(i).x, e.id * 10);
        }

        for (size_t i = 0; i < a2.EntityCount(); i++)
        {
            Entity e = a2.GetComponent<Entity>(i);
            EXPECT_EQ(a2.GetComponent<Position>(i).x, e.id * 10);
            EXPECT_EQ(a2.GetComponent<Velocity>(i).x, e.id * 100);
        }

        Index indices[5] = { 4, 2, 1, 3, 0 };

        for (size_t i = 0; i < 5; i++)
        {
            Entity e = a1.GetComponent<Entity>(i, indices[i]);
            Velocity v{ (int)e.id * 100, 0 };
            a2.AddEntityAddComponent(a1, i, indices[i], Velocity::GetType(), ToBytes(v));
            a1.DestroyEntity(i, indices[i]);
        }

        EXPECT_EQ(a1.EntityCount(), 20);
        EXPECT_EQ(a2.EntityCount(), 30);

        for (size_t i = 0; i < a1.EntityCount(); i++)
        {
            Entity e = a1.GetComponent<Entity>(i);
            EXPECT_EQ(a1.GetComponent<Position>(i).x, e.id * 10);
        }

        for (size_t i = 0; i < a2.EntityCount(); i++)
        {
            Entity e = a2.GetComponent<Entity>(i);
            EXPECT_EQ(a2.GetComponent<Position>(i).x, e.id * 10);
            EXPECT_EQ(a2.GetComponent<Velocity>(i).x, e.id * 100);
        }

        while (a2.EntityCount() > 0)
        {
            a2.DestroyEntity(0, 0);
        }

        EXPECT_EQ(a1.EntityCount(), 20);
        EXPECT_EQ(a2.EntityCount(), 0);

        for (size_t i = 0; i < 10; i++)
        {
            Entity e = a1.GetComponent<Entity>(0, 0);
            Velocity v{ (int)e.id * 100, 0 };
            a2.AddEntityAddComponent(a1, 0, 0, Velocity::GetType(), ToBytes(v));
            a1.DestroyEntity(0, 0);
        }

        EXPECT_EQ(a1.EntityCount(), 10);
        EXPECT_EQ(a2.EntityCount(), 10);

        for (size_t i = 0; i < a1.EntityCount(); i++)
        {
            Entity e = a1.GetComponent<Entity>(i);
            EXPECT_EQ(a1.GetComponent<Position>(i).x, e.id * 10);
        }

        for (size_t i = 0; i < a2.EntityCount(); i++)
        {
            Entity e = a2.GetComponent<Entity>(i);
            EXPECT_EQ(a2.GetComponent<Position>(i).x, e.id * 10);
            EXPECT_EQ(a2.GetComponent<Velocity>(i).x, e.id * 100);
        }
    }

    TEST(Archetype, RemoveComponent)
    {
        ComponentList cl1({ Position::GetType(), StructComponentA::GetType() });
        size_t chunkSize1 = 5 * (sizeof(Entity) + sizeof(Position) + sizeof(StructComponentA));
        Archetype a1(cl1, chunkSize1);

        ComponentList cl2({ Position::GetType(), Velocity::GetType(), StructComponentA::GetType() });
        size_t chunkSize2 = 5 * (sizeof(Entity) + sizeof(Position) + sizeof(Velocity) + sizeof(StructComponentA));
        Archetype a2(cl2, chunkSize2);

        for (size_t i = 0; i < 25; i++)
        {
            a1.CreateEntity(Entity(i));
            a1.GetComponent<Position>(i).x = (int)i * 10;
        }

        for (size_t i = 0; i < 25; i++)
        {
            size_t id = 20 + 1;
            a2.CreateEntity(Entity(id));
            a2.GetComponent<Position>(i).x = (int)id * 10;
            a2.GetComponent<Velocity>(i).x = (int)id * 100;
        }

        EXPECT_EQ(a1.EntityCount(), 25);
        EXPECT_EQ(a2.EntityCount(), 25);

        for (size_t i = 0; i < a1.EntityCount(); i++)
        {
            Entity e = a1.GetComponent<Entity>(i);
            EXPECT_EQ(a1.GetComponent<Position>(i).x, e.id * 10);
        }

        for (size_t i = 0; i < a2.EntityCount(); i++)
        {
            Entity e = a2.GetComponent<Entity>(i);
            EXPECT_EQ(a2.GetComponent<Position>(i).x, e.id * 10);
            EXPECT_EQ(a2.GetComponent<Velocity>(i).x, e.id * 100);
        }

        Index indices[5] = { 4, 2, 1, 3, 0 };

        for (size_t i = 0; i < 5; i++)
        {
            a1.AddEntityRemoveComponent(a1, i, indices[i], Velocity::GetType());
            a2.DestroyEntity(i, indices[i]);
        }

        EXPECT_EQ(a1.EntityCount(), 30);
        EXPECT_EQ(a2.EntityCount(), 20);

        for (size_t i = 0; i < a1.EntityCount(); i++)
        {
            Entity e = a1.GetComponent<Entity>(i);
            EXPECT_EQ(a1.GetComponent<Position>(i).x, e.id * 10);
        }

        for (size_t i = 0; i < a2.EntityCount(); i++)
        {
            Entity e = a2.GetComponent<Entity>(i);
            EXPECT_EQ(a2.GetComponent<Position>(i).x, e.id * 10);
            EXPECT_EQ(a2.GetComponent<Velocity>(i).x, e.id * 100);
        }

        while (a1.EntityCount() > 0)
        {
            a1.DestroyEntity(0, 0);
        }

        EXPECT_EQ(a1.EntityCount(), 0);
        EXPECT_EQ(a2.EntityCount(), 20);

        for (size_t i = 0; i < 10; i++)
        {
            Entity e = a2.GetComponent<Entity>(0, 0);
            a1.AddEntityRemoveComponent(a2, 0, 0, Velocity::GetType());
            a2.DestroyEntity(0, 0);
        }

        EXPECT_EQ(a1.EntityCount(), 10);
        EXPECT_EQ(a2.EntityCount(), 10);

        for (size_t i = 0; i < a1.EntityCount(); i++)
        {
            Entity e = a1.GetComponent<Entity>(i);
            EXPECT_EQ(a1.GetComponent<Position>(i).x, e.id * 10);
        }

        for (size_t i = 0; i < a2.EntityCount(); i++)
        {
            Entity e = a2.GetComponent<Entity>(i);
            EXPECT_EQ(a2.GetComponent<Position>(i).x, e.id * 10);
            EXPECT_EQ(a2.GetComponent<Velocity>(i).x, e.id * 100);
        }
    }

    TEST(Archetype, EntityIterator)
    {
        ComponentList cl1 = ComponentList::Create<Position, StructComponentA>();
        size_t chunkSize1 = 4 * (sizeof(Entity) + sizeof(Position) + sizeof(StructComponentA));
        Archetype a1(cl1, chunkSize1);

        ComponentList cl2 = ComponentList::Create<Position, Velocity>();
        size_t chunkSize2 = 4 * (sizeof(Entity) + sizeof(Position) + sizeof(Velocity));
        Archetype a2(cl2, chunkSize2);

        for (size_t i = 0; i < 20; i++)
        {
            a1.CreateEntity(Entity(i));
            a2.CreateEntity(Entity(20 + i));
        }

        EXPECT_EQ(a1.EntityCount(), 20);
        EXPECT_EQ(a2.EntityCount(), 20);

        EntityIterator<Entity, Position> it({ &a1, &a2 });

        EXPECT_FALSE(it.Empty());
        EXPECT_EQ(it.Count(), 40);

        for (auto [entity, position] : it)
        {
            position.x = (int)entity.id * 10;
        }

        for (Index i = 0; i < 20; i++)
        {
            EXPECT_EQ(a1.GetComponent<Position>(i).x, a1.GetComponent<Entity>(i).id * 10);
            EXPECT_EQ(a2.GetComponent<Position>(i).x, a2.GetComponent<Entity>(i).id * 10);
        }
    }

    TEST(Archetype, EntityIteratorOptional)
    {
        ComponentList cl1 = ComponentList::Create<Position, StructComponentA>();
        size_t chunkSize1 = 4 * (sizeof(Entity) + sizeof(Position) + sizeof(StructComponentA));
        Archetype a1(cl1, chunkSize1);

        ComponentList cl2 = ComponentList::Create<Position, Velocity>();
        size_t chunkSize2 = 4 * (sizeof(Entity) + sizeof(Position) + sizeof(Velocity));
        Archetype a2(cl2, chunkSize2);

        for (size_t i = 0; i < 20; i++)
        {
            a1.CreateEntity(Entity(i));
            a2.CreateEntity(Entity(20 + i));
        }

        EXPECT_EQ(a1.EntityCount(), 20);
        EXPECT_EQ(a2.EntityCount(), 20);

        {
            EntityIterator<Entity, Position> it({ &a1, &a2 });

            EXPECT_FALSE(it.Empty());
            EXPECT_EQ(it.Count(), 40);

            for (auto [entity, position] : it)
            {
                position.x = (int)entity.id * 10;
            }
        }

        {
            EntityIterator<Entity, Velocity> it({ &a2 });

            EXPECT_FALSE(it.Empty());
            EXPECT_EQ(it.Count(), 20);

            for (auto [entity, velocity] : it)
            {
                velocity.x = 100;
                velocity.y = 0;
            }
        }

        {
            EntityIterator<Entity, Position, std::optional<Velocity>> it({ &a1, &a2 });

            EXPECT_FALSE(it.Empty());
            EXPECT_EQ(it.Count(), 40);

            for (auto [entity, position, velocity] : it)
            {
                if (velocity.has_value())
                {
                    position.x    = velocity.value().x;
                    (*velocity).y = (int)entity.id;
                }
            }

            for (Index i = 0; i < 20; i++)
            {
                EXPECT_EQ(a1.GetComponent<Position>(i).x, a1.GetComponent<Entity>(i).id * 10);
                EXPECT_EQ(a2.GetComponent<Position>(i).x, 100);
                EXPECT_EQ(a2.GetComponent<Velocity>(i).y, a2.GetComponent<Entity>(i).id);
            }
        }
    }
} // namespace EVA::ECS
