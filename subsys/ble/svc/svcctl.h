#ifndef BLE_SVC_CTL_H
#define BLE_SVC_CTL_H

#include "stdbool.h"
#include <stdint.h>

enum ble_uuid_type { BLE_SVC_UUID_TYPE_16 = 0x01U, BLE_SVC_UUID_TYPE_128 = 0x02U };
enum ble_svc_type { BLE_SVC_TYPE_PRIMARY = 0x01U, BLE_SVC_TYPE_SECONDARY = 0x02U };

struct ble_uuid {
	enum ble_uuid_type type;
	uint8_t uuid[16];
};

struct ble_char_handle {
	uint16_t svc_handle;
	uint16_t char_handle;
};

enum ble_char_prop {
	BLE_CHAR_PROP_NONE = 0x00,
	BLE_CHAR_PROP_BROADCAST = 0x01,
	BLE_CHAR_PROP_READ = 0x02,
	BLE_CHAR_PROP_WRITE_WITHOUT_RESP = 0x04,
	BLE_CHAR_PROP_WRITE = 0x08,
	BLE_CHAR_PROP_NOTIFY = 0x10,
	BLE_CHAR_PROP_INDICATE = 0x20,
	BLE_CHAR_PROP_SIGNED_WRITE = 0x40,
	BLE_CHAR_PROP_EXT = 0x80
};

enum ble_char_evt_notify {
	BLE_CHAR_EVT_DONT_NOTIFY = 0x00,
	BLE_CHAR_EVT_NOTIFY_ATTRIBUTE_WRITE = 0x01,
	BLE_CHAR_EVT_NOTIFY_WRITE_REQ_WAIT_FOR_APPL_RESP = 0x02,
	BLE_CHAR_EVT_NOTIFY_READ_REQ_WAIT_FOR_APPL_RESP = 0x04,
	BLE_CHAR_EVT_NOTIFY_NOTIFICATION_COMPLETION = 0x08
};

enum ble_char_evt_type {
	BLE_CHAR_EVT_WRITE,
	BLE_CHAR_EVT_WRITE_REQ,
	BLE_CHAR_EVT_READ_REQ,
	BLE_CHAR_EVT_NOTIFY_ENABLED,
	BLE_CHAR_EVT_NOTIFY_DISABLED,
	BLE_CHAR_EVT_INDICATE_ENABLED,
	BLE_CHAR_EVT_INDICATE_DISABLED,
};

struct ble_char_evt {
	enum ble_char_evt_type type;
	uint16_t conn_handle;
	const uint8_t *data;
	uint16_t len;
};

typedef void (*ble_svc_init_fn_t)(void);
typedef void (*ble_char_evt_cb_t)(const struct ble_char_evt *evt, void *ctx);

void ble_svcctl_register_init(ble_svc_init_fn_t init_fn);
void ble_svcctl_set_ad_svc_uuid(struct ble_uuid uuid);
void ble_svcctl_add_svc(const struct ble_uuid uuid, enum ble_svc_type type, uint8_t max_attr,
						uint16_t *svc_handle);
void ble_svcctl_add_char(uint16_t svc_handle, const struct ble_uuid uuid, uint16_t value_len,
						 uint8_t char_props, uint8_t char_evts, bool is_variable,
						 ble_char_evt_cb_t evt_cb, void *ctx, uint16_t *char_handle);
void ble_svcctl_update_char(uint16_t svc_handle, uint16_t char_handle, uint8_t offset, uint8_t len,
							const uint8_t *data);
void ble_svcctl_write_resp(uint16_t conn_handle, uint16_t attr_handle, bool accept);
void ble_svcctl_read_resp(uint16_t conn_handle, uint16_t attr_handle, const uint8_t *data,
						  uint16_t len, bool accept);

#endif
