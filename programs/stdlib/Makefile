ASM := nasm
CC  := i386-elf-gcc

FLAGS      := -g -ffreestanding -falign-jumps -falign-labels -falign-loops -falign-functions -fomit-frame-pointer -finline-functions -fno-builtin -Wcpp -Wunused -Werror -Wall -Wextra -nostdlib -nostartfiles -nodefaultlibs -O0
COMP_FALGS := $(KERN_FLAGS) -MMD -MF

SRC_DIR    := src
OUTPUT_DIR := ../bin/stdlib
DEPS_DIR   := ../bin/deps

SRC_FILES_C      := $(wildcard $(SRC_DIR)/*.c)
SRC_FILES_ASM    := $(wildcard $(SRC_DIR)/*.asm)
OBJECT_FILES_C   := $(patsubst $(SRC_DIR)/%.c, $(OUTPUT_DIR)/%.c.obj, $(SRC_FILES_C))
OBJECT_FILES_ASM := $(patsubst $(SRC_DIR)/%.asm, $(OUTPUT_DIR)/%.asm.obj, $(SRC_FILES_ASM))

INCLUDES := 

.PHONY:
.SILENT: 

all: $(OBJECT_FILES_C) $(OBJECT_FILES_ASM)
	echo $(OBJECT_FILE_ASM)
	i386-elf-ld -m elf_i386 -relocatable $^ -o ../bin/stdlib.elf.link

$(OUTPUT_DIR)/%.c.obj : $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	echo "STDLIB : Compiling C translation '$(notdir $<)', generating object '$@'..."
	$(CC) -c $(INCLUDES) $(COMP_FALGS) $(patsubst %.c.obj, $(DEPS_DIR)/%.d, $(notdir $@)) $< -o $@

$(OUTPUT_DIR)/%.asm.obj : $(SRC_DIR)/%.asm
	mkdir -p $(dir $@)
	echo "STDLIB : Compiling ASM translation '$(notdir $<)', generating object '$@'..."
	$(ASM) -f elf32 -g $< -o $@

-include $(wildcard $(DEPS_DIR)/*.d)