#include "stm32f4xx.h"

/* Called by startup_stm32f411xx.s before main()        */
/* For embtest we keep it minimal — no PLL setup,       */
/* just run at default 16MHz HSI clock.                 */

void SystemInit(void)
{
    /* Disable all interrupts */
    __disable_irq();

    /* Reset RCC to default values */
    RCC->CR    |=  0x00000001U;   /* Enable HSI */
    RCC->CFGR   =  0x00000000U;   /* Reset CFGR */
    RCC->CR    &= ~0xFEF6FFFFU;   /* Disable PLL, HSE */
    RCC->PLLCFGR = 0x24003010U;   /* Reset PLLCFGR */
    RCC->CR    &= ~0x01000000U;   /* Disable PLLSAI */
    RCC->CIR    =  0x00000000U;   /* Disable interrupts */

    /* Re-enable interrupts */
    __enable_irq();
}
