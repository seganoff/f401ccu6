ENTRY(Reset_Handler);
MEMORY {
  flash(rx)  : ORIGIN = 0x08000000, LENGTH = 2048k /*401ccu6 256k*/
  sram(rwx) : ORIGIN = 0x20020000, LENGTH = 384k  /*401ccu6 0x2000_0000 64k*/
}
_estack     = ORIGIN(sram) + LENGTH(sram);    /* stack points to end of SRAM */

SECTIONS {
  .vectors  : { KEEP(*(.isr_vector)) }   > flash
  .text     : { *(.text*) }           > flash
  .rodata   : { *(.rodata*) }         > flash

  .data : {
    _sdata = .;   /* .data section start */
    *(.first_data)
    *(.data SORT(.data.*))
    _edata = .;  /* .data section end */
  } > sram AT > flash
  _sidata = LOADADDR(.data);

  .bss : {
    _sbss = .;              /* .bss section start */
    *(.bss SORT(.bss.*) COMMON)
    _ebss = .;              /* .bss section end */
  } > sram

  . = ALIGN(8); /*4 libopencm gen*/
  _end = .;     /* for cmsis_gcc.h  */
}
/*PROVIDE(_stack = ORIGIN(ram) + LENGTH(ram));*//*libopencm gen*/
