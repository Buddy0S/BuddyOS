PREFIX = arm-none-eabi-
CFLAGS = -c -fno-stack-protector -fomit-frame-pointer -march=armv7-a -O0

BUILD_DIR = build/
BIN_DIR = bin/

OUTPUT = Team00.img

.PHONY: all clean objdump

all: $(OUTPUT)

clean:
	rm -rf $(OUTPUT) MLO header.bin build/ bin/

$(BUILD_DIR) :
	mkdir -p $(BUILD_DIR)

$(BIN_DIR) :
	mkdir -p $(BIN_DIR)

$(BUILD_DIR)init.o : init.S | $(BUILD_DIR)
	$(PREFIX)as init.S -o $@

$(BUILD_DIR)boot.o : boot.c memory_map.h | $(BUILD_DIR)
	$(PREFIX)gcc $(CFLAGS) boot.c -o $@

$(BIN_DIR)boot.out : boot.ld $(BUILD_DIR)boot.o $(BUILD_DIR)init.o | $(BIN_DIR)
	$(PREFIX)ld -flto -T $^ -o $@

MLO : $(BIN_DIR)boot.out
	$(PREFIX)objcopy -S -O binary $< $@

Team00.img: MLO
	bash create_header.sh
	dd if=MLO of=Team00.img bs=512 seek=1 conv=notrunc,sync status=none	

objdump: Team00.img
	$(PREFIX)objdump -D -b binary -m arm MLO
