#!/bin/bash
set -e

echo "========================================="
echo " Building Quantum-Safe RISC-V Bootloader"
echo "========================================="

# Check toolchain
if ! command -v riscv64-unknown-elf-gcc &> /dev/null; then
    echo "Error: RISC-V toolchain not found"
    echo "Install: https://github.com/riscv-collab/riscv-gnu-toolchain"
    exit 1
fi

# Build bootloader
echo -e "\n[1/4] Building bootloader..."
make clean
make -j$(nproc)

# Generate keys
echo -e "\n[2/4] Generating Dilithium2 keys..."
make keygen

# Create test kernel if not exists
if [ ! -f kernel/test_kernel.elf ]; then
    echo -e "\n[3/4] Creating test kernel..."
    mkdir -p kernel
    echo "Test kernel placeholder" > kernel/test_kernel.elf
fi

# Sign kernel
echo -e "\n[4/4] Signing kernel..."
make sign-kernel

echo -e "\n✓ Build complete!"
echo "Run with: make run"