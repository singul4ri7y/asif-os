ASM := nasm
CC  := i386-elf-gcc

FLAGS      := -g -ffreestanding -falign-jumps -falign-labels -falign-loops -falign-functions -fomit-frame-pointer -finline-functions -fno-builtin -Wcpp -Wunused -Werror -Wall -Wextra -nostdlib -nostartfiles -nodefaultlibs -O0
COMP_FALGS := $(KERN_FLAGS) -MMD -MF

SRC_DIR    := src
OUTPUT_DIR := ../bin/blank
DEPS_DIR   := ../bin/deps

SRC_FILES    := $(wildcard $(SRC_DIR)/*.c)
OBJECT_FILES := $(patsubst $(SRC_DIR)/%.c, $(OUTPUT_DIR)/%.c.obj, $(SRC_FILES))

INCLUDES := 

.PHONY:
.SILENT: 

all: $(OBJECT_FILES)
	i386-elf-gcc -T ../linker.ld -o $(FLAGS) $< ../bin/_start.elf.link ../bin/stdlib.elf.link -o ../bin/blank.elf

$(OUTPUT_DIR)/%.c.obj : $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	echo "BLANK : Compiling C translation '$(notdir $<)', generating object '$@'..."
	
	$(CC) -c $(INCLUDES) $(COMP_FALGS) $(patsubst %.c.obj, $(DEPS_DIR)/%.d, $(notdir $@)) $< -o $@

-include $(wildcard $(DEPS_DIR)/*.d)