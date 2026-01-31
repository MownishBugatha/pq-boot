#include "boot.h"
#include "riscv.h"

#define UART_REG(offset) (*(volatile uint8_t*)(UART0_BASE + (offset)))
#define UART_THR 0  // Transmit Holding Register
#define UART_LSR 5  // Line Status Register
#define UART_LSR_THRE 0x20  // Transmitter Holding Register Empty

void uart_init(void) {
    // QEMU UART is pre-initialized
}

void uart_putc(char c) {
    // Wait for transmitter to be ready
    while ((UART_REG(UART_LSR) & UART_LSR_THRE) == 0);
    UART_REG(UART_THR) = c;
}

void uart_puts(const char *s) {
    while (*s) {
        if (*s == '\n') uart_putc('\r');
        uart_putc(*s++);
    }
}

void uart_puthex(uint64_t val) {
    const char hex[] = "0123456789abcdef";
    uart_puts("0x");
    for (int i = 60; i >= 0; i -= 4) {
        uart_putc(hex[(val >> i) & 0xf]);
    }
}