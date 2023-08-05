ASM := nasm

SRC := $(wildcard src/*.asm)
OUT := $(patsubst src/%.asm, ../bin/_start/%.asm.obj, $(SRC))

.PHONY:
.SILENT: 

all: $(OUT) 
	i386-elf-ld -m elf_i386 -relocatable $< -o ../bin/_start.elf.link

../bin/_start/%.asm.obj : src/%.asm
	mkdir -p $(dir $@)
	echo "_START : Compiling ASM translation '$(notdir $<)', generating object '$@'..."
	$(ASM) -f elf32 -g $< -o $@
