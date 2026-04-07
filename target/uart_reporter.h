#ifndef UART_REPORTER_H
#define UART_REPORTER_H

#include <stdint.h>

void reporter_start(const char *suite, uint32_t count);
void reporter_pass(const char *name, uint32_t elapsed_us);
void reporter_fail(const char *name, const char *file,
                   int line,  const char *msg);
void reporter_end(uint32_t passed, uint32_t failed);

#endif /* UART_REPORTER_H */
