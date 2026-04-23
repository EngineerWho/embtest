#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <stdint.h>

void        watchdog_arm(const char *test_name,
                         uint32_t    timeout_ms);
void        watchdog_disarm(void);
int         watchdog_timed_out(void);
void        watchdog_trigger_longjmp(void);
const char *watchdog_current_test(void);

#endif /* WATCHDOG_H */
