ASM := nasm
CC  := i386-elf-gcc

FLAGS      := -g -ffreestanding -falign-jumps -falign-labels -falign-loops -falign-functions -fomit-frame-pointer -finline-functions -fno-builtin -Wcpp -Wunused -Werror -Wall -Wextra -nostdlib -nostartfiles -nodefaultlibs -O0
COMP_FALGS := $(FLAGS) -MMD -MF

SRC_DIR    := src
OUTPUT_DIR := ../bin/shell
DEPS_DIR   := ../bin/deps

SRC_FILES    := $(wildcard $(SRC_DIR)/*.c)
OBJECT_FILES := $(patsubst $(SRC_DIR)/%.c, $(OUTPUT_DIR)/%.c.obj, $(SRC_FILES))

INCLUDES := -I../stdlib/include/

.PHONY:
.SILENT: 

all: $(OBJECT_FILES)
	i386-elf-gcc $(FLAGS) -T ../linker.ld $< ../bin/_start.elf.link ../bin/stdlib.elf.link -o ../bin/shell.elf

$(OUTPUT_DIR)/%.c.obj : $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	echo "SHELL : Compiling C translation '$(notdir $<)', generating object '$@'..."
	$(CC) -c $(INCLUDES) $(COMP_FALGS) $(patsubst %.c.obj, $(DEPS_DIR)/%.d, $(notdir $@)) $< -o $@

-include $(wildcard $(DEPS_DIR)/*.d)