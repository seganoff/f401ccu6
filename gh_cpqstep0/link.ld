ENTRY(_reset);
MEMORY {
/*flash(rx)  : ORIGIN = 0x08000000, LENGTH = 2048k*/
  flash(rx)  : ORIGIN = 0x08000000, LENGTH = 256k /*401ccu6*/
/*sram(rwx) : ORIGIN = 0x20000000, LENGTH = 192k   remaining 64k in a separate address space */
  sram(rwx) : ORIGIN = 0x20000000, LENGTH = 64k  /*401ccu6*/
}
_estack     = ORIGIN(sram) + LENGTH(sram);    /* stack points to end of SRAM */

SECTIONS {
  .vectors  : { KEEP(*(.vectors)) }   > flash
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
