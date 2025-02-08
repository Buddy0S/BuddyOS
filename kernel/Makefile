# Set the cross-compiler prefix
CROSS = arm-none-eabi-
CC = $(CROSS)gcc
LD = $(CROSS)ld
OBJCOPY = $(CROSS)objcopy

# Compiler flags:
CFLAGS = -nostdlib -ffreestanding -Wall -O2 -marm -mcpu=cortex-a8 -nostartfiles -Iinclude/memory/

LDFLAGS = -T linker.ld

all: kernel.img

kernel.elf: kernel.o
	$(LD) $(LDFLAGS) -o $@ kernel.o

kernel.o: kernel.c include/memory/memory.h
	$(CC) $(CFLAGS) -c kernel.c -o kernel.o

kernel.img: kernel.elf
	$(OBJCOPY) -O binary kernel.elf kernel.img

clean:
	rm -f *.o kernel.elf kernel.img

