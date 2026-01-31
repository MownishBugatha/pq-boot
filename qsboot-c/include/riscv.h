#ifndef RISCV_H
#define RISCV_H

#include "types.h"

// CSR access macros
#define read_csr(reg) ({ \
    uint64_t __val; \
    asm volatile("csrr %0, " #reg : "=r"(__val)); \
    __val; \
})

#define write_csr(reg, val) ({ \
    asm volatile("csrw " #reg ", %0" :: "r"(val)); \
})

#define set_csr(reg, bit) ({ \
    asm volatile("csrs " #reg ", %0" :: "r"(bit)); \
})

#define clear_csr(reg, bit) ({ \
    asm volatile("csrc " #reg ", %0" :: "r"(bit)); \
})

// CSR definitions
#define MSTATUS_MIE  (1UL << 3)
#define MSTATUS_MPIE (1UL << 7)
#define MSTATUS_MPP  (3UL << 11)

// PMP configuration
#define PMP_R     0x01
#define PMP_W     0x02
#define PMP_X     0x04
#define PMP_A     0x18
#define PMP_A_TOR 0x08
#define PMP_A_NA4 0x10
#define PMP_A_NAPOT 0x18
#define PMP_L     0x80

// UART base address (QEMU virt machine)
#define UART0_BASE 0x10000000UL

// Memory regions
#define KERNEL_START 0x80200000UL
#define KERNEL_MAX_SIZE 0x00800000UL // 8MB

static inline uint64_t read_cycle(void) {
    uint64_t val;
    asm volatile ("rdcycle %0" : "=r"(val));
    return val;
}

#endif