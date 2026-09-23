# Low-Power Management (`subsys/lpm/`)

BioWatch runs a **tickless RTOS**: the idle task doesn't wake up on every tick just to check "is there work yet". Instead, whenever no task is ready to run, it consults `kernel_timer_deadline_ms()` and the state of any blocked tasks, picks the deepest power mode that won't miss that deadline, and enters it. Measured behaviour on-device: the MCU is asleep **>99.8%** of the time, waking only for interrupts (sensor data-ready, button press, BLE mailbox event, RTC/LPTIM timeout).

## Modes

```c
enum lpm_mode {
    LPM_MODE_RUN,
    LPM_MODE_LP_SLEEP,
    LPM_MODE_SLEEP,
    LPM_MODE_STOP,
    LPM_MODE_STANDBY,
    LPM_MODE_SHUTDOWN,
    LPM_MODE_COUNT
};
```

- **RUN** — full 64 MHz PLL sysclock (`clock_conf_performance()`). Used only while a task is actively doing work (reading a sensor, running a DSP algorithm, driving the display). The CPU is deliberately run as fast as possible during this window so it can finish and go back to sleep sooner — a "race to idle" strategy rather than running slow-and-long.
- **LP_SLEEP** — MSI at 1 MHz (`clock_conf_lp_sleep()`), CPU clock-gated. Used for short waits where a fast wake-up matters more than the deepest sleep.
- **SLEEP / STOP** — deeper CPU/peripheral clock gating, RAM retained.
- **STANDBY / SHUTDOWN** — deepest modes, entered explicitly (`lpm_enter_standby`, `lpm_enter_shutdown`) with a defined wake pin/edge; RAM is not retained across these, so the caller is choosing to reset application state on wake.

### Mode voting

```c
void lpm_enable_mode(enum lpm_mode mode, const char *name);
void lpm_disable_mode(enum lpm_mode mode, const char *name);
void lpm_enter_mode();
```

Modes are arbitrated by named "votes" rather than a single global flag: any subsystem that currently cannot tolerate a given depth of sleep (for example, BLE mid-transaction, or a driver mid-DMA-transfer) calls `lpm_disable_mode()` with its own tag, and `lpm_enter_mode()` always resolves to the shallowest mode that has an active disable vote — clearing the vote (`lpm_enable_mode`) hands the decision back down. This means individual drivers don't need to know about each other; they only need to know their own constraint.

## Wake sources

| Source | Used for |
|---|---|
| **LPTIM** | Sub-2-second deadlines and RTOS tick generation while sleeping — low-power timer that keeps running in Stop mode. |
| **RTC** | Deadlines beyond ~2 seconds (e.g. the once-a-minute environment BLE push), and calendar/wall-clock timekeeping. |
| **EXTI** (buttons, sensor INT lines, IPCC/HSEM lines) | Any asynchronous event — button press, IMU/oximeter data-ready, inter-processor mailbox signal from CPU2. |

Splitting the timer wake source by deadline length (LPTIM for short, RTC for long) avoids keeping a high-power timer block active just to cover the rare long sleep, while still giving sub-second wake resolution when something needs it soon.

## Interaction with drivers

Every peripheral driver in `drivers/` is interrupt/DMA-driven, never a blocking poll loop. This is a hard requirement for the sleep budget above to hold: a blocking `while(!flag)` spin would keep the core awake and defeat tickless idle entirely. A driver kicks off a transfer, the calling task blocks on a semaphore/notification, the core goes to sleep, and the DMA/peripheral-complete interrupt is what wakes it back up.

## Application-level power behaviour

The application layer (`biowatch-firmware`) builds on top of this by also reducing *how often* it asks the hardware to do anything — e.g. the display-brightness LDR is read every 2 s only while the screen is on, and is skipped entirely while the display is off, further shrinking the number of wake events per minute. See [../../biowatch-firmware/docs/tasks.md](../../biowatch-firmware/docs/tasks.md).
