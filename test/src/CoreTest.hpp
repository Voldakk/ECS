#pragma once

#include "test.hpp"

namespace EVA::ECS
{
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
} // namespace EVA::ECS