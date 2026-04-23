#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

/* ── called once at startup ─────────────────────────── */
void board_init(void);

/* ── UART output ────────────────────────────────────── */
void board_uart_putchar(char c);
void board_uart_print(const char *s);

/* ── microsecond timer ──────────────────────────────── */
uint32_t board_get_tick_us(void);

#endif /* BOARD_H */
