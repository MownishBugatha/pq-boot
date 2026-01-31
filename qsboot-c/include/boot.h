#ifndef BOOT_H
#define BOOT_H

#include "types.h"

// Boot status codes
#define BOOT_SUCCESS 0
#define BOOT_ERROR_HASH 1
#define BOOT_ERROR_VERIFY 2
#define BOOT_ERROR_NO_KERNEL 3
#define BOOT_ERROR_PMP 4

// Function prototypes
void boot_main(void);
void setup_pmp(uint64_t kernel_start, uint64_t kernel_size);
void jump_to_kernel(uint64_t entry_point);
void jump_to_kernel(uint64_t entry_point);
// UART functions
void uart_init(void);
void uart_putc(char c);
void uart_puts(const char *s);
void uart_puthex(uint64_t val);

#endif