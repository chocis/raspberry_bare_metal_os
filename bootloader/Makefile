
ARMGNU ?= arm-none-eabi

COPS = -Wall -O2 -nostdlib -nostartfiles -ffreestanding

gcc : kernel.img blinker.bin

all : gcc clang

clean :
	rm -f *.o
	rm -f *.bin
	rm -f *.hex
	rm -f *.elf
	rm -f *.list
	rm -f *.img
	rm -f *.bc
	rm -f *.clang.s


vectors.o : vectors.s
	$(ARMGNU)-as vectors.s -o vectors.o

bootloader05.o : bootloader05.c
	$(ARMGNU)-gcc $(COPS) -c bootloader05.c -o bootloader05.o

periph.o : periph.c
	$(ARMGNU)-gcc $(COPS) -c periph.c -o periph.o

kernel.img : loader vectors.o periph.o bootloader05.o
	$(ARMGNU)-ld vectors.o periph.o bootloader05.o -T loader -o bootloader05.elf
	$(ARMGNU)-objdump -D bootloader05.elf > bootloader05.list
	$(ARMGNU)-objcopy bootloader05.elf -O ihex bootloader05.hex
	$(ARMGNU)-objcopy bootloader05.elf -O binary kernel.img





start.o : start.s
	$(ARMGNU)-as start.s -o start.o

blinker.o : blinker.c
	$(ARMGNU)-gcc $(COPS) -c blinker.c -o blinker.o

blinker.bin : memmap start.o blinker.o
	$(ARMGNU)-ld start.o blinker.o -T memmap -o blinker.elf
	$(ARMGNU)-objdump -D blinker.elf > blinker.list
	$(ARMGNU)-objcopy blinker.elf -O ihex blinker.hex
	$(ARMGNU)-objcopy blinker.elf -O binary blinker.bin
