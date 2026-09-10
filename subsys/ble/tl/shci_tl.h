#ifndef BLE_SHCI_TL_H
#define BLE_SHCI_TL_H

#include <stdbool.h>
#include "subsys/ble/tl/tl.h"

typedef bool (*shci_evt_rx)(tl_evt_packet_t *p_evt_pkt);

void shci_init(tl_cmd_packet_t *p_cmd_buf, shci_evt_rx evt_rx);
enum bw_status shci_send(uint16_t cmd_code, uint8_t cmd_pl_len, uint8_t *p_cmd_pl,
						 tl_evt_packet_t *p_rsp);

#endif
