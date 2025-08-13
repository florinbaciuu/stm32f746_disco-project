/* Memories definition */
MEMORY
{
  DTCMRAM (xrw) : ORIGIN = 0x20000000, LENGTH = 128K
  RAM_D1  (xrw) : ORIGIN = 0x20020000, LENGTH = 240K
  RAM_D2  (xrw) : ORIGIN = 0x2007C000, LENGTH = 16K
  FLASH   (rx)  : ORIGIN = 0x08000000, LENGTH = 1024K
}

/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = ORIGIN(RAM_D1) + LENGTH(RAM_D1);

/* Heap/stack size (default) */
_Min_Heap_Size = 0x200; 
_Min_Stack_Size = 0x400;

/* Sections */
SECTIONS
{
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector))
    . = ALIGN(4);
  } >FLASH

  .text :
  {
    . = ALIGN(4);
    *(.text)
    *(.text*)
    *(.glue_7)
    *(.glue_7t)
    *(.eh_frame)
    KEEP (*(.init))
    KEEP (*(.fini))
    . = ALIGN(4);
    _etext = .;
  } >FLASH

  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
  } >FLASH

  .ARM.extab (READONLY) :
  {
    . = ALIGN(4);
    *(.ARM.extab* .gnu.linkonce.armextab.*)
    . = ALIGN(4);
  } >FLASH

  .ARM (READONLY) :
  {
    . = ALIGN(4);
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
    . = ALIGN(4);
  } >FLASH

  .preinit_array (READONLY) :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
    . = ALIGN(4);
  } >FLASH

  .init_array (READONLY) :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
    . = ALIGN(4);
  } >FLASH

  .fini_array (READONLY) :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
    . = ALIGN(4);
  } >FLASH

  _sidata = LOADADDR(.data);

  .data : 
  {
    . = ALIGN(4);
    _sdata = .;
    *(.data)
    *(.data*)
    *(.RamFunc)
    *(.RamFunc*)
    . = ALIGN(4);
    _edata = .;
  } >RAM_D1 AT> FLASH

  .tdata : ALIGN(4)
  {
    *(.tdata .tdata.* .gnu.linkonce.td.*)
    . = ALIGN(4);
    _edata = .;
    PROVIDE(__data_end = .);
    PROVIDE(__tdata_end = .);
  } >RAM_D1 AT> FLASH

  .tbss (NOLOAD) : ALIGN(4)
  {
    _sbss = .;
    __bss_start__ = _sbss;
    *(.tbss .tbss.*)
    . = ALIGN(4);
    PROVIDE(__tbss_end = .);
  } >RAM_D1

  .bss (NOLOAD) : ALIGN(4)
  {
    _sbss = .;
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
    __bss_end__ = _ebss;
  } >RAM_D1

  ._user_heap_stack (NOLOAD) :
  {
    . = ALIGN(8);
    PROVIDE(end = .);
    PROVIDE(_end = .);
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >RAM_D1

  /* HEAP REGIONS for heap_5.c */
  .dtcm_heap (NOLOAD) :
  {
    . = ALIGN(32);
    _sdtcm_heap = .;
    KEEP(*(.dtcm_heap))
    _edtcm_heap = .;
  } >DTCMRAM

  .sram1_heap (NOLOAD) :
  {
    . = ALIGN(32);
    _ssram1_heap = .;
    KEEP(*(.sram1_heap))
    _esram1_heap = .;
  } >RAM_D1

  .sram2_heap (NOLOAD) :
  {
    . = ALIGN(32);
    _ssram2_heap = .;
    KEEP(*(.sram2_heap))
    _esram2_heap = .;
  } >RAM_D2

  /DISCARD/ :
  {
    libc.a: * ( * )
    libm.a: * ( * )
    libgcc.a: * ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
