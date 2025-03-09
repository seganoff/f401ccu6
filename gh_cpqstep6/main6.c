#include "hal.h"

static volatile uint32_t s_ticks;
void SysTick_Handler(void) {s_ticks++;}

uint32_t SystemCoreClock = SYS_FREQUENCY;

void SystemInit(void) {  // Called automatically by startup code
  SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2));  // Enable FPU
  FLASH->ACR |= FLASH_LATENCY | BIT(8) | BIT(9);      // Flash latency, prefetch
/*
Since the main-PLL configuration parameters cannot be changed once PLL is enabled, it is
recommended to configure PLL before enabling it 
(selection of the HSI or HSE oscillator as PLL clock source, !! done where?
and configuration of division factors M, N, P, and Q).
where csson ?
*/
  RCC->PLLCFGR &= ~((BIT(17) - 1));  // Clear PLL multipliers
  RCC->PLLCFGR |= (((PLL_P - 2) / 2) & 3) << 16;      // Set PLL_P
  RCC->PLLCFGR |= PLL_M | (PLL_N << 6);               // Set PLL_M and PLL_N
//PLL_Q what?
  RCC->CR |= BIT(24);                                 // Enable PLL
  while ((RCC->CR & BIT(25)) == 0) spin(1);           // Wait until done
//pllrdy 25
  RCC->CFGR = (APB1_PRE << 10) | (APB2_PRE << 13);    // Set prescalers
  RCC->CFGR |= 2;                                     // Set clock source to PLL
  while ((RCC->CFGR & 12) == 0) spin(1);              // Wait until done

  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;    // Enable SYSCFG
// where to set csson 19, 
  SysTick_Config(SystemCoreClock / 1000);  // Sys tick every 1ms
}

int main(void) {
uint16_t led = PIN('B', 7);                 // Blue LED
gpio_output(led);       // Set blue LED to output mode
  //uart_init(UART_DEBUG, 115200);              // Initialise UART
volatile uint32_t timer = 0, period = 500;  // Declare timers
for (;;) {
if (timer_expired(&timer, period, s_ticks)) {
static bool on;       // This block is executed
gpio_write(led, on);  // Every `period` milliseconds
on = !on;             // Toggle LED state
     // printf("LED: %d, tick: %lu\r\n", on, s_ticks);  // Write message
}//if
    // Here we could perform other activities!
}//for loop
return 0;}
