#include "uart_reporter.h"
#include <stdint.h>
#include <stdio.h>

/* ── board function (defined in boards/stm32f4/board.c) ─── */

void board_uart_print(const char *s);

/* ── internal buffer ───────────────────────────────────── */

static char s_buf[192];

/* ── reporter implementation ───────────────────────────── */

void reporter_start(const char *suite, uint32_t count)
{
    board_uart_print("\r\n");
    board_uart_print("=== embtest starting ===\r\n");
    (void)suite;
    (void)count;
}

void reporter_pass(const char *name, uint32_t elapsed_us)
{
    snprintf(s_buf, sizeof(s_buf),
             "  PASS  %-40s (%lu us)\r\n",
             name, (unsigned long)elapsed_us);
    board_uart_print(s_buf);
}

void reporter_fail(const char *name, const char *file,
                   int line, const char *msg)
{
    snprintf(s_buf, sizeof(s_buf),
             "  FAIL  %s\r\n"
             "        %s:%d\r\n"
             "        %s\r\n",
             name, file, line, msg);
    board_uart_print(s_buf);
}

void reporter_end(uint32_t passed, uint32_t failed)
{
    snprintf(s_buf, sizeof(s_buf),
             "=== done: %lu passed  %lu failed ===\r\n",
             (unsigned long)passed, (unsigned long)failed);
    board_uart_print(s_buf);
}
