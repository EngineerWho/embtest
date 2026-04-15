#include "board.h"
#include <stdint.h>
#include <stddef.h>
#include "stm32f4xx.h"


/* ── function decleration ─────────────────────────────── */

void board_uart_putchar(char c);
void board_uart_print(const char *s);
uint32_t board_get_tick_us(void);

/* ── microsecond tick counter ───────────────────────── */

static volatile uint32_t s_tick_us = 0;

/*── Simple millisecond delay using SysTick (1 ms tick)───────  */
void delay_ms(uint32_t ms)
{
    uint32_t start =  board_get_tick_us();
    while ((board_get_tick_us() - start) < (ms * 1000U))
    {
        /* You can add __WFI() here later for lower power */
    }
}
//================================================================================
/* ── debug blink ────────────────────────────────────── */

static void debug_blink(uint32_t times)
{
    /* PA5 = green LED on Nucleo F411RE                 */
    RCC->AHB1ENR  |= RCC_AHB1ENR_GPIOAEN;
    GPIOA->MODER  &= ~(3U << (5 * 2));
    GPIOA->MODER  |=  (1U << (5 * 2));

    for (uint32_t i = 0; i < times; i++) {
        GPIOA->ODR |=  (1U << 5);
        volatile uint32_t d = 400000;
        while (d--) {}
        GPIOA->ODR &= ~(1U << 5);
        d = 400000;
        while (d--) {}
    }

    /* pause between groups so counts are readable      */
    volatile uint32_t p = 1200000;
    while (p--) {}
}

/* ── board_init ─────────────────────────────────────── */

void board_init(void)
{

    /* line: RCC->CR |= RCC_CR_HSION */
    RCC->CR |= RCC_CR_HSION;

   /* line: wait for HSIRDY with timeout */
    volatile uint32_t t = 50000;
    while (!(RCC->CR & RCC_CR_HSIRDY) && t--) {}

    /* line: enable GPIOA clock */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    /* line: enable USART2 clock */
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    /* line: dummy read to flush clock enable */
    volatile uint32_t d = RCC->APB1ENR; (void)d;

    /* line: PA2 MODER clear */
    GPIOA->MODER &= ~(3U << (2 * 2));
    /* line: PA2 MODER set alternate function */
    GPIOA->MODER |= (2U << (2 * 2));
    /* line: PA2 OTYPER push-pull */
    GPIOA->OTYPER &= ~(1U << 2);
   /* line: PA2 OSPEEDR high speed */
    GPIOA->OSPEEDR |= (3U << (2 * 2));
    /* line: PA2 PUPDR no pull */
    GPIOA->PUPDR &= ~(3U << (2 * 2));
    /* line: PA2 AFR clear */
    GPIOA->AFR[0] &= ~(0xFU << (4 * 2));

    /* line: PA2 AFR set AF7 */
    GPIOA->AFR[0] |= (7U << (4 * 2));
    /* line: PA3 MODER clear */
    GPIOA->MODER &= ~(3U << (3 * 2));
    /* line: PA3 MODER set alternate function */
    GPIOA->MODER |= (2U << (3 * 2));
    /* line: PA3 AFR clear */
    GPIOA->AFR[0] &= ~(0xFU << (4 * 3));
    /* line: PA3 AFR set AF7 */
    GPIOA->AFR[0] |= (7U << (4 * 3));
    /* line: USART2 CR1 = 0 (disable before config) */
    USART2->CR1 = 0;
    /* line: USART2 CR2 = 0 */
    USART2->CR2 = 0;
    /* line: USART2 CR3 = 0 */
    USART2->CR3 = 0;
    /* line: USART2 BRR = 0x0683 (9600 baud at 16MHz) */
    USART2->BRR = 0x0683U;
    /* line: USART2 CR1 enable UE + TE + RE */
    USART2->CR1 = USART_CR1_UE
                | USART_CR1_TE
                | USART_CR1_RE;

    /* line: SysTick LOAD */
    SysTick->LOAD = 15U;
    /* line: SysTick VAL reset */
    SysTick->VAL = 0U; 
    /* SysTick — polling mode, no interrupt             */
    SysTick->LOAD = 0xFFFFFFU;   /* max reload value   */
    SysTick->VAL  = 0U;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk
                  | SysTick_CTRL_ENABLE_Msk;
                  /* NOTE: no TICKINT — no interrupt    */
 
    /* line: wait before uart print */
    volatile uint32_t wait = 200000;
    while (wait--) {}

   /* line: board_uart_print boot message */
    board_uart_print("\r\nEMBTEST BOOT\r\n");
    debug_blink(3);
    /* reached end of board_init successfully */
}
//====================================================================================
/* ── board_init ─────────────────────────────── */
/* ── board_uart_putchar ─────────────────────────────── */

void board_uart_putchar(char c)
{
    /* Wait until transmit data register is empty       */
    while (!(USART2->SR & USART_SR_TXE)) {}
    USART2->DR = (uint8_t)c;
}

/* ── board_uart_print ───────────────────────────────── */

void board_uart_print(const char *s)
{
    while (*s) {
        board_uart_putchar(*s++);
    }
}

/* ── board_get_tick_us ──────────────────────────────── */
uint32_t board_get_tick_us(void)
{
    /* SysTick counts DOWN from LOAD to 0               */
    /* We return a simple incrementing value            */
    /* based on the SysTick current value               */
    return (SysTick->LOAD - SysTick->VAL);
}
