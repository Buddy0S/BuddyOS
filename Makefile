PREFIX = arm-none-eabi-
CFLAGS = -c -fno-stack-protector -fomit-frame-pointer -march=armv7-a -O0 -I./include

BUILD_DIR = build/
BIN_DIR = bin/
INCLUDE = include/

OUTPUT = BuddyOS.img

.PHONY: all clean objdump

all: $(OUTPUT)

clean:
	rm -rf $(OUTPUT) MLO header.bin build/ bin/

$(BUILD_DIR) :
	mkdir -p $(BUILD_DIR)

$(BIN_DIR) :
	mkdir -p $(BIN_DIR)

$(BUILD_DIR)vector_table.o : interrupt/vector_table.S | $(BUILD_DIR)
	$(PREFIX)as interrupt/vector_table.S -o $@

$(BUILD_DIR)init.o : bootloader/init.S | $(BUILD_DIR)
	$(PREFIX)as bootloader/init.S -o $@


$(BUILD_DIR)main.o : bootloader/main.c $(INCLUDE)memory_map.h $(INCLUDE)led.h | $(BUILD_DIR)
	$(PREFIX)gcc $(CFLAGS) bootloader/main.c -o $@

$(BUILD_DIR)led.o : peripherals/led.c $(INCLUDE)memory_map.h $(INCLUDE)led.h | $(BUILD_DIR)
	$(PREFIX)gcc $(CFLAGS) peripherals/led.c -o $@

$(BUILD_DIR)uart.o :peripherals/uart.c $(INCLUDE)memory_map.h $(INCLUDE)uart.h | $(BUILD_DIR)
	$(PREFIX)gcc $(CFLAGS) peripherals/uart.c -o $@

$(BUILD_DIR)clock.o :boardinit/clock.c $(INCLUDE)clock.h | $(BUILD_DIR)
	$(PREFIX)gcc $(CFLAGS) boardinit/clock.c -o $@

$(BUILD_DIR)interrupt.o : interrupt/interrupts.c $(INCLUDE)interrupts.h | $(BUILD_DIR)
	$(PREFIX)gcc $(CFLAGS) interrupt/interrupts.c -o $@

$(BUILD_DIR)timer.o : peripherals/timer.c $(INCLUDE)timer.h | $(BUILD_DIR)
	$(PREFIX)gcc $(CFLAGS) peripherals/timer.c -o $@

$(BUILD_DIR)memcpy.o : misc/memcpy.c | $(BUILD_DIR)
	$(PREFIX)gcc $(CFLAGS) misc/memcpy.c -o $@

$(BUILD_DIR)ddr.o : boardinit/ddr.c | $(BUILD_DIR)
	$(PREFIX)gcc $(CFLAGS) boardinit/ddr.c -o $@

$(BIN_DIR)boot.out : boot.ld $(BUILD_DIR)main.o $(BUILD_DIR)led.o $(BUILD_DIR)init.o $(BUILD_DIR)vector_table.o $(BUILD_DIR)interrupt.o $(BUILD_DIR)uart.o $(BUILD_DIR)timer.o $(BUILD_DIR)clock.o $(BUILD_DIR)ddr.o $(BUILD_DIR)memcpy.o | $(BIN_DIR)
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
