/*
******************************************************************************
**  Linker script for the Cortex-M7 core of the STM32H755ZITx (MCORE)
**
**  FLASH bank 1 (CM7 boot area)  0x08000000  1024K
**  DTCM                          0x20000000   128K  (CPU only: no DMA access)
**  AXI SRAM  (D1)                0x24000000   512K  <- .data/.bss/heap/stack
**  SRAM1/2/3 (D2)                0x30000000   288K  Cortex-M4 only (see the CM4 script)
**  SRAM4     (D3)                0x38000000    64K  <- .ipc_shared (both cores)
**
**  .data/.bss live in AXI SRAM so that DMA1/DMA2 (UART, TIM, I2C buffers)
**  can reach them. SRAM4 is shared with the Cortex-M4 and is made
**  non-cacheable by the MPU region set up in mcore/core/mpu.cpp.
**
**  Flash bank 2 (0x08100000) holds the Cortex-M4 image.
******************************************************************************
*/

ENTRY(Reset_Handler)

_estack = ORIGIN(RAM_D1) + LENGTH(RAM_D1);   /* end of AXI SRAM */

_Min_Heap_Size = 0x200;   /* required amount of heap  */
_Min_Stack_Size = 0x1000; /* required amount of stack (tasks nest on it, see mcore/sched) */

MEMORY
{
  FLASH    (rx)  : ORIGIN = 0x08000000, LENGTH = 1024K
  DTCMRAM  (xrw) : ORIGIN = 0x20000000, LENGTH = 128K
  RAM_D1   (xrw) : ORIGIN = 0x24000000, LENGTH = 512K
  RAM_D3   (rw)  : ORIGIN = 0x38000000, LENGTH = 64K
  ITCMRAM  (xrw) : ORIGIN = 0x00000000, LENGTH = 64K
}

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

  . = ALIGN(4);
  .bss :
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

  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >RAM_D1

  /* Memory shared with the Cortex-M4 (app/ipc_shared.hpp). NOLOAD: the
     Cortex-M7 initialises it before it releases the M4. Both images must
     place it at the very start of SRAM4. */
  .ipc_shared (NOLOAD) : { KEEP(*(.ipc_shared)) } >RAM_D3
  ASSERT(ADDR(.ipc_shared) == ORIGIN(RAM_D3), "ipc_shared must start SRAM4")

  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
