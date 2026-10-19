# biowatch-core

The platform layer for [BioWatch](https://github.com/Zeta-Chrome/biowatch-firmware) — a health-monitoring smartwatch built on the **STM32WB55** dual-core wireless MCU.

`biowatch-core` is consumed as a git submodule by application firmware. It contains everything that is hardware-specific but application-agnostic: a custom preemptive RTOS kernel, interrupt-driven peripheral drivers, the BLE transport/mailbox stack, and shared utilities. No STM32 HAL, LL, or CubeMX-generated code is used anywhere — every driver talks directly to the peripheral registers.

The core is compiled directly into the firmware image. `core.mk` exports source lists, include paths, defines, and the linker script, and the firmware's top-level Makefile builds everything in one pass with its own flags.

## Why a separate core

Splitting the MCU/RTOS/driver layer from the watch application means:
- The kernel and drivers can be unit-checked in isolation (`make check_core` compiles core with zero application include paths, to catch any accidental leakage of app-level dependencies into core).
- The same core can back more than one product built on the STM32WB55 without duplicating driver or RTOS code.
- Application code (`biowatch-firmware`) only has to reason about tasks, sensors, and UI — not register-level init.

## Repository layout

```
arch/cortex_m4/          Cortex-M4 CMSIS headers + prebuilt CMSIS-DSP lib (arm_math)
soc/stm32wb55/           Startup code, vector table, linker script, system clock bring-up
drivers/                 Interrupt/DMA-driven peripheral drivers
  ├─ clock/  gpio/  exti/ dma/  systick/ lptim/ rtc/ pwr/
  ├─ i2c/    i2c_bus.c        (bus-arbitration layer on top of i2c.c)
  ├─ spi/    spi_bus.c        (bus-arbitration layer on top of spi.c)
  ├─ adc/  flash/  ipcc/  hsem/  uart/
kernel/                  The RTOS: scheduler, tasks, sync primitives, software timers
  ├─ task/               TCB, context switch (task_port.s), stack/pool allocator (mem.c)
  ├─ sync/               event, mqueue, mutex (incl. recursive), semaphore
  ├─ timer.c             Software timers, tick source
  └─ critical.c          Critical-section / interrupt masking primitives
subsys/
  ├─ ble/                CPU2 IPCC/mailbox transport + auto-generated HCI/ACI command layer
  └─ lpm/                Low-power mode arbitration (sleep / lp-sleep / standby)
lib/                     assert, logger, SEGGER RTT, intrusive containers (list/clist/queue/ring)
core.mk                  Exported include paths, sources, linker script — included by the app Makefile
```

## Using this repo

`biowatch-core` isn't built standalone; it's pulled in as a submodule and its `core.mk` is included by the parent project's `Makefile`:

```make
include core/core.mk
CFLAGS  += $(CORE_DEFS)
INCS    += $(addprefix -I,$(CORE_INCS))
C_SRCS  += $(CORE_C_SRCS)
ASM_SRCS+= $(CORE_ASM_SRCS)
LDFLAGS += -T$(CORE_LDSCRIPT) -L$(CORE_BUILD) $(CORE_LIBS)
```

See [biowatch-firmware](https://github.com/Zeta-Chrome/biowatch-firmware) for a working example.

## Documentation

| Doc | Covers |
|---|---|
| [docs/kernel.md](docs/kernel.md) | Scheduler, task model, sync primitives (mutex/semaphore/event/mqueue), software timers, memory pools |
| [docs/lpm.md](docs/lpm.md) | Tickless idle, sleep/lp-sleep/standby modes, wake sources, measured power behaviour |
| [docs/ble.md](docs/ble.md) | CPU1↔CPU2 IPCC mailbox transport, HCI/ACI command layer, service-controller (svcctl) framework |

## Status

Actively developed alongside `biowatch-firmware`. The BLE stack binaries under `subsys/ble/fw` are STMicroelectronics-provided CPU2 wireless-stack images (FUS + BLE full stack) flashed separately via `make flash_ble` — everything else in this repo is original code.
