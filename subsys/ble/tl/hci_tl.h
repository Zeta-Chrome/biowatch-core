#ifndef BLE_HCI_TL_H
#define BLE_HCI_TL_H

#include "tl.h"
#include <stdbool.h>

typedef bool (*hci_evt_rx)(tl_evt_packet_t *p_evt_pkt);

struct hci_request {
	uint16_t ogf;
	uint16_t ocf;
	int event;
	void *cmd_param;
	int cmd_len;
	void *ret_param;
	int ret_len;
};

void hci_init(tl_cmd_packet_t *p_cmd_buf, hci_evt_rx evt_rx);
enum bw_status hci_send_req(struct hci_request *p_cmd);

#endif
