# BLE Subsystem (`subsys/ble/`)

The STM32WB55 is a dual-core part: CPU1 (Cortex-M4) runs this project's RTOS and application; CPU2 (Cortex-M0+) runs ST's closed-source wireless stack, flashed separately (`make flash_ble`, binaries in `subsys/ble/fw/`). `subsys/ble/` is CPU1's side of that boundary — the transport, the command layer, and a small service-registration framework that the application builds its GATT database on top of.

This layer was written from scratch (learning from ST's reference implementation) specifically so BLE could run on this custom RTOS and driver stack instead of on STM32Cube/HAL.

## Layers, bottom to top

```
tl/            Transport Layer — IPCC-based mailbox between CPU1 and CPU2
shci/          System channel HCI — stack config, FUS commands (firmware update, BLE stack start)
auto/          Generated HCI/ACI command + event encoders/decoders (GAP, GATT, L2CAP, HAL, generic)
svc/svcctl.c   Service-controller: register services/characteristics, dispatch attribute events
ble.c          Init/config, connection state machine, advertising parameters, pairing/IO capability
```

### Transport (`tl/`)

`tl_mbox.c` / `shci_tl.c` implement the mailbox protocol over **IPCC** (Inter-Processor Communication Controller) with **HSEM** (hardware semaphore) for mutual exclusion between the two cores — this is the piece described in the core [kernel/lpm](lpm.md) docs as an EXTI wake source (IPCC line 36, HSEM line 38). Commands going to CPU2 and events/responses coming back are exchanged through shared-RAM circular lists (`stm_clist.c`), with CPU1 woken by EXTI when CPU2 has something waiting.

### Command layer (`auto/`)

The `ble_*_aci.c/.h` and `ble_hci_le.c` files are the encode/decode layer for ST's HCI/ACI command set (GAP, GATT, L2CAP, HAL, generic access) — one function per command/event, generated from ST's command tables so the rest of the codebase calls plain C functions instead of hand-packing HCI byte buffers.

### Service controller (`svc/svcctl.c`)

A thin framework so the application layer doesn't touch ACI calls directly to build its GATT database:

```c
enum bw_status ble_svcctl_add_svc(struct ble_uuid uuid, enum ble_svc_type type,
                                   uint8_t attr_records, struct ble_char_handle *handle);
enum bw_status ble_svcctl_add_char(struct ble_char_handle svc_handle, struct ble_uuid uuid,
                                    uint16_t max_len, enum ble_char_prop props,
                                    enum ble_char_evt_notify notify_mask, bool is_variable,
                                    ble_char_evt_cb_t cb, void *ctx, struct ble_char_handle *out);
enum bw_status ble_svcctl_update_char(struct ble_char_handle handle, /* ... */);
```

Each characteristic can register its own event callback (`ble_char_evt_cb_t`) for writes/read-requests/notification-state-changes, and svcctl dispatches the underlying GATT attribute-modified events to the right callback — so, for example, a "settings write" characteristic and a "vitals notify" characteristic each handle their own logic without a central switch statement growing per service. The actual service/characteristic *definitions* (Activity, Vitals, Environment, Settings, Device Information, and so on) live in the application layer — `subsys/ble` only provides the mechanism.

### Connection & security (`ble.c`)

`ble_init(struct ble_conf *conf)` configures the device name/appearance, IO capability, MITM/bonding policy, and connection/disconnection/tx-free callbacks used by the application's BLE task to drive its own state machine (`BLE_CONN_STATUS_IDLE` → advertising → connected, etc.).

## Power interaction

BLE is one of the subsystems that actively votes against deep sleep through the LPM arbitration described in [lpm.md](lpm.md): mid-transaction over IPCC, `ble`/`tl` disable the deeper LPM modes so a mailbox exchange with CPU2 isn't interrupted by CPU1 dropping into Stop/Standby, then re-enable them once the exchange completes.
