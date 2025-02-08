# Toolchain
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy
CFLAGS = -nostdlib -ffreestanding -Wall -O2 -marm -mcpu=cortex-a8 -I../include/
LDFLAGS = -T linker.ld

# Source files
SRC = kernel.c
OBJ = $(SRC:.c=.o)

# Output binary name
OUTPUT = kernel.elf
BIN = kernel.bin

# Build Rules
all: $(BIN)

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUTPUT): $(OBJ)
	$(LD) $(LDFLAGS) $^ -o $@

$(BIN): $(OUTPUT)
	$(OBJCOPY) -O binary $< $@

# Clean the build
clean:
	rm -f $(OBJ) $(OUTPUT) $(BIN)

