#include "embtest.h"
#include <stddef.h>

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

TEST(string_not_null)
{
    const char *s = "hello";
    ASSERT_NOT_NULL(s);
}

TEST(causes_timeout)
{
    /* hangs forever — watchdog must catch this         */
    volatile int x = 1;
    while (x) {}
}

TEST(runs_after_timeout)
{
    /* this MUST run after the timeout above            */
    /* if it does — watchdog recovery is proven         */
    ASSERT_TRUE(1);
}

TEST(causes_hardfault)
{
    /* null pointer dereference — HardFault guaranteed  */
    volatile uint32_t *p = (volatile uint32_t *)0x00000000U;
    *p = 0xDEADBEEFU;
}

TEST(runs_after_fault)
{
    /* proves suite continued after HardFault           */
    ASSERT_TRUE(1);
}

EMBTEST_MAIN();
