ASM := nasm

SRC_DIR    := src
BUILD_DIR  := bin/build
DEPS_DIR   := $(BUILD_DIR)/deps
OUTPUT_DIR := bin/target/i386

BOOTLOADER_SRC := $(SRC_DIR)/boot/boot.asm
BOOTLOADER_OUT := $(BUILD_DIR)/boot.bin

KERNEL_SRC_ASM := $(wildcard $(SRC_DIR)/kernel/*.asm) $(wildcard $(SRC_DIR)/kernel/**/*.asm)
KERNEL_SRC_C   := $(wildcard $(SRC_DIR)/kernel/*.c) $(wildcard $(SRC_DIR)/kernel/**/*.c)
KERNEL_OUT_ASM := $(patsubst $(SRC_DIR)/kernel/%.asm, $(BUILD_DIR)/%.asm.obj, $(KERNEL_SRC_ASM))
KERNEL_OUT_C   := $(patsubst $(SRC_DIR)/kernel/%.c, $(BUILD_DIR)/%.c.obj, $(KERNEL_SRC_C))

CC         := i386-elf-gcc
INCLUDES   := -Isrc/kernel/include/
KERN_FLAGS := -g -ffreestanding -falign-jumps -falign-labels -falign-loops -falign-functions -fomit-frame-pointer -finline-functions -fno-builtin -Wcpp -Wunused -Werror -Wall -Wextra -nostdlib -nostartfiles -nodefaultlibs -O0
COMP_FALGS := $(KERN_FLAGS) -MMD -MF

.PHONY: 
.SILENT:

all: create bootloader kernel
	echo "Linking the bootloader, kernel and creating the final bootable OS..."
	dd if=$(BUILD_DIR)/boot.bin > $(OUTPUT_DIR)/os.img status=none
	dd if=$(BUILD_DIR)/kernel.bin >> $(OUTPUT_DIR)/os.img status=none

	# Fill it with empty sectors. Here I will store my kernel. Loading the kernel from the FAT formatted
	# disk is hard work.

	dd if=/dev/zero bs=512 count=200 >> $(OUTPUT_DIR)/os.img status=none
	truncate -s 100K $(OUTPUT_DIR)/os.img

	# Now this portion is the FAT16 tabel entries. Add 4 magic bytes to define FAT16 table start.
	# I'm using 2 FAT tables. So the method has to be done twice.

	# First FAT table.

	echo -n -e '\xf8\xff\xff\xff' >> $(OUTPUT_DIR)/os.img
	dd if=/dev/zero count=508 bs=1 >> $(OUTPUT_DIR)/os.img status=none
	dd if=/dev/zero count=255 bs=512 >> $(OUTPUT_DIR)/os.img status=none

	# Second FAT table.

	echo -n -e '\xf8\xff\xff\xff' >> $(OUTPUT_DIR)/os.img
	dd if=/dev/zero count=508 bs=1 >> $(OUTPUT_DIR)/os.img status=none
	dd if=/dev/zero count=255 bs=512 >> $(OUTPUT_DIR)/os.img status=none

	dd if=/dev/zero bs=1M count=256 >> $(OUTPUT_DIR)/os.img status=none
	truncate -s 256M $(OUTPUT_DIR)/os.img

	echo "Successfully created $(OUTPUT_DIR)/os.img"

bootloader: 
	echo "Compiling the bootloader..."
	echo "Creating $(BOOTLOADER_OUT)..."
	$(ASM) -f bin $(BOOTLOADER_SRC) -o $(BOOTLOADER_OUT)

# Create a full kernel object binary by resolving all the linker symbols.

kernel: $(KERNEL_OUT_ASM) $(KERNEL_OUT_C)
	echo "Linking kernel object files..."
	i386-elf-ld -g -r $^ -o $(BUILD_DIR)/kernelfull.obj
	echo "Extracting raw binary from full kernel object..."
	i386-elf-gcc -T $(SRC_DIR)/linker.ld $(KERN_FLAGS) $(BUILD_DIR)/kernelfull.obj -o $(BUILD_DIR)/kernel.bin

$(BUILD_DIR)/%.asm.obj: $(SRC_DIR)/kernel/%.asm
	mkdir -p $(dir $@)
	echo "Compiling ASM translation '$(patsubst $(SRC_DIR)/kernel/%.asm,%.asm, $<)', generating object '$@'..."
	$(ASM) -f elf32 -g $< -o $@

$(BUILD_DIR)/%.c.obj: $(SRC_DIR)/kernel/%.c
	mkdir -p $(dir $@)
	echo "Compiling C translation '$(patsubst $(SRC_DIR)/kernel/%.c,%.c, $<)', generating object '$@'..."
	$(CC) -c $(INCLUDES) $(COMP_FALGS) $(patsubst %.c.obj, $(DEPS_DIR)/%.d, $(notdir $@)) $< -o $@

create: 
	mkdir -p bin/{build/deps,target/i386}

clean: 
	rm -rf bin

# Inlcude all the header file dependencies.

-include $(wildcard $(DEPS_DIR)/*.d)
