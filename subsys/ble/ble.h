#ifndef SUBSYS_BLE_H

#define SUBSYS_BLE_H

#include "stdbool.h"
#include "svc/svcctl.h" // IWYU pragma: export
#include "ble_sig_uuids.h"

enum ble_io_capability {
	BLE_IO_CAPABILITY_DISPLAY_ONLY = 0x00,
	BLE_IO_CAPABILITY_DISPLAY_YES_NO = 0x01,
	BLE_IO_CAPABILITY_KEYBOARD_ONLY = 0x02,
	BLE_IO_CAPABILITY_NO_INPUT_NO_OUTPUT = 0x03,
	BLE_IO_CAPABILITY_KEYBOARD_DISPLAY = 0x04
};

enum ble_secure_support {
	BLE_SECURE_NOT_SUPPORTED = 0x00,
	BLE_SECURE_OPTIONAL = 0x01,
	BLE_SECURE_MANDATORY = 0x02
};

enum ble_keypress_support { BLE_KEYPRESS_NOT_SUPPORTED = 0x00, BLE_KEYPRESS_SUPPORTED = 0x01 };

enum ble_conn_status {
	BLE_CONN_STATUS_IDLE,
	BLE_CONN_STATUS_FAST_ADV,
	BLE_CONN_STATUS_LP_ADV,
	BLE_CONN_STATUS_SCAN,
	BLE_CONN_STATUS_LP_CONNECTING,
	BLE_CONN_STATUS_CONNECTED_SERVER,
	BLE_CONN_STATUS_CONNECTED_CLIENT
};

typedef void (*ble_conn_evt_cb_t)();

struct ble_conf {
	const char *name;
	enum ble_appearance_uuid appearance;
	enum ble_io_capability io_capability;
	enum ble_secure_support secure_support;
	bool mitm_protection;
	bool bonding_mode;
	ble_conn_evt_cb_t connection_evt_cb;
	ble_conn_evt_cb_t disconnection_evt_cb;
};

void ble_init(struct ble_conf *conf);
void ble_adv_start(enum ble_conn_status status);
void ble_adv_stop();
void ble_terminate();
enum ble_conn_status ble_get_conn_status();

#endif
