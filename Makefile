PREFIX = arm-none-eabi-
CFLAGS = -c -fno-stack-protector -fomit-frame-pointer -march=armv7-a -O0 -I./include

BUILD_DIR = build/
BIN_DIR = bin/
INCLUDE = include/

OUTPUT = BuddyOS.img

.PHONY: all clean objdump

all: $(OUTPUT)

clean:
	rm -rf $(OUTPUT) MLO kernel.bin build/ bin/ hello.txt kernel.elf

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

$(BUILD_DIR)drivers.o : drivers/mmc.c | $(BUILD_DIR)
	$(PREFIX)gcc $(CFLAGS) drivers/mmc.c -o $@

$(BUILD_DIR)led.o : peripherals/led.c $(INCLUDE)memory_map.h $(INCLUDE)led.h | $(BUILD_DIR)
	$(PREFIX)gcc $(CFLAGS) peripherals/led.c -o $@

$(BUILD_DIR)uart.o :peripherals/uart.c $(INCLUDE)memory_map.h $(INCLUDE)uart.h $(INCLUDE)args.h | $(BUILD_DIR)
	$(PREFIX)gcc $(CFLAGS) peripherals/uart.c -o $@

$(BUILD_DIR)clock.o :boardinit/clock.c $(INCLUDE)clock.h | $(BUILD_DIR)
	$(PREFIX)gcc $(CFLAGS) boardinit/clock.c -o $@

$(BUILD_DIR)interrupt.o : interrupt/interrupts.c $(INCLUDE)interrupts.h | $(BUILD_DIR)
	$(PREFIX)gcc $(CFLAGS) interrupt/interrupts.c -o $@

$(BUILD_DIR)timer.o : peripherals/timer.c $(INCLUDE)timer.h | $(BUILD_DIR)
	$(PREFIX)gcc $(CFLAGS) peripherals/timer.c -o $@

$(BUILD_DIR)memcmd.o : misc/memcmd.c | $(BUILD_DIR)
	$(PREFIX)gcc $(CFLAGS) misc/memcmd.c -o $@

$(BUILD_DIR)ddr.o : boardinit/ddr.c | $(BUILD_DIR)
	$(PREFIX)gcc $(CFLAGS) boardinit/ddr.c -o $@

$(BUILD_DIR)fat12.o : drivers/fat12.c | $(BUILD_DIR)
	$(PREFIX)gcc $(CFLAGS) drivers/fat12.c -o $@

$(BIN_DIR)boot.out : boot.ld $(BUILD_DIR)main.o $(BUILD_DIR)led.o $(BUILD_DIR)init.o $(BUILD_DIR)vector_table.o $(BUILD_DIR)interrupt.o $(BUILD_DIR)uart.o $(BUILD_DIR)timer.o $(BUILD_DIR)clock.o $(BUILD_DIR)ddr.o $(BUILD_DIR)memcmd.o $(BUILD_DIR)drivers.o $(BUILD_DIR)fat12.o | $(BIN_DIR)
	$(PREFIX)gcc -nostartfiles -flto=all -T $^ -o $@

MLO : $(BIN_DIR)boot.out
	$(PREFIX)objcopy -S -O binary $< $@

$(BUILD_DIR)kinit.o : kernel/kinit.S | $(BUILD_DIR)
	$(PREFIX)as kernel/kinit.S -o $@

$(BUILD_DIR)kernel.o: kernel/main.c
	$(PREFIX)gcc $(CFLAGS) kernel/main.c -o $@

kernel.elf: kernel.ld $(BUILD_DIR)kernel.o $(BUILD_DIR)uart.o $(BUILD_DIR)kinit.o
	$(PREFIX)gcc -nostartfiles -flto=all -T $^ -o $@

kernel.bin: kernel.elf
	$(PREFIX)objcopy -O binary kernel.elf kernel.bin

BuddyOS.img: MLO kernel.bin
	dd if=/dev/zero of=BuddyOS.img bs=512 count=2880
	mkfs.fat -F 12 BuddyOS.img
	mcopy -i BuddyOS.img MLO "::MLO"
	echo "HI FROM FAT12!" > hello.txt
	mcopy -i BuddyOS.img hello.txt "::hello.txt"
	mcopy -i BuddyOS.img kernel.bin "::kernel.bin"

objdump: BuddyOS.img
	$(PREFIX)objdump -D -b binary -m arm MLO

verify: BuddyOS.img
	mdir -i BuddyOS.img ::
