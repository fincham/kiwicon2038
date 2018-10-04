# I don't like Makefiles.

build/chip.rom: $(shell find src)
	gcc -std=gnu99 -Os -nostdlib -m32 -march=i386 -ffreestanding -fno-pie -Wl,--nmagic,--script=com.ld -o build/ai.com src/ai.c
	gcc -DROM -std=gnu99 -Os -nostdlib -m32 -march=i386 -ffreestanding -fno-pie -Wl,--nmagic,--script=rom.ld -o build/ai.rom src/ai.c
	
	# this ROM fits in 16 blocks (8k) at the moment
	./romify.py 16 < build/ai.rom > build/padded.rom

	# for the hacked up 128k ROM used for Kiwicon 2038 the 8k padded ROM needs to be pushed up to 32k inside the "chip image"
	dd bs=512 count=192 if=/dev/zero > build/chip.rom
	cat build/padded.rom >> build/chip.rom
	dd bs=512 count=48 if=/dev/zero >> build/chip.rom

clean: 
	find build -type f -delete

emu: build/chip.rom
	bochs -q -f test/bochsrc -rc test/bochscommands

.PHONY: clean emu
