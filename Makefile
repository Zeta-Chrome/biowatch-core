MAKEFLAGS += --no-builtin-rules
.DELETE_ON_ERROR:

TARGET := bwcore

PREFIX := arm-none-eabi-
CC     := $(PREFIX)gcc
AS     := $(PREFIX)gcc -x assembler-with-cpp
AR     := $(PREFIX)ar

CPU       = -mcpu=cortex-m4
FPU       = -mfpu=fpv4-sp-d16
FLOAT_ABI = -mfloat-abi=hard
MCU       = $(CPU) -mthumb $(FPU) $(FLOAT_ABI)

DEBUG ?= 1
C_DEFS = -DSTM32WB55xx
ifeq ($(DEBUG), 1)
BUILD_DIR := build/debug
C_DEFS  += -DDEBUG
OPT      = -Og
DBGFLAGS = -g3 -gdwarf-2
else
BUILD_DIR := build/release
OPT      = -O2
DBGFLAGS =
endif

CFLAGS  = $(MCU) $(C_DEFS) $(OPT) $(DBGFLAGS)
CFLAGS += -Wall -Wextra -Wshadow
CFLAGS += -fdata-sections -ffunction-sections
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

C_INCLUDES := -I. -Icmsis -Idevice -Ihal -Iutils

C_SOURCES := $(shell find . -type f -name "*.c" \
                 -not -path "./startup/*" \
                 -not -path "./$(BUILD_DIR)/*")

STARTUP_C   := startup/vectors.c
STARTUP_ASM := startup/startup_stm32wb55xx_cm4.s

LIB_OBJECTS     := $(patsubst %.c, $(BUILD_DIR)/%.o, $(C_SOURCES))
STARTUP_OBJ_C   := $(BUILD_DIR)/startup/vectors.o
STARTUP_OBJ_ASM := $(BUILD_DIR)/startup/startup.o

all: $(BUILD_DIR)/lib$(TARGET).a $(STARTUP_OBJ_C) $(STARTUP_OBJ_ASM)
	@echo ""
	@echo "✓ biowatch-core built!"
	@echo "  lib:     $(BUILD_DIR)/lib$(TARGET).a"
	@echo "  startup: $(BUILD_DIR)/startup/"

$(BUILD_DIR)/lib$(TARGET).a: $(LIB_OBJECTS)
	@mkdir -p $(dir $@)
	@echo "  AR    $@"
	@$(AR) rcs $@ $^

$(STARTUP_OBJ_C): $(STARTUP_C) Makefile
	@mkdir -p $(dir $@)
	@echo "  CC    $<"
	@$(CC) -c $(CFLAGS) $(C_INCLUDES) $< -o $@

$(STARTUP_OBJ_ASM): $(STARTUP_ASM) Makefile
	@mkdir -p $(dir $@)
	@echo "  AS    $<"
	@$(AS) -c $(MCU) $(OPT) $(DBGFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo "  CC    $<"
	@$(CC) -c $(CFLAGS) $(C_INCLUDES) $< -o $@

$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_DIR)
	@echo "Clean complete!"

clean_all:
	@rm -rf build/
	@echo "Clean all complete!"

compiledb:
	@bear -- $(MAKE) DEBUG=1
	@echo "✓ compile_commands.json updated"

-include $(LIB_OBJECTS:.o=.d)
.PHONY: all clean clean_all compiledb
