# Set the cross-compiler prefix. We use arm-none-eabi because its our original OS
CROSS = arm-none-eabi-
CC = $(CROSS)gcc
LD = $(CROSS)ld
OBJCOPY = $(CROSS)objcopy

# Compiler flags:
# -nostdlib and -ffreestanding: No standard library; freestanding environment.
# -marm and -mcpu=cortex-a8: Target ARM Cortex-A8.
# -nostartfiles: Do not use the standard startup files.
CFLAGS = -nostdlib -ffreestanding -Wall -O2 -marm -mcpu=cortex-a8 -nostartfiles
LDFLAGS = -T linker.ld

all: kernel.img

kernel.elf: kernel.o
	$(LD) $(LDFLAGS) -o $@ kernel.o

kernel.o: kernel.c memory.h
	$(CC) $(CFLAGS) -c kernel.c -o kernel.o

kernel.img: kernel.elf
	$(OBJCOPY) -O binary kernel.elf kernel.img

clean:
	rm -f *.o kernel.elf kernel.img
