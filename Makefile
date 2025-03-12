# Set the kernel directory as a variable
KERNEL_DIR = kernel
INCLUDES = include

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
CFLAGS = -nostdlib -ffreestanding -Wall -Wextra -O2
LDFLAGS = -T kernel/linker.ld -nostdlib

# Directories for kernel source and include
KERNEL_SRC = $(KERNEL_DIR)
KERNEL_INC = $(INCLUDES)

# Include the kernel's headers from the include folder
CFLAGS += -I $(KERNEL_INC)

# Targets
all: $(QEMU_KERN) kernel.bin

$(QEMU_KERN): $(KERNEL_SRC)/start.o $(KERNEL_SRC)/kernel.o $(KERNEL_SRC)/uart.o $(KERNEL_SRC)/context_switch.o $(KERNEL_SRC)/process1.o $(KERNEL_SRC)/process2.o $(KERNEL_SRC)/process3.o $(KERNEL_SRC)/memory.o $(KERNEL_SRC)/proc.o
	$(LD) $(LDFLAGS) -o $@ $^

kernel.bin: $(QEMU_KERN)
	$(OBJCOPY) -O binary $< $@

$(KERNEL_SRC)/%.o: $(KERNEL_SRC)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(KERNEL_SRC)/%.o: $(KERNEL_SRC)/%.s
	$(CC) -c -o $@ $<

$(FS_IMG):
	qemu-img create -f raw $(FS_IMG) $(QEMU_FS_SIZE)

qemu: $(QEMU_KERN) $(FS_IMG)
	$(QEMU) $(QEMU_OPTS) -kernel $(QEMU_KERN)

qemu-gdb: $(QEMU_KERN) $(FS_IMG)
	@echo "*****Starting QEMU for debugging*****"
	@echo "*** Open another terminal and run 'make gdb-client' ***"
	$(QEMU) $(QEMU_OPTS) -kernel $(QEMU_KERN) -S -gdb tcp::9000

gdb-client:
	@echo "*****Starting GDB Remote Debugger*****"
	@echo "*** Enter 'target remote tcp::9000' in GDB ***"
	$(CROSS_COMPILE)gdb $(QEMU_KERN)

clean:
	rm -f $(KERNEL_SRC)/*.o $(QEMU_KERN) kernel.bin $(FS_IMG)

