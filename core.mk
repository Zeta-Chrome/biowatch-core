# Core directory
CORE_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

# Chip identity
CORE_DEFS := -DSTM32WB55xx

# Include paths
CORE_INCS := \
	$(CORE_DIR) \
	$(CORE_DIR)/arch/cortex_m4 \
	$(CORE_DIR)/arch/cortex_m4/cmsis/core \
	$(CORE_DIR)/arch/cortex_m4/cmsis/dsp \
	$(CORE_DIR)/soc/stm32wb55

# Sources
CORE_C_SRCS := $(shell find $(CORE_DIR) -type f -name "*.c")
CORE_ASM_SRCS := $(shell find $(CORE_DIR) -type f -name "*.s")

# Prebuilt libraries
CORE_LIBS := $(shell find $(CORE_DIR) -type f -name "*.a")

# Binaries
CORE_BIN_DIR := $(CORE_DIR)/subsys/ble/fw
CORE_BLE_STACK_BIN := $(CORE_BIN_DIR)/stm32wb5x_BLE_Stack_full_fw.bin
CORE_FUS_BIN        := $(CORE_BIN_DIR)/stm32wb5x_FUS_fw.bin

# Linker script
CORE_LDSCRIPT := $(CORE_DIR)/soc/stm32wb55/startup/stm32wb55xx_flash_cm4.ld
