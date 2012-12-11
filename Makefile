# The toolchain to use. arm-none-eabi works, but there does exist
# arm-bcm2708-linux-gnueabi.
ARMGNU ?= arm-none-eabi
#ARMGNU ?= ~/crosscompilerrpi/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin/arm-linux-gnueabihf
# Mac Ports has arm-elf

AOPS = --warn --fatal-warnings
COPS = -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mcpu=arm1176jzf-s
INC =-Isource/headers
#ARM doesnt have division instruction, so special division functions must be implemented, which are really found in this
LIBGCC ?= lib/libgcc.a



# The intermediate directory for compiled object files.
BUILD = build/

# The directory in which source files are stored.
SOURCE = source/

# The name of the output file to generate.
TARGET = kernel.img

# The name of the assembler listing file to generate.
LIST = kernel.list

# The name of the map file to generate.
MAP = kernel.map

# The name of the linker script to use.
LINKER = kernel.ld

# The names of all object files that must be generated. Deduced from the
# assembly code files in source.
ASM_OBJECTS := $(patsubst $(SOURCE)%.s,$(BUILD)%.o,$(wildcard $(SOURCE)*.s))
C_OBJECTS   := $(patsubst $(SOURCE)%.c,$(BUILD)%.o,$(wildcard $(SOURCE)*.c))
OBJECTS     := $(ASM_OBJECTS) $(C_OBJECTS)

# Rule to make everything.
all: $(TARGET) $(LIST)

# Rule to remake everything. Does not include clean.
rebuild: all

# Rule to make the listing file.
$(LIST) : $(BUILD)output.elf
	$(ARMGNU)-objdump -d $(BUILD)output.elf > $(LIST)

# Rule to make the image file.
$(TARGET) : $(BUILD)output.elf
	$(ARMGNU)-objcopy $(BUILD)output.elf -O binary $(TARGET)

# Rule to make the elf file.
$(BUILD)output.elf : $(OBJECTS) $(LINKER)
	$(ARMGNU)-ld $(OBJECTS) -Map $(MAP) -L$(BUILD) -o $(BUILD)output.elf -T $(LINKER) $(LIBGCC)

# Rule to make the object files.
$(BUILD)%.o: $(SOURCE)%.s
	$(ARMGNU)-as -I $(SOURCE) $< -o $@

$(BUILD)%.o : $(SOURCE)%.c
	$(ARMGNU)-gcc $(COPS) $(INC) -c -o $@ $?


# Rule to clean files.
clean :
	-rm -f $(BUILD)*.o
	-rm -f $(BUILD)output.elf
	-rm -f $(TARGET)
	-rm -f $(LIST)
	-rm -f $(MAP)
