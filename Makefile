# Set the kernel directory as a variable
KERNEL_DIR = kernel

# Compiler and Tools
CROSS_COMPILE = arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy

# QEMU Settings
QEMU = qemu-system-arm
QEMU_CPUS = 1
QEMU_MEM  = 2G
QEMU_FS_SIZE = 4G
QEMU_KERN = kernel_qemu.elf
FS_IMG = fs.img

QEMU_OPTS = -m $(QEMU_MEM) -smp $(QEMU_CPUS) -nographic
QEMU_OPTS += -audio none  # Disable audio (fix for realview-pb-a8)
QEMU_OPTS += -drive file=$(FS_IMG),if=none,format=raw,id=sda0
QEMU_OPTS += -serial mon:stdio
QEMU_OPTS += -machine realview-pb-a8
QEMU_OPTS += -no-reboot

# Compilation Flags
CFLAGS = -nostdlib -ffreestanding -Wall -Wextra -O0 -g
LDFLAGS = -T kernel/linker.ld -nostdlib

# Directories for kernel source and include
KERNEL_SRC = $(KERNEL_DIR)
KERNEL_INC = include

# Include the kernel's headers
CFLAGS += -I $(KERNEL_INC)

# Targets
all: $(QEMU_KERN) kernel.bin

# Add the other object files here that the kernel needs to be compiled with
$(QEMU_KERN): $(KERNEL_SRC)/start.o $(KERNEL_SRC)/kernel.o $(KERNEL_SRC)/uart.o $(KERNEL_SRC)/mm.o
	$(LD) $(LDFLAGS) -o $@ $^

kernel.bin: $(QEMU_KERN)
	$(OBJCOPY) -O binary $< $@

# Compile .c and .s files
$(KERNEL_SRC)/%.o: $(KERNEL_SRC)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(KERNEL_SRC)/%.o: $(KERNEL_SRC)/%.s
	$(CC) -c -o $@ $<

# Filesystem image creation
$(FS_IMG):
	qemu-img create -f raw $(FS_IMG) $(QEMU_FS_SIZE)

# QEMU run target
qemu: $(QEMU_KERN) $(FS_IMG)
	$(QEMU) $(QEMU_OPTS) -kernel $(QEMU_KERN)

# QEMU with GDB debugging
qemu-gdb: $(QEMU_KERN) $(FS_IMG)
	@echo "*****Starting QEMU for debugging*****"
	@echo "*** Open another terminal and run 'make gdb-client' ***"
	$(QEMU) $(QEMU_OPTS) -kernel $(QEMU_KERN) -S -gdb tcp::9000

# GDB client setup
gdb-client:
	@echo "*****Starting GDB Remote Debugger*****"
	@echo "*** Enter 'target remote tcp::9000' in GDB ***"
	gdb $(QEMU_KERN)

# Clean up object files and other generated files
clean:
	rm -f $(KERNEL_SRC)/*.o $(QEMU_KERN) $(KERNEL_DIR)/kernel.bin $(FS_IMG)

