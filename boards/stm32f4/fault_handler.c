#include <stdint.h>
#include "stm32f4xx.h"

/* from watchdog.c */
extern const char *watchdog_current_test(void);

/* from uart_reporter.c */
void reporter_fault(const char *name,
                    const char *fault_type);

/* ── C handler ──────────────────────────────────────── */
/* stack_frame points to saved registers:               */
/* [0]=R0 [1]=R1 [2]=R2 [3]=R3                         */
/* [4]=R12 [5]=LR [6]=PC [7]=xPSR                      */

void embtest_hardfault_c(uint32_t *stack_frame)
{
    /* extract the PC that caused the fault             */
    uint32_t fault_pc = stack_frame[6];
    (void)fault_pc;   /* available for future reporting */

    const char *name = watchdog_current_test();

    /* report fault over UART before resetting          */
    reporter_fault(
        name ? name : "unknown",
        "HardFault"
    );

    /* small delay to ensure UART transmits             */
    volatile uint32_t d = 100000;
    while (d--) {}

    /* reset — persistent RAM from runner.c handles     */
    /* resume automatically on next boot                */
    NVIC_SystemReset();
}

/* ── Assembly trampoline ─────────────────────────────── */

__attribute__((naked))
void HardFault_Handler(void)
{
    __asm volatile (
        "tst    lr, #4              \n"
        "ite    eq                  \n"
        "mrseq  r0, msp             \n"
        "mrsne  r0, psp             \n"
        "b      embtest_hardfault_c \n"
    );
}
