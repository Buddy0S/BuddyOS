PREFIX = arm-none-eabi-
CFLAGS = -c -nostdlib -nostartfiles -nodefaultlibs -fno-builtin

BUILD_DIR = build/
BIN_DIR = bin/

OUTPUT = MLO

.PHONY: all clean objdump

all: $(OUTPUT)

clean:
	rm -rf $(OUTPUT) build/ bin/

$(BUILD_DIR) :
	mkdir -p $(BUILD_DIR)

$(BIN_DIR) :
	mkdir -p $(BIN_DIR)

$(BUILD_DIR)boot.o : boot.c memory_map.h | $(BUILD_DIR)
	$(PREFIX)gcc $(CFLAGS) boot.c -o $@

$(BIN_DIR)boot.out : boot.ld $(BUILD_DIR)boot.o | $(BIN_DIR)
	$(PREFIX)ld -T $^ -o $@

MLO : $(BIN_DIR)boot.out
	$(PREFIX)objcopy -S -O binary $< $@

objdump: MLO
	$(PREFIX)objdump -D -b binary -m arm $<
