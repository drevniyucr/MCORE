/*
******************************************************************************
**  Linker script for the Cortex-M4 core of the STM32H755ZITx (MCORE)
**
**  FLASH bank 2 (CM4 boot area)  0x08100000  1024K
**  SRAM1/2   (D2)                0x30000000   256K  <- .data/.bss/heap/stack
**  SRAM3     (D2)                0x30040000    32K  <- Ethernet descriptors/buffers
**  SRAM4     (D3)                0x38000000    64K  <- .ipc_shared (both cores)
**
**  The Cortex-M4 runs the network stack. D2 SRAM is clocked for the M4
**  from reset. The M4 has no data cache, so the Ethernet DMA pool and the
**  shared area need no MPU setup on this side. Addresses are the 0x30000000
**  aliases so that any bus master (ETH DMA, DMA1/2) can use the buffers.
******************************************************************************
*/

ENTRY(Reset_Handler)

_estack = ORIGIN(RAM_D2) + LENGTH(RAM_D2);   /* end of SRAM1/2 */

_Min_Heap_Size = 0x200;   /* required amount of heap  */
_Min_Stack_Size = 0x2000;  /* required amount of stack: 4 KB peak measured with TLS (cm4_stats.stack_free_min) */

MEMORY
{
  FLASH    (rx)  : ORIGIN = 0x08100000, LENGTH = 1024K
  RAM_D2   (xrw) : ORIGIN = 0x30000000, LENGTH = 256K
  ETH_POOL (rw)  : ORIGIN = 0x30040000, LENGTH = 32K
  RAM_D3   (rw)  : ORIGIN = 0x38000000, LENGTH = 64K
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
  } >RAM_D2 AT> FLASH

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
  } >RAM_D2

  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >RAM_D2

  /* Ethernet DMA pool (NOLOAD: initialised by the driver) */
  .RxDecripSection (NOLOAD) : { . = ALIGN(32); *(.RxDecripSection) } >ETH_POOL
  .TxDecripSection (NOLOAD) : { . = ALIGN(32); *(.TxDecripSection) } >ETH_POOL
  .RxPoolSection   (NOLOAD) : { . = ALIGN(32); *(.RxPoolSection)   } >ETH_POOL
  .TxPoolSection   (NOLOAD) : { . = ALIGN(32); *(.TxPoolSection)   } >ETH_POOL

  /* Memory shared with the Cortex-M7 (app/ipc_shared.hpp). NOLOAD: the
     Cortex-M7 initialises it before it releases this core. Both images must
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
