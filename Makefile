# option ROMs have a size measured in 512 byte blocks.
# this ROM almost fits in 16 blocks (8k) with -Os but I'd rather it just be longer
# and not worry about what weird stuff the compiler is doing
ROM_BLOCKS := 32
ROM_BYTES := 0x4000

GCC_OPTIONS := -DVENDOR=\"Toshiba\" -DROM_BYTES=\"$(ROM_BYTES)\" -std=gnu99 -nostdlib -m32 -march=i386 -ffreestanding -fno-pie
COM_LD_OPTIONS := --nmagic,--script=com.ld
ROM_LD_OPTIONS := --nmagic,--script=rom.ld

all: build/chip.rom build/disk.img

build/chip.rom: romify.py $(shell find -name \*.ld) $(shell find src -not -name \*.S)
	mkdir -p build
	./obfuscate.py < src/ai.c > src/ai_obfuscated.c
	gcc $(GCC_OPTIONS) -Wl,$(COM_LD_OPTIONS) src/ai_obfuscated.c -o build/ai.com
	gcc $(GCC_OPTIONS) -DROM -Wl,$(ROM_LD_OPTIONS) src/ai_obfuscated.c -o build/ai.rom

	./romify.py $(ROM_BLOCKS) < build/ai.rom > build/padded.rom

	# for the hacked up 128k ROM used for Kiwicon 2038 the 16k binary needs to be pushed up to 32k inside the "chip image"
	dd bs=512 count=192 if=/dev/zero > build/chip.rom
	cat build/padded.rom >> build/chip.rom
	# make sure the rest of the file is sufficiently padded with zeroes
	dd bs=512 count=$$((64 - $(ROM_BLOCKS)))  if=/dev/zero >> build/chip.rom

build/disk.img: build/chip.rom $(shell find src -name \*.S)
	# build the backup "disk bootable" version
	as -o build/boot.o src/boot.S
	ld -Ttext 0x7C00 -o build/boot.elf build/boot.o
	objcopy -O binary build/boot.elf build/boot.bin
	cat build/boot.bin build/ai.rom > build/disk.img
	dd if=/dev/null of=build/disk.img bs=1 count=0 seek=1440k

clean:
	find build -type f -delete
	rm -f src/ai_obfuscated.c

emu: build/chip.rom
	bochs -q -f test/bochsrc -rc test/bochscommands

diskemu: build/disk.img
	bochs -q -f test/bochsrc-disk -rc test/bochscommands

.PHONY: clean emu
