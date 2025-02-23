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

