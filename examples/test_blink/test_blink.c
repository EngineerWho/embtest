#include "embtest.h"

TEST(always_passes)
{
    ASSERT_TRUE(1);
}

TEST(basic_math)
{
    ASSERT_EQ(2 + 2, 4);
}

TEST(always_fails)
{
    ASSERT_EQ(1, 2);
}

EMBTEST_MAIN();
