ENTRY(Reset_Handler);
MEMORY {
  flash(rx) : ORIGIN = 0x08000000, LENGTH = 2048k
  sram(rwx) : ORIGIN = 0x20020000, LENGTH = 384k
/* todo ccm section missing  */
}
_estack     = ORIGIN(sram) + LENGTH(sram);    /* stack points to end of SRAM */

SECTIONS {
  .vectors  : { KEEP(*(.isr_vector)) }  > flash
  .text     : { *(.text* .text.*) }     > flash
  .rodata   : { *(.rodata*) }           > flash

  .data : {
    _sdata = .; /* .data start */
    *(.first_data)
    *(.data SORT(.data.*))
    *(.iram .iram* .iram.*)
    _edata = .; /* .data end */
  } > sram AT > flash
  _sidata = LOADADDR(.data); /* cube: used by the startup to initialize data */

  .bss : { _sbss = .; *(.bss SORT(.bss.*) COMMON) _ebss = .; } > sram
/*   .bss start;  cube: no SORT, just:  *(.bss*)         .bss end      */
  . = ALIGN(8);
  _end = .;
}/*close SECTION*/

/* cube after .bss sectio
 User_heap_stack section, used to check that there is enough RAM left 
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >RAM



   Remove information from the standard libraries 
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }
*/
