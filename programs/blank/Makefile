ASM := nasm

SRC := $(wildcard src/*.asm)
OUT := $(patsubst src/%.asm, ../bin/blank/%.asm.obj, $(SRC))

.PHONY:
.SILENT: 

all: $(OUT)
	i386-elf-gcc -g -T ../linker.ld -o ../bin/blank.elf -ffreestanding -nostdlib -O0 -fpic $<
	

../bin/blank/%.asm.obj : src/%.asm
	mkdir -p $(dir $@)
	echo "Blank : Compiling ASM translation '$(patsubst $(SRC_DIR)/kernel/%.asm,%.asm, $<)', generating object '$@'..."
	$(ASM) -f elf32 -g $< -o $@
