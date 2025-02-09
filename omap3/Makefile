# Toolchain
CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy
CFLAGS = -nostdlib -ffreestanding -nostartfiles -Wall -O2 -marm -mcpu=cortex-a8 -I../include/
LDFLAGS = -T linker.ld --no-dynamic-linker --build-id=none

# Source files
SRC = kernel.c entry.S
OBJ = kernel.o entry.o 

# Output binary name
OUTPUT = kernel.elf
BIN = kernel.bin

# Build Rules
all: $(BIN)

# Rule for compiling C files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule for assembling S files
%.o: %.S
	$(AS) -mcpu=cortex-a8 -o $@ $<

# Link the kernel with the object files
$(OUTPUT): $(OBJ)
	$(LD) $(LDFLAGS) $^ -o $@

# Convert ELF to binary
$(BIN): $(OUTPUT)
	$(OBJCOPY) -O binary $< $@

loopdev: $(BIN)
	@echo "DOING STUFF" 
	sudo losetup --partscan /dev/loop8 ../../sdcard.img
	sudo mount /dev/loop8p1 /mnt/beagle
	sudo cp kernel.bin /mnt/beagle
	sudo umount /mnt/beagle
	sudo losetup -d /dev/loop8
	@echo "DONE STUFF" 

# Clean the build
devclean:
	sudo losetup --partscan /dev/loop8 ../../sdcard.img
	sudo mount /dev/loop8p1 /mnt/beagle
	sudo rm /mnt/beagle/kernel.*
	sudo umount /mnt/beagle
	sudo losetup -d /dev/loop8

clean:
	rm -f $(OBJ) $(OUTPUT) $(BIN)

