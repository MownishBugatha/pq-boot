OUTPUT_ARCH("riscv")
ENTRY(_start)

MEMORY {
    RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 128M
}

SECTIONS {
    . = 0x80000000;
    
    .text : {
        *(.text.init)
        *(.text .text.*)
    } > RAM
    
    .rodata : ALIGN(8) {
        *(.rodata .rodata.*)
        . = ALIGN(8);
        __pubkey_start = .;
        *(.pubkey)
        __pubkey_end = .;
    } > RAM
    
    .data : ALIGN(8) {
        *(.data .data.*)
        *(.sdata .sdata.*)
    } > RAM

     .sdata : {
      __global_pointer$ = . + 0x800;
      *(.sdata*)
     }
    
     .sbss : {
         *(.sbss*)
     }
    
    .bss : ALIGN(8) {
        __bss_start = .;
        *(.bss .bss.*)
        *(.sbss .sbss.*)
        __bss_end = .;
    } > RAM
    
    . = ALIGN(16);
    . += 0x2000; /* 8KB stack */
    __stack_top = .;
}