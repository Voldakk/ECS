#include "ArchetypeChunkTest.hpp"
#include "ComponentTest.hpp"

/*
TEST(TestSuiteName, TestName) {
  ... test body ...
}

                    Fatal assertion 	            Nonfatal assertion 	            Verifies
Basic Assertions
                    ASSERT_TRUE(condition); 	    EXPECT_TRUE(condition); 	    condition is true
                    ASSERT_FALSE(condition); 	    EXPECT_FALSE(condition); 	    condition is false
Binary Comparison
                    ASSERT_EQ(val1, val2); 	        EXPECT_EQ(val1, val2); 	        val1 == val2
                    ASSERT_NE(val1, val2); 	        EXPECT_NE(val1, val2); 	        val1 != val2
                    ASSERT_LT(val1, val2); 	        EXPECT_LT(val1, val2); 	        val1 < val2
                    ASSERT_LE(val1, val2); 	        EXPECT_LE(val1, val2); 	        val1 <= val2
                    ASSERT_GT(val1, val2); 	        EXPECT_GT(val1, val2); 	        val1 > val2
                    ASSERT_GE(val1, val2); 	        EXPECT_GE(val1, val2); 	        val1 >= val2
C-String Comparison
                    ASSERT_STREQ(str1,str2); 	    EXPECT_STREQ(str1,str2); 	    the two C strings have the same content
                    ASSERT_STRNE(str1,str2); 	    EXPECT_STRNE(str1,str2); 	    the two C strings have different contents
                    ASSERT_STRCASEEQ(str1,str2); 	EXPECT_STRCASEEQ(str1,str2); 	the two C strings have the same content, ignoring case
                    ASSERT_STRCASENE(str1,str2); 	EXPECT_STRCASENE(str1,str2); 	the two C strings have different contents, ignoring case
*/

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
