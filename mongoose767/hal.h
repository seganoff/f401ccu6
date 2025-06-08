
#pragma once

#define LED1 PIN('B', 0)   // On-board LED pin (green)
#define LED2 PIN('B', 7)   // On-board LED pin (blue)
#define LED3 PIN('B', 14)  // On-board LED pin (red)
#define UART_DEBUG USART3  // Wired to the on-board st-link debugger

#include <stm32f767xx.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define BIT(x) (1UL << (x))
#define CLRSET(reg, clear, set) ((reg) = ((reg) & ~(clear)) | (set))
#define PIN(bank, num) ((((bank) - 'A') << 8) | (num))
#define PINNO(pin) (pin & 255)
#define PINBANK(pin) (pin >> 8)

void hal_init(void);
size_t hal_ram_free(void);
size_t hal_ram_used(void);

/* System clock
APB1 clock <= 54MHz; APB2 clock <= 108MHz
3.3.2, Table 5: configure flash latency (WS) in accordance to clock freq
38.4: The AHB clock frequency must be at least 25 MHz when the Ethernet
controller is used */
enum { APB1_PRE = 4 /* AHB clock / 4 */, APB2_PRE = 2 /* AHB clock / 2 */ };
enum { /*PLL_HSI=16*/OSC_IN=8,PLL_M=4,PLL_N=216,PLL_P=2,PLL_Q=9,PLL_R=2};  // Run at 216 Mhz
#define FLASH_LATENCY 7
#define SYS_FREQUENCY ((/*PLL_HSI*/OSC_IN * PLL_N / PLL_M / PLL_P) * 1000000)
#define APB2_FREQUENCY (SYS_FREQUENCY / (BIT(APB2_PRE - 3)))
#define APB1_FREQUENCY (SYS_FREQUENCY / (BIT(APB1_PRE - 3)))

enum { GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_AF, GPIO_MODE_ANALOG };
enum { GPIO_OTYPE_PUSH_PULL, GPIO_OTYPE_OPEN_DRAIN };
enum { GPIO_SPEED_LOW, GPIO_SPEED_MEDIUM, GPIO_SPEED_HIGH, GPIO_SPEED_INSANE };
enum { GPIO_PULL_NONE, GPIO_PULL_UP, GPIO_PULL_DOWN };
#define GPIO(N) ((GPIO_TypeDef *) (0x40020000 + 0x400 * (N)))

static GPIO_TypeDef *gpio_bank(uint16_t pin) {
  return GPIO(PINBANK(pin));
}
static inline void gpio_toggle(uint16_t pin) {
  GPIO_TypeDef *gpio = gpio_bank(pin);
  uint32_t mask = BIT(PINNO(pin));
  gpio->BSRR = mask << (gpio->ODR & mask ? 16 : 0);
}
static inline int gpio_read(uint16_t pin) {
  return gpio_bank(pin)->IDR & BIT(PINNO(pin)) ? 1 : 0;
}
static inline void gpio_write(uint16_t pin, bool val) {
  GPIO_TypeDef *gpio = gpio_bank(pin);
  gpio->BSRR = BIT(PINNO(pin)) << (val ? 0 : 16);
}
static inline void gpio_init(uint16_t pin, uint8_t mode, uint8_t type,
                             uint8_t speed, uint8_t pull, uint8_t af) {
  GPIO_TypeDef *gpio = gpio_bank(pin);
  uint8_t n = (uint8_t) (PINNO(pin));
  RCC->AHB1ENR |= BIT(PINBANK(pin));  // Enable GPIO clock
  CLRSET(gpio->OTYPER, 1UL << n, ((uint32_t) type) << n);
  CLRSET(gpio->OSPEEDR, 3UL << (n * 2), ((uint32_t) speed) << (n * 2));
  CLRSET(gpio->PUPDR, 3UL << (n * 2), ((uint32_t) pull) << (n * 2));
  CLRSET(gpio->AFR[n >> 3], 15UL << ((n & 7) * 4),
         ((uint32_t) af) << ((n & 7) * 4));
  CLRSET(gpio->MODER, 3UL << (n * 2), ((uint32_t) mode) << (n * 2));
}
static inline void gpio_input(uint16_t pin) {
  gpio_init(pin, GPIO_MODE_INPUT, GPIO_OTYPE_PUSH_PULL, GPIO_SPEED_HIGH,
            GPIO_PULL_NONE, 0);
}
static inline void gpio_output(uint16_t pin) {
  gpio_init(pin, GPIO_MODE_OUTPUT, GPIO_OTYPE_PUSH_PULL, GPIO_SPEED_HIGH,
            GPIO_PULL_NONE, 0);
}

static inline void leds_init(void) {
  gpio_output(LED1);              // Initialise LED1
  gpio_output(LED2);              // Initialise LED2
  gpio_output(LED3);              // Initialise LED3
}

static inline void irq_exti_attach(uint16_t pin) {
  uint8_t bank = (uint8_t) (PINBANK(pin)), n = (uint8_t) (PINNO(pin));
  SYSCFG->EXTICR[n / 4] &= ~(15UL << ((n % 4) * 4));
  SYSCFG->EXTICR[n / 4] |= (uint32_t) (bank << ((n % 4) * 4));
  EXTI->IMR |= BIT(n);
  EXTI->RTSR |= BIT(n);
  EXTI->FTSR |= BIT(n);
  int irqvec = n < 5 ? 6 + n : n < 10 ? 23 : 40;  // IRQ vector index, 10.1.2
  NVIC_SetPriority(irqvec, 3);
  NVIC_EnableIRQ(irqvec);
}

static inline void uart_init(USART_TypeDef *uart, unsigned long baud) {
  uint8_t af = 7;           // Alternate function
  uint16_t rx = 0, tx = 0;  // pins
  uint32_t freq = 0;        // Bus frequency. UART1 is on APB2, rest on APB1

  if (uart == USART1) freq = APB2_FREQUENCY, RCC->APB2ENR |= BIT(4);
  if (uart == USART2) freq = APB1_FREQUENCY, RCC->APB1ENR |= BIT(17);
  if (uart == USART3) freq = APB1_FREQUENCY, RCC->APB1ENR |= BIT(18);

  if (uart == USART1) tx = PIN('A', 9), rx = PIN('A', 10);
  if (uart == USART2) tx = PIN('A', 2), rx = PIN('A', 3);
  if (uart == USART3) tx = PIN('D', 8), rx = PIN('D', 9);

  gpio_init(tx, GPIO_MODE_AF, GPIO_OTYPE_PUSH_PULL, GPIO_SPEED_HIGH, 0, af);
  gpio_init(rx, GPIO_MODE_AF, GPIO_OTYPE_PUSH_PULL, GPIO_SPEED_HIGH, 0, af);
  uart->CR1 = 0;                          // Disable this UART
  uart->BRR = freq / baud;                // Set baud rate
  uart->CR1 |= BIT(0) | BIT(2) | BIT(3);  // Set UE, RE, TE
}
static inline void uart_write_byte(USART_TypeDef *uart, uint8_t byte) {
  uart->TDR = byte;
  while ((uart->ISR & BIT(7)) == 0) (void) 0;
}
static inline void uart_write_buf(USART_TypeDef *uart, char *buf, size_t len) {
  while (len-- > 0) uart_write_byte(uart, *(uint8_t *) buf++);
}
static inline int uart_read_ready(USART_TypeDef *uart) {
  return uart->ISR & BIT(5);  // If RXNE bit is set, data is ready
}
static inline uint8_t uart_read_byte(USART_TypeDef *uart) {
  return (uint8_t) (uart->RDR & 255);
}

static inline void rng_init(void) {
  RCC->AHB2ENR |= RCC_AHB2ENR_RNGEN;
  RNG->CR |= RNG_CR_RNGEN;
}
static inline uint32_t rng_read(void) {
  while ((RNG->SR & RNG_SR_DRDY) == 0) (void) 0;
  return RNG->DR;
}

// Hw pull-ups on PHY RXD0,1,DV to enable autonegotiation
static inline void ethernet_init(void) {
  // Initialise Ethernet. Enable MAC GPIO pins, see
  // https://www.farnell.com/datasheets/2014265.pdf section 6.10
  uint16_t pins[] = {PIN('A', 1),  PIN('A', 2),  PIN('A', 7),
                     PIN('B', 13), PIN('C', 1),  PIN('C', 4),
                     PIN('C', 5),  PIN('G', 11), PIN('G', 13)};
  for (size_t i = 0; i < sizeof(pins) / sizeof(pins[0]); i++) {
    gpio_init(pins[i], GPIO_MODE_AF, GPIO_OTYPE_PUSH_PULL, GPIO_SPEED_INSANE,
              GPIO_PULL_NONE, 11);  // 11 is the Ethernet AF_function
  }
  NVIC_EnableIRQ(ETH_IRQn);                // Setup Ethernet IRQ handler
  SYSCFG->PMC |= SYSCFG_PMC_MII_RMII_SEL;  // Use RMII. Goes first!
  RCC->AHB1ENR |=
      RCC_AHB1ENR_ETHMACEN | RCC_AHB1ENR_ETHMACTXEN | RCC_AHB1ENR_ETHMACRXEN;
}

static inline void wizard_clock_init(void) {
  FLASH->ACR |= FLASH_LATENCY | BIT(8) | BIT(9);    // Flash latency, prefetch
  RCC->PLLCFGR &= ~((BIT(17) - 1));                 // Clear PLL multipliers
  RCC->PLLCFGR |= (((PLL_P - 2) / 2) & 3) << 16;    // Set PLL_P
  RCC->PLLCFGR |= PLL_M | (PLL_N << 6);             // Set PLL_M and PLL_N
  RCC->CR |= BIT(24);                               // Enable PLL
  while ((RCC->CR & BIT(25)) == 0) (void) 0;        // Wait until done
  RCC->CFGR = (APB1_PRE << 10) | (APB2_PRE << 13);  // Set prescalers
  RCC->CFGR |= 2;                                   // Set clock source to PLL
  while ((RCC->CFGR & 12) == 0) (void) 0;           // Wait until done
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;             // Enable SYSCFG
}
//STM32Cube/Repository/STM32Cube_FW_F7_V1.17.3/Drivers/STM32F7xx_HAL_Driver$ 
//grep -C 2 -n -r 'LL_RCC_SetTIMPrescaler' {Src,Inc}
//WIP supply clock init, using cubes's alg, but no ll or hal calls > bare register access(use stm32f767xx.h)
//ctag -R * pinche f7Cube & ctrl ](checkout tutorial for other cool shortcuts) !!FU!! cube, multiple times
//stm32f7xx.h:181-195
//#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
//#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
//#define READ_BIT(REG, BIT)    ((REG) & (BIT))
//#define CLEAR_REG(REG)        ((REG) = (0x0))
//#define WRITE_REG(REG, VAL)   ((REG) = (VAL))
//#define READ_REG(REG)         ((REG))
//#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))
//val = reg & ~clearmask | setmask
//reg = val
static inline void clock_init(void){
//cube main
SCB_EnableICache(); SCB_EnableDCache();//core_cm7.h:2229 static_inline voids
//Reset of all peripherals, Initializes the Flash interface and the Systick.
//LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
//         \/                  /\ *ll_bus.h defRemap to RCC_APB1ENR_PWREN 767xx.h;
//__IO uint32_t tmpreg;SET_BIT(RCC->APB1ENR, Periphs);//__IO volatile -r '#define\s*__IO' cmsis5
//tmpreg = READ_BIT(RCC->APB1ENR, Periphs);(void)tmpreg;//??for what?? Delay after an RCC peripheral clock enabling
// fu cube, wtf is grp1?
RCC->APB1ENR|=RCC_APB1ENR_PWREN;//LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
RCC->APB2ENR|=RCC_APB2ENR_SYSCFGEN;//LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
//system interrupt init
//(void)((uint32_t)0x00000003U) ??? NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4/*hal_cortex.h*/);
//BS from cube: main.h #define NVIC_PRIORITYGROUP_4 ((uint32_t)0x00000003) 4 bits for pre-emption priority, 0 bit subprio
//config system clock
//SystemClock_Config();gpio_init; uart3_init; while(1)//main loop
//void SystemClock_Config(void)
//{
//LL_FLASH_SetLatency(LL_FLASH_LATENCY_7);// MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, Latency);
FLASH->ACR |= (FLASH_LATENCY | BIT(8) | BIT(9) );
//val = flash->rcc & ~flash_acr_latency | latency7
//val =  & ~0x0000000F | 0x00000007U
//while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_7){} return (uint32_t)(READ_BIT(FLASH->ACR, FLASH_ACR_LATENCY));
while((FLASH->ACR&FLASH_ACR_LATENCY)!=FLASH_ACR_LATENCY_7WS){}
//LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);//MODIFY_REG(PWR->CR1, PWR_CR1_VOS, VoltageScaling);
//ll_pwr.h:104-106                            767xx.h
//#define LL_PWR_REGU_VOLTAGE_SCALE3         PWR_CR1_VOS_0
//#define LL_PWR_REGU_VOLTAGE_SCALE2         PWR_CR1_VOS_1
//#define LL_PWR_REGU_VOLTAGE_SCALE1         (PWR_CR1_VOS_0 | PWR_CR1_VOS_1)
PWR->CR1|=(PWR_CR1_VOS_0|PWR_CR1_VOS_1|PWR_CR1_ODEN);//Entering Over-drive mode rm0410 ink125
//LL_PWR_EnableOverDriveMode();//SET_BIT(PWR->CR1, PWR_CR1_ODEN)
//LL_RCC_HSE_EnableBypass();//SET_BIT(RCC->CR, RCC_CR_HSEBYP);
//LL_RCC_HSE_Enable();//SET_BIT(RCC->CR, RCC_CR_HSEON);
RCC->CR|=(RCC_CR_HSEBYP|RCC_CR_HSEON);
//Wait till HSE is ready 
//while(LL_RCC_HSE_IsReady() != 1){}return (READ_BIT(RCC->CR, RCC_CR_HSERDY) == (RCC_CR_HSERDY));
while((RCC->CR & RCC_CR_HSERDY)!=RCC_CR_HSERDY){}
//LL_RCC_HSE_EnableCSS();//SET_BIT(RCC->CR, RCC_CR_CSSON);
RCC->CR|=RCC_CR_CSSON;
//LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_4, 216, LL_RCC_PLLP_DIV_2);
//MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC | RCC_PLLCFGR_PLLM | RCC_PLLCFGR_PLLN | RCC_PLLCFGR_PLLP,
//          Source | PLLM | PLLN << RCC_PLLCFGR_PLLN_Pos | PLLP);
RCC->PLLCFGR = (RCC_PLLCFGR_PLLSRC_HSE)      /*0x0040_0000 0x0040_0000 */
| (PLL_M<<RCC_PLLCFGR_PLLM_Pos)              /*0x0000_0008 0x0040_0008 */
| (PLL_N << RCC_PLLCFGR_PLLN_Pos)            /*0x0000_3600 0x0040_3608 */
| (((PLL_P >> 1) -1) << RCC_PLLCFGR_PLLP_Pos)/*0x0000_0000 0x0040_3608 2:00;4:01;6:10;8:11*/
| (PLL_Q<<RCC_PLLCFGR_PLLQ_Pos)              /*0x0900_0000 0x0940_3608 */
| (PLL_R<<RCC_PLLCFGR_PLLR_Pos)              /*0x2000_0000 0x2940_3608.assert_equals(RCC_PLLCFGR) */
;

//LL_RCC_PLL_Enable();//SET_BIT(RCC->CR, RCC_CR_PLLON);
RCC->CR|=RCC_CR_PLLON;

//Wait till PLL is ready
//while(LL_RCC_PLL_IsReady() != 1){}return (READ_BIT(RCC->CR, RCC_CR_PLLRDY) == (RCC_CR_PLLRDY));
while((RCC->CR&RCC_CR_PLLRDY)!=RCC_CR_PLLRDY){}
//while (LL_PWR_IsActiveFlag_VOS() == 0){}return (READ_BIT(PWR->CSR1, PWR_CSR1_VOSRDY) == (PWR_CSR1_VOSRDY));
while((PWR->CSR1&PWR_CSR1_VOSRDY)!=PWR_CSR1_VOSRDY){}
//LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, Prescaler);
//LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, Prescaler);
//LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, Prescaler);
//LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, Source);
RCC->CFGR|= RCC_CFGR_HPRE_DIV1|RCC_CFGR_PPRE1_DIV4|RCC_CFGR_PPRE2_DIV2|RCC_CFGR_SW_PLL;

//wait till System clock is ready
//while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL){}return (uint32_t)(READ_BIT(RCC->CFGR, RCC_CFGR_SWS));
while((RCC->CFGR&/*sws set by HW*/RCC_CFGR_SWS)!=RCC_CFGR_SWS_PLL){}
//LL_Init1msTick(216000000);//ll_inittick, same as systick_init(cpq)
//LL_SetSystemCoreClock(216000000);SystemCoreClock = HCLKFrequency;

//Set Timers Clock Prescalers
//LL_RCC_SetTIMPrescaler(LL_RCC_TIM_PRESCALER_TWICE);MODIFY_REG(RCC->DCKCFGR1, RCC_DCKCFGR1_TIMPRE, Prescaler);
//}//close SystemClock_Config
}//close clock_init

static inline void system_init(void) {
//Core/Include/core_cm7.h:481:  __IOM uint32_t CPACR;/* Offset: 0x088 (R/W)  Coprocessor Access Control Register */
SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2));  // Enable FPU//enable cp10 cp11 full access
//Core/Include/cmsis_armcc.h:429
__DSB();//DataSynchronizationBarrier|completes when all explicit memory accesses before this instruction complete.
__ISB();//Instruction Synchronization Barrier flushes the pipeline in the processor,
//so that all instructions following the ISB are fetched from cache or memory,
//after the instruction has been completed.
}

