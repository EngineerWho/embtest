#ifndef EMBTEST_H
#define EMBTEST_H

#include <stdint.h>
#include <stdbool.h>

/* ── internal types ─────────────────────────────────── */

typedef void (*embtest_fn_t)(void);

typedef struct {
    const char   *name;
    embtest_fn_t  fn;
} embtest_entry_t;

void board_init(void);
/* ── internal state (defined in runner.c) ───────────── */

#define EMBTEST_MAX_TESTS 64

extern embtest_entry_t embtest_registry[EMBTEST_MAX_TESTS];
extern uint32_t        embtest_count;
extern bool            embtest_current_failed;

/* ── internal functions ─────────────────────────────── */

void embtest_run_all(void);
void embtest_assert_fail(const char *file, int line, const char *msg);

/* ── TEST() macro ───────────────────────────────────── */

#define TEST(test_name)                                             \
    static void test_##test_name(void);                             \
    __attribute__((constructor))                                    \
    static void embtest_reg_##test_name(void) {                     \
        if (embtest_count < EMBTEST_MAX_TESTS) {                    \
            embtest_registry[embtest_count].name = #test_name;      \
            embtest_registry[embtest_count].fn   = test_##test_name;\
            embtest_count++;                                        \
        }                                                           \
    }                                                               \
    static void test_##test_name(void)
/* ── ASSERT macros ──────────────────────────────────── */

#define ASSERT_TRUE(expr)                                           \
    do {                                                            \
        if (!(expr)) {                                              \
            embtest_assert_fail(__FILE__, __LINE__, #expr " is false"); \
        }                                                           \
    } while (0)

#define ASSERT_FALSE(expr)                                          \
    do {                                                            \
        if (expr) {                                                 \
            embtest_assert_fail(__FILE__, __LINE__, #expr " is true"); \
        }                                                           \
    } while (0)

#define ASSERT_EQ(a, b)                                             \
    do {                                                            \
        if ((a) != (b)) {                                           \
            embtest_assert_fail(__FILE__, __LINE__, #a " != " #b);  \
        }                                                           \
    } while (0)

#define ASSERT_NEQ(a, b)                                            \
    do {                                                            \
        if ((a) == (b)) {                                           \
            embtest_assert_fail(__FILE__, __LINE__, #a " == " #b);  \
        }                                                           \
    } while (0)

#define ASSERT_NULL(ptr)                                            \
    do {                                                            \
        if ((ptr) != NULL) {                                        \
            embtest_assert_fail(__FILE__, __LINE__, #ptr " is not NULL"); \
        }                                                           \
    } while (0)

#define ASSERT_NOT_NULL(ptr)                                        \
    do {                                                            \
        if ((ptr) == NULL) {                                        \
            embtest_assert_fail(__FILE__, __LINE__, #ptr " is NULL"); \
        }                                                           \
    } while (0)

/* ── EMBTEST_MAIN() ─────────────────────────────────── */

#define EMBTEST_MAIN()                  \
    int main(void) {                    \
        board_init();                   \
        embtest_run_all();              \
        while (1) {}                    \
        return 0;                       \
    }

#endif /* EMBTEST_H */
