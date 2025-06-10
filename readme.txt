github.com/cpq/bare-metal-programming-guide
follow along with f401ccu6
stmDocumentation style is so DEPRESSING, u gotta mention every little detail when citating

cpq wording | pinche stm wording
-------------------------------------
datasheet   | Reference manual rm0368 rev5
boardSheet  | UM1974 (nucleo 144-MB1137)
   _____    | ds9716 rev11

16383.to_s(16) > 0x3fff
bytes: kilo 2**10 1024 | mega 2**20 (1024*1024) 1048576
32 bit~4 bytes offset +0x4

Introduction: ram region 2.4rm(just the start address & size) 5ds 0x2000[0000-ffff]  & size: 2.3.1rm 64Kbyte
flash mem mapping 256Kbytes 5ds: 0x0800 0000 - 0x0803 ffff (0x0803_ffff+0x1-0x0800_0000)/1024 > 256 
ahb1,gpioA 5ds 0x4002_0[000-3ff] 0x400~1024~1kB

@ref pm0214 nvic
table 38 vector table 10rm, 52 interrupt channels + 16 cortex-m4
boot process cnoviello book for explainations


pre step5cmsis my Landsmann also advertises ozone, so it must be something to it. (as well as Brian Amos: Hand-on Freertos)
Nevertheless i had lots of headache with that license level 0 jlink, maybe try this time the blackmagic probe.
Actually that UM1974 according using build in st link to program external targets is full of BS (Vdd from Application)
its possible to use 3v3 line JP1, look @ page 3 in schematics mb1137.pdf

ozone, ok, chapter 6 in brian amos book. New Project, dale .svd .elf + append RTOS aware plugin
void onprojload(void)
Project.SetDevice ("STM32F767ZI");Project.SetHostIF ("USB", "778754800");
Project.SetTargetIF ("SWD");Project.SetTIFSpeed ("4 MHz");
Project.SetOSPlugin("FreeRTOSPlugin_CM7");//

happy debug. // try converting bluepill st-link into jlink, < failed
ozone level 0 jlink flashes .elf's fine(download to target). refuses to flash .bin though.

$ find /opt -name 'STM32F767.svd'
/opt/st/stm32cubeide_1.17.0/plugins/com.st.stm32cube.ide.mcu.productdb.debug_2.2.0.202411061604/resources/cmsis/STMicroelectronics_CMSIS_SVD/STM32F767.svd
/opt/st/stm32cubeclt_1.17.0/STMicroelectronics_CMSIS_SVD/STM32F767.svd
diff bur would say same files | but not same as in(CAD resources)  hXXps://www.st.com/resource/en/svd/stm32f7-svd.zip
FU stm, really, thnx for spammin directories

#------------------767zit6--------------------------
rm0410Rev5
ds11532Rev8

3.3ds 2Mbytes
10.1rm 110 interrupt channels
2.2.2rm memory map (+3.5ds)
dtcm  128k 0x2000_0000 0x2001_ffff
sram1 368k 0x2002_0000 0x2007_bfff
sram2 16k  0x2007_c000 0x2007_ffff

5.3.14rm rcc_apb2enr, bit 14 syscfgen: system config controller clock enabled|disabled
"also we must clock it via /\" 5.2rm, shortly before 5.2.1
The RCC feeds the external clock of the Cortex System Timer (SysTick) with the AHB clock
(HCLK) divided by 8. The SysTick can work either with this clock or with the Cortex clock
(HCLK), configurable in the SysTick control and status register.

step2 evry 1ms s_ticks++ into: 10µs, lower to 1 µs (16 cycles systickHandler interrupt)
blinking fails cuz time_expired + superloop (never) gets completly executed

some uart & comprehending the MESS st does with their cmsis files (but arm dictates the dir structure)
cpq includes #include "stm32f429xx.h" in step5, y not the header above, stm32f4xx.h?
apb2
4001'1400 usart6 4001'1000 usart1
apb1 
4000'7c00 uart8 4000'7800 uart7 4000'5000 uart5 4000'4c00 uart4
4000'4800 usart3 4000'4400 usart2

- cmsis5 describe arm core
Device/_Template_Vendor/ReadMe.txt dir structure for st to follow, ok

- pinche stm cmsis_device_f7 mcu periferals
arm/startup_stm32f767xx.s mdk-arm toolchain 
gcc/startup_stm32f767xx.s gcc-based toolchain
Include/system_stm32f7xx.h ifdefs include mcu specific e.g Include/stm32f767xx.h (regs defs, 18948 lines)
system_stm32f7xx.c #include "stm32f7xx.h" 
void SystemInit(void) call is made inside "startup_stm32f7xx.s" (there is no such file)
again some hot BS from st, actually they mean mcu specific(& toolchain dependant) gcc/startup_stm32f767xx.s


clone arm...cmsis5 > cd DEVICE clone gh...STM.../cmsis-device-f7 STM32F7
stm32-without-cubeide-part-2-cmsis-make-and-clock-configuration (kleinembedd...)
, but cpq cloned in root folder, not in arm's cmsis5 DEVICE folder

seems its time to include arm docs, to find tha answer y there are so many folderstructures(& possible duplicates) 
in cube+hal driver+cmsis-device

in CMSIS_5 folder $ git checkout -b 540 5.4.0
$ ln -s <cloned cmsis core> cmsis_core
$ ln -s <cloned cmsisDeviceF7> cmsis_f7
pay attention with branches/tags, 
It is crucial that you use a consistent set of versions for the CMSIS Core - CMSIS Device, 
as mentioned in this release note. this > CMSIS V5.4_CM7, again some hot BS from stm, what they mean is:
STM32CubeF7/tree/master/Drivers/CMSIS#cmsis-version-5: 5.4.0

cmsis_device_fX inconsistency, uart init, in hal.h, thanks for ref to datasheet
f429xx.h
typedef struct
{
  __IO uint32_t SR;         /*!< USART Status register,                   Address offset: 0x00 */
  __IO uint32_t DR;         /*!< USART Data register,                     Address offset: 0x04 */
  __IO uint32_t BRR;        /*!< USART Baud rate register,                Address offset: 0x08 */
  __IO uint32_t CR1;        /*!< USART Control register 1,                Address offset: 0x0C */
  __IO uint32_t CR2;        /*!< USART Control register 2,                Address offset: 0x10 */
  __IO uint32_t CR3;        /*!< USART Control register 3,                Address offset: 0x14 */
  __IO uint32_t GTPR;       /*!< USART Guard time and prescaler register, Address offset: 0x18 */
} USART_TypeDef;
f767xx.h
typedef struct
{
  __IO uint32_t CR1;    /*!< USART Control register 1,                 Address offset: 0x00 */
  __IO uint32_t CR2;    /*!< USART Control register 2,                 Address offset: 0x04 */
  __IO uint32_t CR3;    /*!< USART Control register 3,                 Address offset: 0x08 */
  __IO uint32_t BRR;    /*!< USART Baud rate register,                 Address offset: 0x0C */
  __IO uint32_t GTPR;   /*!< USART Guard time and prescaler register,  Address offset: 0x10 */
  __IO uint32_t RTOR;   /*!< USART Receiver Time Out register,         Address offset: 0x14 */
  __IO uint32_t RQR;    /*!< USART Request register,                   Address offset: 0x18 */
  __IO uint32_t ISR;    /*!< USART Interrupt and status register,      Address offset: 0x1C */
  __IO uint32_t ICR;    /*!< USART Interrupt flag Clear register,      Address offset: 0x20 */
  __IO uint32_t RDR;    /*!< USART Receive Data register,              Address offset: 0x24 */
  __IO uint32_t TDR;    /*!< USART Transmit Data register,             Address offset: 0x28 */
} USART_TypeDef;
nah, aint gonna use uart

lib/stm32/f4/rcc.c libopencm3 for blackpill wif 25MHz HSE
const struct rcc_clock_scale rcc_hse_25mhz_3v3[RCC_CLOCK_3V3_END] = {
	{ /* 84MHz */
		.pllm = 25,
		.plln = 336,
		.pllp = 4,
		.pllq = 7,
		.pllr = 0,
		.pll_source = RCC_CFGR_PLLSRC_HSE_CLK,
		.hpre = RCC_CFGR_HPRE_NODIV,
		.ppre1 = RCC_CFGR_PPRE_DIV2,
		.ppre2 = RCC_CFGR_PPRE_NODIV,
		.voltage_scale = PWR_SCALE1,
		.flash_config = FLASH_ACR_DCEN | FLASH_ACR_ICEN |
				FLASH_ACR_LATENCY_2WS,
		.ahb_frequency  = 84000000,
		.apb1_frequency = 42000000,
		.apb2_frequency = 84000000,
------- step 6 -----------
3.3.2rm Read access latency
"- When VOS[1:0] = '0x11, the maximum value of fHCLK is 180 MHz. It can be extended to
216 MHz by activating the over-drive mode."
Table7 7WS(8 cpu cycles) 210 < HCLK ≤ 216
FLASH->ACR |= FLASH_LATENCY | BIT(8) prefetch enable  | BIT(9) art enable ;
> for f429 its rm0090Rev21 3.5.1 Table12 HCKL in MHz: Highes Voltage Range 
(highest possible) 5WS  150<HCLK<=180 ;;; same note bout overdrive mode for 180MHz
max values for apb1 clock & apb2 clock
5.3.3rm RCC_CFGR bits 15:13 PPRE2 APB2 not to exceed 90MHz,Bits 12:10 PPRE1 APB1 not to exceed 45MHz
PLL Values, "Here we chose the values manually." lyka how? > RCC_PLLCFGR
5.2.3rm PLL
Since the main-PLL configuration parameters cannot be changed once PLL is enabled, it is
recommended to configure PLL before enabling it (selection of the HSI or HSE oscillator as
PLL clock source, and configuration of division factors M, N, P, and Q). not mentioning R here?
5.3.2rm RCC PLL configuration register (RCC_PLLCFGR) pll clockout according to formulas: 4
f vco_clock = f pll_clock_input  * N / M
f pll_gen_out = f vco / P ; pll general output, hot BS, imo its pllclk
!!!dont use those formulas!!! two times hot BS
> Bits description provide better

Comparing with with Figure 13. & text 5.2.3 & Bits description PLLCFGR
f VCO = 1/M * ( HSI || HSE ) * N ; PLLCLK = f VCO / P
RCC_PLLCFGR  otg>=48MHz & sdmmc+rng <=48MHz ; PLLM: to limit pll jitter recommanded to select 2MHz
2 <= PLLR <= 7 | 2 ≤ PLLQ ≤ 15 | PLLP = 2, 4, 6, or 8 | 50 ≤ PLLN ≤ 432 | 2 ≤ PLLM ≤ 63
               |pll48clk       |   <=216MHz          |100<=VCOout<=432MHz|VCOin between [1..2]MHz
RCC_PLLCFGR
0. chose HSI||HSE this gonna be input f, from this chose M: same as input(VCOin 1) or input/2 (VCOin 2)
1. pick N, VCOin * N = VCOout
2. pick P, pllclk = VCOout / P
3. pick Q, pll48  = VCOout / Q
oscillator in = 8MHz > M:4 VCOin=2MHz N:216 fVCO=432MHz P:2 VCOout=216MHz Q:9 fVCO/48MHz R:2 > plldsi=216MHz
7wait cycles; ppre2 apb2 <=90MHz ppre1 apb1<=45MHz
from cube clock config: sysclk 216 ahp_prescaler:1 hclk:216(max) 
apb1/4 > pclk1 54(max) apb2/2 > pclk2 108(max) cubeMX  >   ???<=90 & <=45 ???
5.2.1rm HSE bypass, RCC_CR HSEBYP 18 & HSEON 16 bits 5.2.3rm RCC_PLLCFGR _CFGR 5.2.7rm nmi exception

3.15 Clocks & startup ds11532rev8
'The maximum frequency of the two AHB
buses is 216 MHz while the maximum frequency of the high-speed APB domains is
108 MHz. The maximum allowed frequency of the low-speed APB domain is 54 MHz.'
There is an error in reference manual, maybe from yyp, ⚡ apb1 apb2 max frequency.

routine/alg to setup RCC &whatnots to max frequency
blog.embeddedexpert.io/?p=531
STM32CubeF7/blob/master/Projects/STM32F767ZI-Nucleo/Examples_LL/RCC/RCC_UseHSI_PLLasSystemClock/Src/main.c:489-539
STM32CubeF7/blob/master/Projects/STM32F767ZI-Nucleo/Examples_LL/RCC/RCC_UseHSEasSystemClock/Src/main.c
STM32CubeF7/blob/master/Projects/STM32F767ZI-Nucleo/Templates_LL/Src/main.c
libopencm3/blob/master/lib/stm32/f7/rcc.c:376-435
libopencm3/blob/master/lib/stm32/f4/rcc.c:789-860
examples_ll seem to be mediocre,regard as well "HAL" Examples
STM32CubeF7/blob/master/Projects/STM32F767ZI-Nucleo/Examples/RCC/RCC_ClockConfig/readme.txt
kindof HAL vs LL issue, stay away from HAL, thats the main point.

    includes start: "hal.h":1
cmsis_f7/Include/stm32f767xx.h:2+1 ./system_stm32f7xx.h:0 & (arm)cmsis_core/CMSIS/Core/Include/core_cm7.h:
arm 2x _VIRTUAL_HEADER_FILE #ifdef _VIRTUAL construct &
cmsis_version.h:0 cmsis_compiler.h:#ifdefs additional headers ;  mpu_armv7.h:0

------- step 7 ----------- reuse step6 folder
mongoose.c/.h + _custom.h(defines) net.c packed_fs.c first look seems not beeing HW dependant, 
so just cp <srcDir>/{mongoose*,packed*,net.c} ./

startup.c again? any reason to not use mcu specific one?
'Source/Templates/gcc/startup_stm32f767xx.s' vector table already matches with mcu
"The ST CMSIS package also provides startup files for all their MCUs. We can use those instead of hand-writing the startup.c.
The ST-provided startup file calls SystemInit() function, so we define it in the main.c." (cpq step5)
y degrade back to handwritten startup.c? > explaination of weak concept, but .s file defines interruptHandler weak as well
sysinit.c > moved into hal.h (static inline clock_init(void))

main.c:27 NVIC_EnableIRQ(ETH_IRQn);  // Setup Ethernet IRQ handler
         core_cm{4,7}.h   ETH_IRQn=61, f767xx.h   /*!< Ethernet global Interrupt*/
mongoose.c:6171:void ETH_IRQHandler(void) {

step5+6 gone, SysTick_Config(SystemCoreClock/1000) called instead
static inline void systick_init(uint32_t ticks) handwritten
core_cm7.h:2564:__STATIC_INLINE uint32_t SysTick_Config(uint32_t ticks) arm provided

check weather macroFunctions from hal.h like bit,pin,pinno,pinbank,setbits are already supplied by st/arm headers

not getting ip assignt by dhcp, anyway i dont wanna dig deeper why, so step 7 compiles & runs, 
but not tested the actual dashboard. maybe some other time
---------- done for now---------------------

----------attempt ++ -----------------------






