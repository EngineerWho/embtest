#include "watchdog.h"
#include "uart_reporter.h"
#include "stm32f4xx.h"
#include <stddef.h>
#include <stdbool.h>


/* ── persistent state — survives soft reset ─────────── */
/* placed at end of RAM, not touched by startup code    */
/* STM32F411RE has 128KB RAM: 0x20000000 - 0x2001FFFF  */

#define MAGIC_VALUE     0xDEAD1234U
#define PERSIST_ADDR    ((volatile uint32_t *)0x2001FF00U)
#define PERSIST_MAGIC   ((volatile uint32_t *)0x2001FF04U)
#define PERSIST_PASSED  ((volatile uint32_t *)0x2001FF08U)
#define PERSIST_FAILED  ((volatile uint32_t *)0x2001FF0CU)
#define PERSIST_INDEX   ((volatile uint32_t *)0x2001FF10U)
#define PERSIST_TOTAL   ((volatile uint32_t *)0x2001FF14U)

/* from runner.c */
extern volatile bool embtest_current_failed;

/* ── internal state ─────────────────────────────────── */

static const char  *s_test_name  = NULL;
static uint32_t     s_timeout_ms = 0;
static volatile int s_armed      = 0;
static volatile int s_timed_out  = 0;
static volatile int s_faulted    = 0;

/* ── public: current test name ──────────────────────── */

const char *watchdog_current_test(void)
{
    return s_test_name;
}

/* ── TIM2 IRQ — sets flag, does NOT longjmp ─────────── */

void TIM2_IRQHandler(void)
{
    TIM2->SR &= ~TIM_SR_UIF;

    if (s_armed && !embtest_current_failed) {
        s_armed     = 0;
        s_timed_out = 1;
        TIM2->CR1  &= ~TIM_CR1_CEN;
        NVIC_DisableIRQ(TIM2_IRQn);
        /* report timeout over UART                     */
        reporter_timeout(s_test_name, s_timeout_ms);

        /* reset the processor — only safe way to       */
        /* escape a truly infinite loop on bare metal   */
        NVIC_SystemReset();
    } else {
        /* assert already fired — ignore this timeout   */
        s_armed = 0;
        TIM2->CR1 &= ~TIM_CR1_CEN;
        NVIC_DisableIRQ(TIM2_IRQn);
    }
}

/* ── watchdog_arm ────────────────────────────────────── */

void watchdog_arm(const char *test_name,
                  uint32_t    timeout_ms)
{
    s_test_name  = test_name;
    s_timeout_ms = timeout_ms;
    s_timed_out  = 0;
    s_faulted    = 0;

    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    TIM2->CR1  = 0;
    TIM2->SR   = 0;
    TIM2->PSC  = 15999U;
    TIM2->ARR  = timeout_ms;
    TIM2->CNT  = 0;
    TIM2->DIER = TIM_DIER_UIE;
    TIM2->CR1  = TIM_CR1_OPM | TIM_CR1_CEN;

    NVIC_SetPriority(TIM2_IRQn, 1);
    NVIC_EnableIRQ(TIM2_IRQn);

    s_armed = 1;
}

/* ── watchdog_disarm ─────────────────────────────────── */

void watchdog_disarm(void)
{
    s_armed     = 0;
    s_timed_out = 0;          /* clear flag too          */
    TIM2->CR1  &= ~TIM_CR1_CEN;
    TIM2->SR    = 0;          /* clear interrupt flag    */
    NVIC_DisableIRQ(TIM2_IRQn);
    NVIC_ClearPendingIRQ(TIM2_IRQn);  /* flush pending  */
}

/* ── watchdog_timed_out — runner polls this ─────────── */

int watchdog_timed_out(void)
{
    return s_timed_out;
}

/* ── watchdog_trigger_longjmp — called by HardFault ─── */

void watchdog_trigger_longjmp(void)
{
    s_armed   = 0;
    s_faulted = 1;
    TIM2->CR1 &= ~TIM_CR1_CEN;
    NVIC_DisableIRQ(TIM2_IRQn);

    embtest_current_failed = true;
    reporter_fault(s_test_name ? s_test_name : "unknown",
                   "HardFault");

    /* reset the processor cleanly                      */
    /* this is the safest recovery from HardFault       */
    /* on bare-metal Cortex-M                           */
    NVIC_SystemReset();
}
