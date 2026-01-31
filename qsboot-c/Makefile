CROSS_COMPILE ?= riscv64-unknown-elf-
CC := $(CROSS_COMPILE)gcc
AS := $(CROSS_COMPILE)as
LD := $(CROSS_COMPILE)ld
OBJCOPY := $(CROSS_COMPILE)objcopy
OBJDUMP := $(CROSS_COMPILE)objdump

CFLAGS := -march=rv64imacv -mabi=lp64d -mcmodel=medany
CFLAGS += -nostdlib -nostartfiles -fno-builtin -fno-stack-protector
CFLAGS += -Wall -Wextra -O2 -g -Iinclude -Ilib/dilithium2
CFLAGS += -mno-relax -fno-pic -fno-pie -msmall-data-limit=0

ASFLAGS := -march=rv64imacv -mabi=lp64d
LDFLAGS := -T linker.ld -nostdlib -static

BOOT_ASM := src/boot.S
C_SRCS := src/main.c src/hash.c src/sigverify.c src/pmp.c src/uart.c
DILITHIUM_SRCS := lib/dilithium2/sign.c lib/dilithium2/packing.c \
                  lib/dilithium2/polyvec.c lib/dilithium2/poly.c \
                  lib/dilithium2/ntt.c lib/dilithium2/reduce.c \
                  lib/dilithium2/rounding.c 

BOOT_OBJ := build/boot.o
C_OBJS := $(patsubst src/%.c,build/%.o,$(C_SRCS))
DILITHIUM_OBJS := $(patsubst lib/dilithium2/%.c,build/dil_%.o,$(DILITHIUM_SRCS))

PUBKEY := keys/dilithium2.pub
PRIVKEY := keys/dilithium2.priv
PUBKEY_OBJ := build/pubkey.o

ALL_OBJS := $(BOOT_OBJ) $(C_OBJS) $(DILITHIUM_OBJS) $(PUBKEY_OBJ)

BOOTLOADER := qsboot.elf
BOOTLOADER_BIN := qsboot.bin
KEYGEN := keys/keygen

.PHONY: all clean keygen sign-kernel run dirs

all: dirs $(BOOTLOADER_BIN)

dirs:
	@mkdir -p build keys kernel

$(BOOT_OBJ): $(BOOT_ASM) | dirs
	$(AS) $(ASFLAGS) -o $@ $<

build/%.o: src/%.c | dirs
	$(CC) $(CFLAGS) -c -o $@ $<

build/dil_%.o: lib/dilithium2/%.c | dirs
	$(CC) $(CFLAGS) -DDILITHIUM_MODE=2 -c -o $@ $<

# Embed public key binary into object file
$(PUBKEY_OBJ): $(PUBKEY)
	$(OBJCOPY) -I binary -O elf64-littleriscv -B riscv $< $@

$(BOOTLOADER): $(ALL_OBJS) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(filter-out linker.ld,$^)
	$(OBJDUMP) -d $@ > $(BOOTLOADER:.elf=.dump)

$(BOOTLOADER_BIN): $(BOOTLOADER)
	$(OBJCOPY) -O binary $< $@
	@ls -lh $@

$(KEYGEN): keys/keygen.c lib/dilithium2/*.c | dirs
	gcc -O2 -Ilib/dilithium2 -DDILITHIUM_MODE=2 -o $@ $< \
		lib/dilithium2/sign.c lib/dilithium2/packing.c \
		lib/dilithium2/polyvec.c lib/dilithium2/poly.c \
		lib/dilithium2/ntt.c lib/dilithium2/reduce.c \
		lib/dilithium2/rounding.c 

keygen: $(KEYGEN)
	./$(KEYGEN)

sign-kernel: keygen
	@./scripts/sign_kernel.sh

run: $(BOOTLOADER_BIN) sign-kernel
	@./scripts/qemu-run.sh

clean:
	rm -rf build $(BOOTLOADER) $(BOOTLOADER_BIN) $(BOOTLOADER:.elf=.dump)
	rm -f $(KEYGEN) $(PUBKEY) $(PRIVKEY) kernel/*.sig kernel/*.hash
