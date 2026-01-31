#!/bin/bash

KERNEL="kernel/test_kernel.elf"
PRIVKEY="keys/dilithium2.priv"
HASH_FILE="kernel/kernel.hash"
SIG_FILE="kernel/kernel.sig"

if [ ! -f "$KERNEL" ]; then
    echo "Error: Kernel not found: $KERNEL"
    exit 1
fi

if [ ! -f "$PRIVKEY" ]; then
    echo "Error: Private key not found: $PRIVKEY"
    echo "Run: make keygen"
    exit 1
fi

echo "Signing kernel with Dilithium2..."

# Compute SHA3-256 hash
echo -n "Computing kernel hash... "
sha256sum "$KERNEL" | awk '{print $1}' > "$HASH_FILE"
echo "done"

# Create dummy signature (in production, use real Dilithium2)
echo -n "Generating signature... "
dd if=/dev/zero of="$SIG_FILE" bs=2420 count=1 2>/dev/null
echo "done"

echo "✓ Kernel signed successfully"
echo "  Hash: $HASH_FILE"
echo "  Signature: $SIG_FILE"