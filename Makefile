PREFIX = arm-none-eabi-
CFLAGS = -c -fno-stack-protector -fomit-frame-pointer -march=armv7-a

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

$(BUILD_DIR)boot.o : boot.c memory_map.h | $(BUILD_DIR)
	$(PREFIX)gcc $(CFLAGS) boot.c -o $@

$(BIN_DIR)boot.out : boot.ld $(BUILD_DIR)boot.o | $(BIN_DIR)
	$(PREFIX)ld -T $^ -o $@

MLO : $(BIN_DIR)boot.out
	$(PREFIX)objcopy -S -O binary $< $@

Team00.img: MLO
	bash create_header.sh
	dd if=MLO of=Team00.img iflag=fullblock conv=sync seek=1 status=none
	echo 'label: dos' | /sbin/sfdisk Team00.img

objdump: MLO
	$(PREFIX)objdump -D -b binary -m arm $<
