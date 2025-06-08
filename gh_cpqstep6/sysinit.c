#include "hal.h"

uint32_t SystemCoreClock = SYS_FREQUENCY;
//hal.h > stm32f767xx.h > system_stm32f7xx.h: system_exported_functions\/
//extern void SystemInit(void);extern void SystemCoreClockUpdate(void);
//Called from cmsis_f7/Source/Templates/gcc/startup_stm32f767xx.s:61 bl SystemInit
void cpqHSI(void) {
  SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2));  // Enable FPU
  FLASH->ACR |= FLASH_LATENCY | BIT(8) | BIT(9);      // Flash latency, prefetch
  RCC->PLLCFGR &= ~((BIT(17) - 1));                   // Clear PLL multipliers
  RCC->PLLCFGR |= (((PLL_P - 2) / 2) & 3) << 16;      // Set PLL_P
  RCC->PLLCFGR |= PLL_M | (PLL_N << 6);               // Set PLL_M and PLL_N
  RCC->CR |= BIT(24);                                 // Enable PLL
  while ((RCC->CR & BIT(25)) == 0) spin(1);           // Wait until done
  RCC->CFGR = (APB1_PRE << 10) | (APB2_PRE << 13);    // Set prescalers
  RCC->CFGR |= 2;                                     // Set clock source to PLL
  while ((RCC->CFGR & 12) == 0) spin(1);              // Wait until done
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;    // Enable SYSCFG
  SysTick_Config(SystemCoreClock / 1000);  // Sys tick every 1ms
}



//(arm)systemControlBlock->coprocessorAccessControlRegister
//SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2));
//FLASH->ACR |= FLASH_LATENCY | BIT(8) | BIT(9);      

//void rcc_clock_setup_hse(const struct rcc_clock_scale *clock, uint32_t hse_mhz)
void locm(void){
//uint8_t pllm = hse_mhz;
/* Enable internal high-speed oscillator.
rcc_osc_on(RCC_HSI);rcc_wait_for_osc_ready(RCC_HSI);*/
/* Select HSI as SYSCLK source. rcc_set_sysclk_source(RCC_CFGR_SW_HSI);*/
/* Enable external high-speed oscillator.
rcc_osc_on(RCC_HSE);
rcc_wait_for_osc_ready(RCC_HSE);rcc_periph_clock_enable(RCC_PWR);
pwr_set_vos_scale(clock->vos_scale);
if (clock->overdrive) pwr_enable_overdrive();
*/
/*Set prescalers for AHB, ADC, APB1, APB2.
* Do this before touching the PLL (TODO: why?)
rcc_set_hpre(clock->hpre);
rcc_set_ppre1(clock->ppre1);
rcc_set_ppre2(clock->ppre2);*/
/* Disable PLL oscillator before changing its configuration.
rcc_osc_off(RCC_PLL);*/
/* Configure the PLL oscillator.
rcc_set_main_pll_hse(pllm, clock->plln, clock->pllp, clock->pllq);*/
/* Enable PLL oscillator and wait for it to stabilize.
rcc_osc_on(RCC_PLL);rcc_wait_for_osc_ready(RCC_PLL);*/
/* Configure flash settings.
flash_set_ws(clock->flash_waitstates);
flash_art_enable();
flash_prefetch_enable();*/
/* Select PLL as SYSCLK source. rcc_set_sysclk_source(RCC_CFGR_SW_PLL);*/
/* Wait for PLL clock to be selected.rcc_wait_for_sysclk_status(RCC_PLL);*/
/* Set the clock frequencies used.
rcc_ahb_frequency = clock->ahb_frequency;
rcc_apb1_frequency = clock->apb1_frequency;
rcc_apb2_frequency = clock->apb2_frequency;*/
/* Disable internal high-speed oscillator. rcc_osc_off(RCC_HSI);*/
}//end locm

//Drivers/STM32F7xx_HAL_Driver/Inc/stm32f7xx_ll_system.h:891:
//__STATIC_INLINE void LL_FLASH_SetLatency(uint32_t Latency)

void ll_tempate(void){ //Templates_LL/Src/main.c

//core_cm7.h:2229 static_inline voids
SCB_EnableICache(); SCB_EnableDCache();
// Set FLASH latency LL_FLASH_SetLatency(LL_FLASH_LATENCY_7);
SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2));  // Enable FPU
FLASH->ACR |= FLASH_LATENCY | BIT(8) | BIT(9);      // Flash latency, prefetch

// Enable PWR clock LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR); RCC_APB1ENR_PWREN
RCC->APB1ENR = RCC_APB1ENR_PWREN;
// Activation OverDrive Mode LL_PWR_EnableOverDriveMode(); SET_BIT(PWR->CR1, PWR_CR1_ODEN);
//while(LL_PWR_IsActiveFlag_OD() != 1){}; READ_BIT(PWR->CSR1, PWR_CSR1_ODRDY) == (PWR_CSR1_ODRDY));
// Activation OverDrive Switching LL_PWR_EnableOverDriveSwitching(); SET_BIT(PWR->CR1, PWR_CR1_ODSWEN);
//while(LL_PWR_IsActiveFlag_ODSW() != 1){};
//need dig deeper about overdriveMode & scale modes & if still needed when pll as sysck
  
  /* Main PLL configuration and activation */
  //LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_16, 432, LL_RCC_PLLP_DIV_2);
  //LL_RCC_PLL_Enable();
  //while(LL_RCC_PLL_IsReady() != 1) {};
  /* Sysclk activation on the main PLL */
  //LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  //LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  //while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL){};
  /* Set APB1 & APB2 prescaler*/
  //LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
  /* Set systick to 1ms */
  SysTick_Config(216000000 / 1000);
  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  SystemCoreClock = 216000000; 
}//template

//blog.embeddedexpert.io/?p=531
void p531(void)//void SysClockConfig(void) //set the core frequency to 216MHz
{
//#define PLL_M      4
//#define PLL_N      216
//#define PLL_P      2
__IO uint32_t StartUpCounter = 0, HSEStatus = 0;
RCC->CR |= RCC_CR_CSSON;
RCC->CR |= RCC_CR_HSEBYP;
RCC->CR |= ((uint32_t)RCC_CR_HSEON);
do{HSEStatus = RCC->CR & RCC_CR_HSERDY;StartUpCounter++;} 
while((HSEStatus == 0) && (StartUpCounter != 3000));
if ((RCC->CR & RCC_CR_HSERDY) != 0/*RESET*/) HSEStatus = (uint32_t)0x01;
else HSEStatus = (uint32_t)0x00;

if (HSEStatus == (uint32_t)0x01){
RCC->APB1ENR |= RCC_APB1ENR_PWREN;
PWR->CR1 &= (uint32_t)~(PWR_CR1_VOS);
RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;
RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
RCC->PLLCFGR = PLL_M | (PLL_N << RCC_PLLCFGR_PLLN_Pos) | (((PLL_P >> 1) -1) << RCC_PLLCFGR_PLLP_Pos) |
           (RCC_PLLCFGR_PLLSRC_HSE);
RCC->CR |= RCC_CR_PLLON;
while((RCC->CR & RCC_CR_PLLRDY) == 0){}
/* Configure Flash prefetch, Instruction cache, Data cache and wait state */
FLASH->ACR = FLASH_ACR_LATENCY_7WS;
/* Select the main PLL as system clock source */
RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
RCC->CFGR |= RCC_CFGR_SW_PLL;
/* Wait till the main PLL is used as system clock source */
while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL)
{;}
}//status 0x1
else
{ /* If HSE fails to start-up, the application will have wrong clock
  configuration. User can add here some code to deal with this error */
}
RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;    // Enable SYSCFG
SysTick_Config(SystemCoreClock / 1000);  // Sys tick every 1ms
//SystemCoreClockUpdate();
}//p531

void SystemInit(void){
//cpqHSI();
//locm();
//ll_template();
//p531();
clock_init();// > hal.h
}

