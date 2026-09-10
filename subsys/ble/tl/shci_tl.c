#include "shci_tl.h"
#include "kernel/sync/mutex.h"
#include "kernel/sync/semaphore.h"
#include "kernel/task/task.h"
#include "lib/status.h"
#include "tl.h"
#include "subsys/ble/utilities/stm_clist.h"
#include <stddef.h>
#include <string.h>

#define SHCI_EVT_TASK_PRIO 4
#define SHCI_RESP_TIMEOUT 1000

PLACE_IN_SECTION("SYSTEM_DRIVER_CONTEXT") static struct stm_list_node g_shci_asynch_evt_queue;
PLACE_IN_SECTION("SYSTEM_DRIVER_CONTEXT") static tl_cmd_packet_t *g_p_cmd_buf;

static shci_evt_rx g_shci_evt_rx;
static struct tl_sys_conf g_sys_conf;
static struct mutex g_cmd_mutex;
static struct semaphore g_cmd_rx_sem;
static task_handle_t g_shci_evt_task_h;

static void shci_cmd_callback();
static void shci_evt_callback(tl_evt_packet_t *p_evt_pkt);
static void shci_evt_task(void *user_data);

void shci_init(tl_cmd_packet_t *p_cmd_buf, shci_evt_rx evt_rx)
{
	stm_list_init(&g_shci_asynch_evt_queue);
	kernel_mutex_init(&g_cmd_mutex);
	kernel_semaphore_binary_init(&g_cmd_rx_sem, true);
	kernel_task_create(shci_evt_task, "SHCI evt task", SHCI_EVT_TASK_PRIO, 256, NULL,
					   &g_shci_evt_task_h);

	g_shci_evt_rx = evt_rx;
	g_p_cmd_buf = p_cmd_buf;

	g_sys_conf.p_cmd_buffer = p_cmd_buf;
	g_sys_conf.cmd_callback = shci_cmd_callback;
	g_sys_conf.evt_callback = shci_evt_callback;
	tl_sys_init(&g_sys_conf);
}

enum bw_status shci_send(uint16_t cmd_code, uint8_t cmd_pl_len, uint8_t *p_cmd_pl,
						 tl_evt_packet_t *p_rsp)
{
	kernel_mutex_lock(&g_cmd_mutex, MAX_TIMEOUT);

	g_p_cmd_buf->cmd_serial.cmd.cmd_code = cmd_code;
	g_p_cmd_buf->cmd_serial.cmd.payload_len = cmd_pl_len;
	memcpy(g_p_cmd_buf->cmd_serial.cmd.payload, p_cmd_pl, cmd_pl_len);

	tl_sys_send_cmd();
	enum bw_status status = kernel_semaphore_take(&g_cmd_rx_sem, SHCI_RESP_TIMEOUT);
	if (status != STATUS_OK) {
		BW_LOG("Timeout occured in shci_send\n");
		kernel_mutex_unlock(&g_cmd_mutex);
		return status;
	}

	memcpy(&(p_rsp->evt_serial), g_p_cmd_buf,
		   ((tl_evt_serial_t *)g_p_cmd_buf)->evt.payload_len + TL_EVT_HDR_SIZE);

	kernel_mutex_unlock(&g_cmd_mutex);
	return status;
}

static void shci_cmd_callback()
{
	kernel_semaphore_give_from_isr(&g_cmd_rx_sem);
}

static void shci_evt_callback(tl_evt_packet_t *p_evt_pkt)
{
	stm_list_insert_tail(&g_shci_asynch_evt_queue, (stm_list_node_t *)p_evt_pkt);
	kernel_task_notify_from_isr(g_shci_evt_task_h, 0, NOTIFY_ACTION_NONE);
}

static void shci_evt_task(void *user_data)
{
	(void)user_data;

	bool evt_flow = true;
	tl_evt_packet_t *p_shci_evt_buf;

	while (1) {
		kernel_task_notify_wait(0, 0, NULL, MAX_TIMEOUT);

		if (!evt_flow)
			continue;

		while (!stm_list_is_empty(&g_shci_asynch_evt_queue)) {
			stm_list_remove_head(&g_shci_asynch_evt_queue, (stm_list_node_t **)&p_shci_evt_buf);

			// Event flow stops if FUS is running instead of BLE
			evt_flow = g_shci_evt_rx(p_shci_evt_buf);
			if (evt_flow)
				tl_mm_evt_done(p_shci_evt_buf);
			else
				stm_list_insert_head(&g_shci_asynch_evt_queue, (stm_list_node_t *)p_shci_evt_buf);
		}
	}
}
