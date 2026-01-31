.section .text.init
.global _start
.type _start, @function

_start:
    .option push
    .option norelax
    
    # Disable interrupts
    csrw mie, zero
    csrw mip, zero
    
    # Get HART ID
    csrr t0, mhartid
    
    # Only HART 0 continues, others spin
    bnez t0, hart_spin
    
    # Clear BSS
    la t0, __bss_start
    la t1, __bss_end
1:
    bge t0, t1, 2f
    sd zero, 0(t0)
    addi t0, t0, 8
    j 1b
2:
    
    # Setup stack pointer
    la sp, __stack_top
    
    # Setup global pointer
    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop
    
    # Jump to C main
    call boot_main
    
    # Should never return
    j hart_spin

hart_spin:
    wfi
    j hart_spin

.option pop