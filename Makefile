PREFIX = arm-none-eabi-
CFLAGS = -c -fno-stack-protector -fomit-frame-pointer -march=armv7-a -O0 -I./include\
				 -I./include/arch -I./include/misc -I./include/memory -I./include/drivers -I./include/kernel 
KCFLAGS = -c -fno-stack-protector -fomit-frame-pointer -march=armv7-a -mno-unaligned-access -O0 -I./include\
					-I./include/arch -I./include/misc -I./include/memory -I./include/drivers -I./include/kernel

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

$(BUILD_DIR)vector_table.o : bootloader/interrupt/vector_table.S | $(BUILD_DIR)
	$(PREFIX)as bootloader/interrupt/vector_table.S -o $@

$(BUILD_DIR)init.o : bootloader/init.S | $(BUILD_DIR)
	$(PREFIX)as bootloader/init.S -o $@

$(BUILD_DIR)main.o : bootloader/main.c | $(BUILD_DIR)
	$(PREFIX)gcc $(CFLAGS) bootloader/main.c -o $@

$(BUILD_DIR)drivers.o : drivers/mmc.c | $(BUILD_DIR)
	$(PREFIX)gcc $(CFLAGS) drivers/mmc.c -o $@

$(BUILD_DIR)led.o : arch/peripherals/led.c | $(BUILD_DIR)
	$(PREFIX)gcc $(CFLAGS) arch/peripherals/led.c -o $@

$(BUILD_DIR)uart.o : arch/peripherals/uart.c | $(BUILD_DIR)
	$(PREFIX)gcc $(CFLAGS) arch/peripherals/uart.c -o $@

$(BUILD_DIR)clock.o : arch/boardinit/clock.c | $(BUILD_DIR)
	$(PREFIX)gcc $(CFLAGS) arch/boardinit/clock.c -o $@

$(BUILD_DIR)interrupt.o : bootloader/interrupt/interrupts.c | $(BUILD_DIR)
	$(PREFIX)gcc $(CFLAGS) bootloader/interrupt/interrupts.c -o $@

$(BUILD_DIR)timer.o : arch/peripherals/timer.c | $(BUILD_DIR)
	$(PREFIX)gcc $(CFLAGS) arch/peripherals/timer.c -o $@

$(BUILD_DIR)memcmd.o : misc/memcmd.c | $(BUILD_DIR)
	$(PREFIX)gcc $(CFLAGS) misc/memcmd.c -o $@

$(BUILD_DIR)ddr.o : arch/boardinit/ddr.c | $(BUILD_DIR)
	$(PREFIX)gcc $(CFLAGS) arch/boardinit/ddr.c -o $@

$(BUILD_DIR)fat12.o : fs/fat12.c | $(BUILD_DIR)
	$(PREFIX)gcc $(CFLAGS) fs/fat12.c -o $@

$(BIN_DIR)boot.out : boot.ld $(BUILD_DIR)main.o $(BUILD_DIR)led.o $(BUILD_DIR)init.o $(BUILD_DIR)vector_table.o $(BUILD_DIR)interrupt.o $(BUILD_DIR)uart.o $(BUILD_DIR)timer.o $(BUILD_DIR)clock.o $(BUILD_DIR)ddr.o $(BUILD_DIR)memcmd.o $(BUILD_DIR)drivers.o $(BUILD_DIR)fat12.o | $(BIN_DIR)
	$(PREFIX)gcc -nostartfiles -flto=all -T $^ -o $@

MLO : $(BIN_DIR)boot.out
	$(PREFIX)objcopy -S -O binary $< $@

$(BUILD_DIR)kinit.o : kernel/kinit.S | $(BUILD_DIR)
	$(PREFIX)as kernel/kinit.S -o $@

$(BUILD_DIR)k_vector.o : kernel/k_vector.S | $(BUILD_DIR)
	$(PREFIX)as kernel/k_vector.S -o $@

$(BUILD_DIR)context_switch.o : kernel/context_switch.S | $(BUILD_DIR)
	$(PREFIX)as kernel/context_switch.S -o $@

$(BUILD_DIR)k_intr.o : kernel/k_intr.c | $(BUILD_DIR)
	$(PREFIX)gcc $(KCFLAGS) kernel/k_intr.c -o $@

$(BUILD_DIR)dispatcher.o: kernel/dispatcher.c
	$(PREFIX)gcc $(KCFLAGS) kernel/dispatcher.c -o $@

$(BUILD_DIR)srr_ipc.o: kernel/srr_ipc.c
	$(PREFIX)gcc $(KCFLAGS) kernel/srr_ipc.c -o $@

$(BUILD_DIR)fs.o: fs/fs.c
	$(PREFIX)gcc $(CFLAGS) fs/fs.c -o $@

$(BUILD_DIR)vfs.o: $(BUILD_DIR)fs.o fs/vfs.c 
	$(PREFIX)gcc $(CFLAGS) fs/vfs.c -o $@

$(BUILD_DIR)memory.o: kernel/memory.c
	$(PREFIX)gcc $(KCFLAGS) kernel/memory.c -o $@

$(BUILD_DIR)string.o: misc/string.c
	$(PREFIX)gcc $(CFLAGS) misc/string.c -o $@

$(BUILD_DIR)net.o: drivers/net.c
	$(PREFIX)gcc $(CFLAGS) drivers/net.c -o $@

$(BUILD_DIR)cpsw.o: drivers/cpsw.c
	$(PREFIX)gcc $(KCFLAGS) drivers/cpsw.c -o $@

$(BUILD_DIR)phy.o: drivers/phy.c
	$(PREFIX)gcc $(KCFLAGS) drivers/phy.c -o $@

$(BUILD_DIR)ethernet.o: net/ethernet.c
	$(PREFIX)gcc $(KCFLAGS) net/ethernet.c -o $@

$(BUILD_DIR)arp.o: net/arp.c
	$(PREFIX)gcc $(KCFLAGS) net/arp.c -o $@

$(BUILD_DIR)ipv4.o: net/ipv4.c
	$(PREFIX)gcc $(KCFLAGS) net/ipv4.c -o $@

$(BUILD_DIR)icmp.o: net/icmp.c
	$(PREFIX)gcc $(KCFLAGS) net/icmp.c -o $@

$(BUILD_DIR)udp.o: net/udp.c
	$(PREFIX)gcc $(KCFLAGS) net/udp.c -o $@

$(BUILD_DIR)socket.o: net/socket.c
	$(PREFIX)gcc $(KCFLAGS) net/socket.c -o $@

$(BUILD_DIR)net_functions.o: misc/net_functions.c
	$(PREFIX)gcc $(KCFLAGS) misc/net_functions.c -o $@

$(BUILD_DIR)kernel.o: kernel/main.c
	$(PREFIX)gcc $(KCFLAGS) kernel/main.c -o $@

kernel.elf: kernel.ld $(BUILD_DIR)kernel.o $(BUILD_DIR)kinit.o\
$(BUILD_DIR)led.o $(BUILD_DIR)uart.o $(BUILD_DIR)memory.o\
$(BUILD_DIR)k_intr.o $(BUILD_DIR)k_vector.o $(BUILD_DIR)net.o\
$(BUILD_DIR)dispatcher.o $(BUILD_DIR)memcmd.o\
$(BUILD_DIR)drivers.o $(BUILD_DIR)fat12.o $(BUILD_DIR)fs.o $(BUILD_DIR)vfs.o\
$(BUILD_DIR)srr_ipc.o $(BUILD_DIR)context_switch.o\
$(BUILD_DIR)cpsw.o $(BUILD_DIR)phy.o $(BUILD_DIR)ethernet.o $(BUILD_DIR)arp.o\
$(BUILD_DIR)ipv4.o $(BUILD_DIR)icmp.o $(BUILD_DIR)udp.o $(BUILD_DIR)socket.o $(BUILD_DIR)net_functions.o
	$(PREFIX)gcc -nostartfiles -flto=all -T $^ -o $@

kernel.bin: kernel.elf
	$(PREFIX)objcopy -O binary kernel.elf kernel.bin

BuddyOS.img: MLO kernel.bin
	dd if=/dev/zero of=BuddyOS.img bs=512 count=2880
	mkfs.fat -F 12 BuddyOS.img
	mcopy -i BuddyOS.img MLO "::MLO"
	mcopy -i BuddyOS.img kernel.bin "::kernel.bin"

objdump: BuddyOS.img
	$(PREFIX)objdump -D -b binary -m arm MLO

verify: BuddyOS.img
	mdir -i BuddyOS.img ::
