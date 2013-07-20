/* This file reuses code from uCore.  See README.ucore for more details. */
/* Simple linker script for the ucore kernel.
   See the GNU ld 'info' manual ("info ld") to learn the syntax. */

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(kern_entry)

phys  = 0x00100000;
vaddr = 0xC0000000;
SECTIONS {
  /* Load the kernel at this address: "." means the current address */
  . = phys + vaddr;
  
  PROVIDE(__CODE_BEGIN__ = .);
  /* First put the multiboot header, as it is required to be put very early
    early in the image or the bootloader won't recognize the file format.
    Next we'll put the .text section. */
  .text : AT(ADDR(.text) - vaddr) {
    *(.multiboot)
    *(.text .stub .text.* .gnu.linkonce.t.*)
  }

  PROVIDE(__CODE_END__ = .);
  PROVIDE(etext = .); /* Define the 'etext' symbol to this value */

  .rodata : AT(ADDR(.rodata) - vaddr) {
    *(.rodata .rodata.* .gnu.linkonce.r.*)
  }

  /* Include debugging information in kernel memory */
  .stab : AT(ADDR(.stab) - vaddr) {
    PROVIDE(__STAB_BEGIN__ = .);
    *(.stab);
    PROVIDE(__STAB_END__ = .);
    BYTE(0)     /* Force the linker to allocate space
                for this section */
  }

  .stabstr : AT(ADDR(.stabstr) - vaddr) {
    PROVIDE(__STABSTR_BEGIN__ = .);
    *(.stabstr);
    PROVIDE(__STABSTR_END__ = .);
    BYTE(0)     /* Force the linker to allocate space
                for this section */
  }

  /* Adjust the address for the data segment to the next page */
  . = ALIGN(0x1000);

  /* The data segment */
  .data : AT(ADDR(.data) - vaddr) {
    *(.data)
  }

  PROVIDE(edata = .);

  .bss : AT(ADDR(.bss) - vaddr) {
    *(.bss)
  }

  PROVIDE(end = .);

  /DISCARD/ : {
      *(.eh_frame .note.GNU-stack)
  }
}
