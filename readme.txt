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

happy debug. // try converting bluepill st-link into jlink
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

seems its time to include arm docs, to find tha answer y there are so many folderstructures(& possible duplicates) in cube+hal driver+cmsis-device
