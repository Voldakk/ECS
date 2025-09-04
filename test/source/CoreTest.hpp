#pragma once

#include "test.hpp"

namespace EVA::ECS
{
    TEST(Core, SizeOf)
    {
        EXPECT_EQ(SizeOf<int>, sizeof(int));
        EXPECT_EQ(SizeOf<long>, sizeof(long));

        auto s = SizeOf<int, long>;
        EXPECT_EQ(s, sizeof(int) + sizeof(long));
    }

    TEST(Core, PostAdd)
    {
        int a = 10;
        int b = PostAdd(a, 5);
        EXPECT_EQ(a, 15);
        EXPECT_EQ(b, 10);
    }

    TEST(Core, CopyInto)
    {
        Position p = Position(123, 456);
        Velocity v = Velocity(321, 654);

        std::vector<Byte> data(sizeof(Position) + sizeof(Velocity));
        std::memmove(&data[0], &p, sizeof(Position));
        std::memmove(&data[sizeof(Position)], &v, sizeof(Velocity));


        std::vector<Byte> data2(sizeof(Position) + sizeof(Velocity));
        Index index = 0;
        CopyInto(data2, index, p, v);

        EXPECT_EQ(memcmp(&data[0], &data2[0], sizeof(Position) + sizeof(Velocity)), 0);
    }

    TEST(Core, CombineBytes)
    {
        Position p = Position(123, 456);
        Velocity v = Velocity(321, 654);

        std::vector<Byte> data(sizeof(Position) + sizeof(Velocity));
        std::memmove(&data[0], &p, sizeof(Position));
        std::memmove(&data[sizeof(Position)], &v, sizeof(Velocity));

        auto data2 = CombineBytes(p, v);

        EXPECT_EQ(memcmp(&data[0], &data2[0], sizeof(Position) + sizeof(Velocity)), 0);
    }

    TEST(Core, CombineBytesById)
    {
        Position p = Position(123, 456);
        Velocity v = Velocity(321, 654);

        std::vector<Byte> data(sizeof(Position) + sizeof(Velocity));
        std::memmove(&data[0], &p, sizeof(Position));
        std::memmove(&data[sizeof(Position)], &v, sizeof(Velocity));

        const auto data2 = CombineBytesById(v, p);

        EXPECT_EQ(memcmp(&data[0], &data2[0], sizeof(Position) + sizeof(Velocity)), 0);
    }
} // namespace EVA::ECS