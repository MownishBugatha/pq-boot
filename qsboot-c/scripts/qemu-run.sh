#!/bin/bash

QEMU=qemu-system-riscv64
BOOTLOADER="qsboot.bin"
KERNEL="kernel/test_kernel.elf"

if ! command -v $QEMU &> /dev/null; then
    echo "Error: QEMU RISC-V not found"
    echo "Install: apt-get install qemu-system-misc"
    exit 1
fi

if [ ! -f "$BOOTLOADER" ]; then
    echo "Error: Bootloader not found. Run: make"
    exit 1
fi

echo "Starting QEMU RISC-V with quantum-safe bootloader..."
echo "Press Ctrl+A then X to exit QEMU"
echo ""

$QEMU \
    -machine virt \
    -nographic \
    -bios "$BOOTLOADER" \
    -m 128M \
    -smp 1