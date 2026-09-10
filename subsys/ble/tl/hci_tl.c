#include "hci_tl.h"
#include "kernel/sync/mutex.h"
#include "kernel/sync/semaphore.h"
#include "lib/status.h"
#include "subsys/ble/tl/tl.h"
#include "subsys/ble/utilities/stm_clist.h"
#include <stddef.h>
#include <string.h>

#define HCI_EVT_TASK_PRIO 4
#define HCI_RESP_TIMEOUT 33000

PLACE_IN_SECTION("SYSTEM_DRIVER_CONTEXT") static struct stm_list_node g_hci_asynch_evt_queue;
PLACE_IN_SECTION("SYSTEM_DRIVER_CONTEXT") static tl_cmd_packet_t *g_p_cmd_buf;

static struct stm_list_node g_hci_cmd_evt_queue;
static hci_evt_rx g_hci_evt_rx;
static struct tl_ble_conf g_ble_conf;
static struct mutex g_cmd_mutex;
static struct semaphore g_cmd_rx_sem;
static task_handle_t g_hci_evt_task_h;

static void hci_evt_callback(tl_evt_packet_t *p_evt_pkt);
static void hci_evt_task(void *user_data);

void hci_init(tl_cmd_packet_t *p_cmd_buf, hci_evt_rx evt_rx)
{
	stm_list_init(&g_hci_cmd_evt_queue);
	stm_list_init(&g_hci_asynch_evt_queue);
	kernel_mutex_init(&g_cmd_mutex);
	kernel_semaphore_binary_init(&g_cmd_rx_sem, true);
	kernel_task_create(hci_evt_task, "HCI evt task", HCI_EVT_TASK_PRIO, 256, NULL,
					   &g_hci_evt_task_h);

	g_p_cmd_buf = p_cmd_buf;
	g_hci_evt_rx = evt_rx;

	g_ble_conf.p_cmd_buffer = p_cmd_buf;
	g_ble_conf.evt_callback = hci_evt_callback;
	tl_ble_init(&g_ble_conf);
}

enum bw_status hci_send_req(struct hci_request *p_cmd)
{
	tl_cc_evt_t *p_cmd_complete_evt;
	tl_cs_evt_t *p_cmd_status_evt;
	tl_evt_packet_t *p_evt_pkt;
	uint16_t opcode;

	kernel_mutex_lock(&g_cmd_mutex, MAX_TIMEOUT);

	opcode = ((p_cmd->ocf) & 0x03ff) | ((p_cmd->ogf) << 10);

	g_p_cmd_buf->cmd_serial.cmd.cmd_code = opcode;
	g_p_cmd_buf->cmd_serial.cmd.payload_len = p_cmd->cmd_len;
	memcpy(g_p_cmd_buf->cmd_serial.cmd.payload, p_cmd->cmd_param, p_cmd->cmd_len);

	tl_ble_send_cmd();

	bool busy = true;
	while (busy) {
		enum bw_status status = kernel_semaphore_take(&g_cmd_rx_sem, HCI_RESP_TIMEOUT);
		if (status == STATUS_TIMEOUT) {
			BW_LOG("Timeout occured while waiting for hci request response\n");
			kernel_mutex_unlock(&g_cmd_mutex);
			return status;
		}

		while (!stm_list_is_empty(&g_hci_cmd_evt_queue)) {
			stm_list_remove_head(&g_hci_cmd_evt_queue, (stm_list_node_t **)&p_evt_pkt);

			if (p_evt_pkt->evt_serial.evt.evtcode == TL_BLEEVT_CS_OPCODE) {
				p_cmd_status_evt = (tl_cs_evt_t *)p_evt_pkt->evt_serial.evt.payload;

				if (p_cmd_status_evt->cmd_code == opcode)
					*(uint8_t *)(p_cmd->ret_param) = p_cmd_status_evt->status;

				if (p_cmd_status_evt->num_cmd != 0)
					busy = false;
			} else {
				p_cmd_complete_evt = (tl_cc_evt_t *)p_evt_pkt->evt_serial.evt.payload;

				if (p_cmd_complete_evt->cmd_code == opcode) {
					p_cmd->ret_len = MIN(p_evt_pkt->evt_serial.evt.payload_len - TL_EVT_HDR_SIZE,
										 p_cmd->ret_len);
					memcpy(p_cmd->ret_param, p_cmd_complete_evt->payload, p_cmd->ret_len);
				}

				if (p_cmd_complete_evt->num_cmd != 0)
					busy = false;
			}
		}
	}

	kernel_mutex_unlock(&g_cmd_mutex);

	return STATUS_OK;
}

static void hci_evt_callback(tl_evt_packet_t *p_evt_pkt)
{
	if ((p_evt_pkt->evt_serial.evt.evtcode == TL_BLEEVT_CS_OPCODE) ||
		(p_evt_pkt->evt_serial.evt.evtcode == TL_BLEEVT_CC_OPCODE)) {
		stm_list_insert_tail(&g_hci_cmd_evt_queue, (stm_list_node_t *)p_evt_pkt);
		kernel_semaphore_give_from_isr(&g_cmd_rx_sem);
	} else {
		stm_list_insert_tail(&g_hci_asynch_evt_queue, (stm_list_node_t *)p_evt_pkt);
		kernel_task_notify_from_isr(g_hci_evt_task_h, 0, NOTIFY_ACTION_NONE);
	}
}

static void hci_evt_task(void *user_data)
{
	(void)user_data;

	bool evt_flow = true;
	tl_evt_packet_t *p_hci_evt_buf;

	while (1) {
		kernel_task_notify_wait(0, 0, NULL, MAX_TIMEOUT);

		if (!evt_flow)
			continue;

		while (!stm_list_is_empty(&g_hci_asynch_evt_queue)) {
			stm_list_remove_head(&g_hci_asynch_evt_queue, (stm_list_node_t **)&p_hci_evt_buf);

			evt_flow = g_hci_evt_rx(p_hci_evt_buf);
			if (evt_flow)
				tl_mm_evt_done(p_hci_evt_buf);
			else
				stm_list_insert_head(&g_hci_asynch_evt_queue, (stm_list_node_t *)p_hci_evt_buf);
		}
	}
}
