#include "embtest.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* ── storage for the test registry (declared extern in embtest.h) ── */

embtest_entry_t embtest_registry[EMBTEST_MAX_TESTS];
uint32_t        embtest_count          = 0;
bool            embtest_current_failed = false;

/* ── internal state ─────────────────────────────────────────────── */

static const char *s_current_name = NULL;
static uint32_t    s_passed       = 0;
static uint32_t    s_failed       = 0;

/* ── reporter declarations (defined in uart_reporter.c) ─────────── */

void reporter_start(const char *suite, uint32_t count);
void reporter_pass(const char *name, uint32_t elapsed_us);
void reporter_fail(const char *name, const char *file,
                   int line, const char *msg);
void reporter_end(uint32_t passed, uint32_t failed);

/* ── board declaration (defined in boards/stm32f4/board.c) ─────── */

uint32_t board_get_tick_us(void);

/* ── assert handler ─────────────────────────────────────────────── */

void embtest_assert_fail(const char *file, int line, const char *msg)
{
    embtest_current_failed = true;
    reporter_fail(s_current_name, file, line, msg);
}

/* ── main run loop ──────────────────────────────────────────────── */

void embtest_run_all(void)
{
    reporter_start("suite", embtest_count);

    for (uint32_t i = 0; i < embtest_count; i++) {

        s_current_name         = embtest_registry[i].name;
        embtest_current_failed = false;

        uint32_t t0      = board_get_tick_us();
        embtest_registry[i].fn();
        uint32_t elapsed = board_get_tick_us() - t0;

        if (!embtest_current_failed) {
            s_passed++;
            reporter_pass(s_current_name, elapsed);
        } else {
            s_failed++;
        }
    }

    reporter_end(s_passed, s_failed);
}
