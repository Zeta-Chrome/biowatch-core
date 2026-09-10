#ifndef BLE_SVC_CTL_P_H
#define BLE_SVC_CTL_P_H

#include "stdbool.h"
#include <stdint.h>

void ble_svcctl_init();
struct ble_uuid *ble_svcctl_get_ad_svc_uuid();
void ble_svcctl_dispatch_write(uint16_t conn_handle, uint16_t attr_handle, const uint8_t *data,
							   uint16_t len, bool needs_resp);
void ble_svcctl_dispatch_read_req(uint16_t conn_handle, uint16_t attr_handle, uint16_t offset);

#endif
