#include "boot.h"
#include "riscv.h"

void setup_pmp(uint64_t kernel_start, uint64_t kernel_size) {
    // Round up kernel size to 4KB boundary
    uint64_t kernel_end = kernel_start + kernel_size;
    kernel_end = (kernel_end + 0xfff) & ~0xfffUL;
    
    // PMP entry 0: Kernel region R+X (TOR mode)
    // pmpaddr0 = kernel_start >> 2
    // pmpaddr1 = kernel_end >> 2
    // pmpcfg0 = R+X+L (locked)
    
    write_csr(pmpaddr0, kernel_start >> 2);
    write_csr(pmpaddr1, kernel_end >> 2);
    
    // Configure PMP0: TOR, R+X, Locked
    uint64_t cfg = (PMP_R | PMP_X | PMP_A_TOR | PMP_L);
    write_csr(pmpcfg0, cfg);
    
    uart_puts("[PMP]  Region 0: ");
    uart_puthex(kernel_start);
    uart_puts(" - ");
    uart_puthex(kernel_end);
    uart_puts(" (R+X+L)\n");
}