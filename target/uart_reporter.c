#include "uart_reporter.h"
#include <stdint.h>
#include <stdio.h>

/* ── board function ─────────────────────────────────── */

void board_uart_print(const char *s);

/* ── internal buffer ────────────────────────────────── */

//static char s_buf[256];

/* ── simple itoa helpers (no printf overhead) ───────── */

static void uart_put_str(const char *s)
{
    board_uart_print(s);
}

static void uart_put_uint(uint32_t n)
{
    char buf[12];
    int i = 10;
    buf[11] = '\0';
    if (n == 0) {
        board_uart_print("0");
        return;
    }
    while (n > 0 && i >= 0) {
        buf[i--] = '0' + (n % 10);
        n /= 10;
    }
    board_uart_print(&buf[i + 1]);
}

/* ── reporter implementation ────────────────────────── */

void reporter_start(const char *suite, uint32_t count)
{
    uart_put_str("EMBTEST:START:");
    uart_put_str(suite);
    uart_put_str(":");
    uart_put_uint(count);
    uart_put_str("\r\n");
}

void reporter_pass(const char *name, uint32_t elapsed_us)
{
    uart_put_str("EMBTEST:PASS:");
    uart_put_str(name);
    uart_put_str(":");
    uart_put_uint(elapsed_us);
    uart_put_str("\r\n");
}

void reporter_fail(const char *name, const char *file,
                   int line, const char *msg)
{
    uart_put_str("EMBTEST:FAIL:");
    uart_put_str(name);
    uart_put_str(":0:");
    uart_put_str(file);
    uart_put_str(":");
    uart_put_uint((uint32_t)line);
    uart_put_str(":");
    uart_put_str(msg);
    uart_put_str("\r\n");
}

void reporter_end(uint32_t passed, uint32_t failed)
{
    uart_put_str("EMBTEST:END:");
    uart_put_uint(passed);
    uart_put_str(":");
    uart_put_uint(failed);
    uart_put_str("\r\n");
}
void reporter_timeout(const char *name, uint32_t ms)
{
    uart_put_str("EMBTEST:TIMEOUT:");
    uart_put_str(name);
    uart_put_str(":");
    uart_put_uint(ms);
    uart_put_str("\r\n");
}

void reporter_fault(const char *name, const char *fault_type)
{
    uart_put_str("EMBTEST:FAULT:");
    uart_put_str(name);
    uart_put_str(":");
    uart_put_str(fault_type);
    uart_put_str("\r\n");
}
