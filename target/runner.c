#include "embtest.h"
#include "uart_reporter.h"
#include "watchdog.h" 
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* ── persistent memory helpers ─────────────────────── */

#define MAGIC_VALUE    0xDEAD1234U
#define PERSIST_MAGIC  ((volatile uint32_t *)0x2001FF04U)
#define PERSIST_PASSED ((volatile uint32_t *)0x2001FF08U)
#define PERSIST_FAILED ((volatile uint32_t *)0x2001FF0CU)
#define PERSIST_INDEX  ((volatile uint32_t *)0x2001FF10U)
#define PERSIST_TOTAL  ((volatile uint32_t *)0x2001FF14U)

/* ── storage for the test registry (declared extern in embtest.h) ── */

embtest_entry_t embtest_registry[EMBTEST_MAX_TESTS];
uint32_t        embtest_count          = 0;
volatile bool   embtest_current_failed = false;

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

/* forward declaration */
void watchdog_disarm(void);

/* ── assert handler ─────────────────────────────────────────────── */

void embtest_assert_fail(const char *file, int line, const char *msg)
{
    embtest_current_failed = true;
    watchdog_disarm();        /* stop timer immediately on failure */
    reporter_fail(s_current_name, file, line, msg);
}

/* ── main run loop ──────────────────────────────────────────────── */

void embtest_run_all(void)
{
    uint32_t start_index = 0;

    /* check if we are resuming after a watchdog reset  */
    if (*PERSIST_MAGIC == MAGIC_VALUE) {
        /* resuming — restore counters                  */
        s_passed     = *PERSIST_PASSED;
        s_failed     = *PERSIST_FAILED;
        start_index  = *PERSIST_INDEX;

        /* clear magic so next normal boot starts fresh */
        *PERSIST_MAGIC = 0;
    } else {
        /* fresh boot — start from beginning            */
        s_passed    = 0;
        s_failed    = 0;
        start_index = 0;
        reporter_start("suite", embtest_count);
    }

    for (uint32_t i = start_index; i < embtest_count; i++) {

        s_current_name         = embtest_registry[i].name;
        embtest_current_failed = false;

        /* save state before running — in case of reset */
        *PERSIST_MAGIC   = MAGIC_VALUE;
        *PERSIST_PASSED  = s_passed;
        *PERSIST_FAILED  = s_failed + 1;  /* assume fail */
        *PERSIST_INDEX   = i + 1;         /* next test   */
        *PERSIST_TOTAL   = embtest_count;

        /* arm watchdog                                 */
        watchdog_arm(s_current_name, 5000);

        /* run the test                                 */
        uint32_t t0 = board_get_tick_us();
        embtest_registry[i].fn();
        uint32_t elapsed = board_get_tick_us() - t0;

        /* disarm immediately                           */
        watchdog_disarm();

        /* clear the assume-fail we saved above         */
        *PERSIST_MAGIC = 0;

        if (watchdog_timed_out()) {
            reporter_timeout(s_current_name, 5000);
            s_failed++;
            continue;
        }

        if (!embtest_current_failed) {
            s_passed++;
            reporter_pass(s_current_name, elapsed);
        } else {
            s_failed++;
        }
    }

    reporter_end(s_passed, s_failed);
}
