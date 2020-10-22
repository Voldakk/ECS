#include "ecs/example.hpp"

#pragma warning(push, 0)
#include <gtest/gtest.h>
#pragma warning(pop)

TEST(MathAddTest, CheckValues)
{
    ASSERT_EQ(Example::Math::Add(1, 2), 3);
    EXPECT_TRUE(true);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
