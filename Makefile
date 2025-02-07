PREFIX = arm-none-eabi-
CFLAGS = -c -fno-stack-protector -fomit-frame-pointer -march=armv7-a -O0

BUILD_DIR = build/
BIN_DIR = bin/

OUTPUT = BuddyOS.img

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

$(BUILD_DIR)boot.o : boot.c memory_map.h led.h | $(BUILD_DIR)
	$(PREFIX)gcc $(CFLAGS) boot.c -o $@

$(BUILD_DIR)led.o : led.c memory_map.h led.h | $(BUILD_DIR)
	$(PREFIX)gcc $(CFLAGS) led.c -o $@

$(BIN_DIR)boot.out : boot.ld $(BUILD_DIR)boot.o $(BUILD_DIR)led.o $(BUILD_DIR)init.o | $(BIN_DIR)
	$(PREFIX)ld -flto -T $^ -o $@

MLO : $(BIN_DIR)boot.out
	$(PREFIX)objcopy -S -O binary $< $@

BuddyOS.img: MLO
	dd if=/dev/zero of=BuddyOS.img bs=512 count=2880
	mkfs.fat -F 12 BuddyOS.img
	mcopy -i BuddyOS.img MLO "::MLO"
		

objdump: BuddyOS.img
	$(PREFIX)objdump -D -b binary -m arm MLO

verify: BuddyOS.img
	mdir -i BuddyOS.img ::
