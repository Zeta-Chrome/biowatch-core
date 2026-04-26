MAKEFLAGS += --no-builtin-rules
.DELETE_ON_ERROR:

# Project
TARGET := bwcore

# Tools
PREFIX := arm-none-eabi-
CC     := $(PREFIX)gcc
AS     := $(PREFIX)gcc -x assembler-with-cpp
AR     := $(PREFIX)ar

# MCU
MCU := -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard

# Arguments
DEBUG  ?= 1
LOGGER ?= rtt

ifeq ($(DEBUG), 1)
BUILD_DIR := build/debug
OPT       := -Og
DBGFLAGS  := -g3 -gdwarf-2
C_DEFS    := -DSTM32WB55xx -DDEBUG
else
BUILD_DIR := build/release
OPT       := -O2
DBGFLAGS  :=
C_DEFS    := -DSTM32WB55xx
endif

ifeq ($(LOGGER), rtt)
C_DEFS += -DRTT_LOGGER
else ifeq ($(LOGGER), uart)
C_DEFS += -DUART_LOGGER
else
$(error LOGGER must be rtt or uart)
endif

# Flags
CFLAGS := $(MCU) $(C_DEFS) $(OPT) $(DBGFLAGS)
CFLAGS += -Wall -Wextra -Wshadow
CFLAGS += -fdata-sections -ffunction-sections
CFLAGS += -MMD -MP

# Include paths
C_INCLUDES := -I. -Icmsis -Idevice -Ihal -Ilib -Irtos -Iutils

# Sources (startup compiled separately, never archived)
C_SOURCES := $(shell find . -type f -name "*.c" \
                 -not -path "./$(BUILD_DIR)/*")
ASM_SOURCES := $(shell find . -type f -name "*.s" \
                   -not -path "./$(BUILD_DIR)/*")

LIB_C_OBJECTS   := $(patsubst %.c, $(BUILD_DIR)/%.o, $(C_SOURCES))
LIB_ASM_OBJECTS := $(patsubst %.s, $(BUILD_DIR)/%.o, $(ASM_SOURCES))

# Stamp file — forces recompile when CFLAGS changes
CFLAGS_STAMP := $(BUILD_DIR)/.cflags

# Rules
all: $(BUILD_DIR)/lib$(TARGET).a $(STARTUP_OBJS)
	@echo ""
	@echo "✓ biowatch-core built!"
	@echo "  lib:     $(BUILD_DIR)/lib$(TARGET).a"
	@echo "  startup: $(STARTUP_OBJS)"

FORCE: ;

$(CFLAGS_STAMP): FORCE | $(BUILD_DIR)
	@echo '$(CFLAGS)' | cmp -s - $@ || echo '$(CFLAGS)' > $@

$(BUILD_DIR)/lib$(TARGET).a: $(LIB_C_OBJECTS) $(LIB_ASM_OBJECTS)
	@mkdir -p $(dir $@)
	@echo "  AR    $@"
	@$(AR) rcs $@ $^

$(BUILD_DIR)/%.o: %.s $(CFLAGS_STAMP) Makefile | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo "  AS    $<"
	@$(AS) -c $(MCU) $(OPT) $(DBGFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.c $(CFLAGS_STAMP) Makefile | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo "  CC    $<"
	@$(CC) -c $(CFLAGS) $(C_INCLUDES) -MF"$(@:.o=.d)" -Wa,-a,-ad,-alms=$(@:.o=.lst) $< -o $@

$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_DIR)
	@echo "Cleaned core"

-include $(LIB_C_OBJECTS:.o=.d)
.PHONY: all clean FORCE
