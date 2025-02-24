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

#------------------767zit6--------------------------
rm0410Rev5
ds11532Rev8

3.3ds 2Mbytes
10.1rm 110 interrupt channels
2.2.2rm memory map (+3.5ds)
dtcm  128k 0x2000_0000 0x2001_ffff
sram1 368k 0x2002_0000 0x2007_bfff
sram2 16k  0x2007_c000 0x2007_ffff
