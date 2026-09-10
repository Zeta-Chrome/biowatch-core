#include "svcctl.h"
#include "lib/assert.h"
#include "lib/logger.h"
#include "subsys/ble/auto/ble_gatt_aci.h"
#include "subsys/ble/auto/ble_types.h"
#include "subsys/ble/ble_defs.h"
#include <stddef.h>
#include <string.h>

#define BLE_SVCCTL_MAX_CHARS 16
#define BLE_SVCCTL_MAX_ATT_VALUE_LEN 247 /* match CFG_BLE_MAX_ATT_MTU - 3, adjust if different */

struct char_registry_entry {
	uint16_t svc_handle;
	uint16_t char_handle;
	ble_char_evt_cb_t cb;
	void *ctx;
	uint8_t cccd_state;
};

struct pending_write_req {
	bool active;
	uint16_t conn_handle;
	uint16_t attr_handle;
	uint16_t len;
	uint8_t data[BLE_SVCCTL_MAX_ATT_VALUE_LEN];
};

struct pending_read_req {
	bool active;
	uint16_t conn_handle;
	uint16_t attr_handle;
	uint16_t offset;
	uint16_t svc_handle;
	uint16_t char_handle; /* gives us svc_handle at response time */
};

static struct ble_uuid g_ad_svc_uuid;
static ble_svc_init_fn_t g_init_fn;
static struct char_registry_entry g_registry[BLE_SVCCTL_MAX_CHARS];
static uint8_t g_registry_count;
static struct pending_write_req g_pending_write;
static struct pending_read_req g_pending_read;

// Private API
void ble_svcctl_init()
{
	BW_ASSERT(g_init_fn != NULL, "BLE svcctl init not registered");
	g_init_fn();
}

struct ble_uuid *ble_svcctl_get_ad_svc_uuid()
{
	return &g_ad_svc_uuid;
}

// PUBLIC API
void ble_svcctl_register_init(ble_svc_init_fn_t init_fn)
{
	g_init_fn = init_fn;
}

void ble_svcctl_set_ad_svc_uuid(struct ble_uuid uuid)
{
	g_ad_svc_uuid = uuid;
}

void ble_svcctl_add_svc(const struct ble_uuid uuid, enum ble_svc_type type, uint8_t max_attr,
						uint16_t *svc_handle)
{
	ble_status_t res = BLE_STATUS_SUCCESS;
	res = aci_gatt_add_service(uuid.type, (const service_uuid_t *)uuid.uuid, type, max_attr,
							   svc_handle);
	if (res == BLE_STATUS_SUCCESS)
		BW_LOG("Service is added Successfully %p\n", *svc_handle);
	else
		BW_LOG("FAILED to add Service, Error: %p !!\n", res);
}

void ble_svcctl_add_char(uint16_t svc_handle, const struct ble_uuid uuid, uint16_t value_len,
						 uint8_t char_props, uint8_t char_evts, bool is_variable,
						 ble_char_evt_cb_t evt_cb, void *ctx, uint16_t *char_handle)
{
	ble_status_t res = BLE_STATUS_SUCCESS;
	res = aci_gatt_add_char(svc_handle, uuid.type, (const char_uuid_t *)uuid.uuid, value_len,
							char_props, 0x0, char_evts, 0, is_variable, char_handle);
	if (res == BLE_STATUS_SUCCESS)
		BW_LOG("Characteristic Added Successfully  %p \n", *char_handle);
	else
		BW_LOG("FAILED to add Characteristic, Error: %p !!\n", res);

	if (evt_cb) {
		BW_ASSERT(g_registry_count < BLE_SVCCTL_MAX_CHARS, "svcctl registry full");
		g_registry[g_registry_count++] = (struct char_registry_entry){
			.svc_handle = svc_handle,
			.char_handle = *char_handle,
			.cb = evt_cb,
			.ctx = ctx,
			.cccd_state = 0,
		};
	}
}

void ble_svcctl_update_char(uint16_t svc_handle, uint16_t char_handle, uint8_t offset, uint8_t len,
							const uint8_t *data)
{
	ble_status_t res = BLE_STATUS_SUCCESS;
	res = aci_gatt_update_char_value(svc_handle, char_handle, offset, len, data);
	if (res == BLE_STATUS_SUCCESS)
		BW_LOG("Characteristic Updated Successfully  %p \n", char_handle);
	else
		BW_LOG("FAILED to update Characteristic, Error: %p !!\n", res);
}

static struct char_registry_entry *find_char(uint16_t attr_handle, uint16_t *offset)
{
	for (uint8_t i = 0; i < g_registry_count; i++) {
		uint16_t off = attr_handle - g_registry[i].char_handle;
		if (off == 1 || off == 2) { /* 1 = value handle, 2 = CCCD */
			*offset = off;
			return &g_registry[i];
		}
	}
	return NULL;
}

void ble_svcctl_dispatch_write(uint16_t conn_handle, uint16_t attr_handle, const uint8_t *data,
							   uint16_t len, bool needs_resp)
{
	uint16_t offset;
	struct char_registry_entry *e = find_char(attr_handle, &offset);
	if (!e)
		return;

	if (needs_resp) {
		BW_ASSERT(len <= BLE_SVCCTL_MAX_ATT_VALUE_LEN,
				  "write payload too large for pending buffer");
		g_pending_write.active = true;
		g_pending_write.conn_handle = conn_handle;
		g_pending_write.attr_handle = attr_handle;
		g_pending_write.len = len;
		memcpy(g_pending_write.data, data, len);
	}

	struct ble_char_evt evt = { .conn_handle = conn_handle, .data = data, .len = len };

	if (offset == 1) {
		evt.type = needs_resp ? BLE_CHAR_EVT_WRITE_REQ : BLE_CHAR_EVT_WRITE;
	} else { /* CCCD */
		uint8_t old = e->cccd_state;
		uint8_t new = data[0];
		e->cccd_state = new;

		bool notify_was = old & 0x1, indicate_was = old & 0x2;
		bool notify_is = new & 0x1, indicate_is = new & 0x2;

		if (notify_is && !notify_was)
			evt.type = BLE_CHAR_EVT_NOTIFY_ENABLED;
		else if (!notify_is && notify_was)
			evt.type = BLE_CHAR_EVT_NOTIFY_DISABLED;
		else if (indicate_is && !indicate_was)
			evt.type = BLE_CHAR_EVT_INDICATE_ENABLED;
		else
			evt.type = BLE_CHAR_EVT_INDICATE_DISABLED;
	}

	if (e->cb)
		e->cb(&evt, e->ctx);
}

void ble_svcctl_write_resp(uint16_t conn_handle, uint16_t attr_handle, bool accept)
{
	if (!g_pending_write.active) {
		BW_LOG("write_resp called with no pending write\n");
		return;
	}
	if (g_pending_write.conn_handle != conn_handle || g_pending_write.attr_handle != attr_handle) {
		BW_LOG("write_resp conn/attr mismatch - stale response\n");
		return;
	}

	ble_status_t res = aci_gatt_permit_write(conn_handle, attr_handle, accept ? 0x00 : 0x01,
											 accept ? 0x00 : 0x08, g_pending_write.len,
											 g_pending_write.data);
	if (res != BLE_STATUS_SUCCESS)
		BW_LOG("FAILED aci_gatt_write_resp, Error: %p !!\n", res);

	g_pending_write.active = false;
}

void ble_svcctl_dispatch_read_req(uint16_t conn_handle, uint16_t attr_handle, uint16_t offset)
{
	uint16_t char_offset;
	struct char_registry_entry *e = find_char(attr_handle, &char_offset);
	if (!e)
		return;

	g_pending_read.active = true;
	g_pending_read.conn_handle = conn_handle;
	g_pending_read.attr_handle = attr_handle;
	g_pending_read.offset = offset;
	g_pending_read.svc_handle = e->svc_handle;
	g_pending_read.char_handle = e->char_handle;

	struct ble_char_evt evt = {
		.type = BLE_CHAR_EVT_READ_REQ,
		.conn_handle = conn_handle,
		.data = NULL,
		.len = 0,
	};

	if (e->cb)
		e->cb(&evt, e->ctx);
}

void ble_svcctl_read_resp(uint16_t conn_handle, uint16_t attr_handle, const uint8_t *data,
						  uint16_t len, bool accept)
{
	if (!g_pending_read.active) {
		BW_LOG("read_resp called with no pending read\n");
		return;
	}
	if (g_pending_read.conn_handle != conn_handle || g_pending_read.attr_handle != attr_handle) {
		BW_LOG("read_resp conn/attr mismatch - stale response\n");
		return;
	}

	ble_status_t res;

	if (accept) {
		res = aci_gatt_update_char_value(g_pending_read.svc_handle, attr_handle,
										 g_pending_read.offset, len, data);
		if (res != BLE_STATUS_SUCCESS)
			BW_LOG("FAILED aci_gatt_update_char_value in read_resp, Error: %p !!\n", res);
	}

	res =
		aci_gatt_permit_read(conn_handle, accept ? 0x00 : 0x01, accept ? 0x00 : 0x08, attr_handle);
	if (res != BLE_STATUS_SUCCESS)
		BW_LOG("FAILED aci_gatt_permit_read, Error: %p !!\n", res);

	g_pending_read.active = false;
}
