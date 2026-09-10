#include "ble_debug.h"
#include "shci/shci.h"
#include "tl/tl.h"
#include <stddef.h>

#define BLE_DTB_CFG 0
#define SYS_DBG_CFG1 (SHCI_C2_DEBUG_OPTIONS_IPCORE_LP | SHCI_C2_DEBUG_OPTIONS_CPU2_STOP_EN)
#define NBR_OF_TRACES_CONFIG_PARAMETERS 4
#define NBR_OF_GENERAL_CONFIG_PARAMETERS 4

PLACE_IN_SECTION("MB_MEM2")
ALIGN(4) static shci_c2_debug_traces_config_t g_debug_traces_config = { 0, 0, 1, 0 };
PLACE_IN_SECTION("MB_MEM2")
ALIGN(4)
static shci_c2_debug_general_config_t g_debug_general_config = { BLE_DTB_CFG,
																 SYS_DBG_CFG1,
																 { 0, 0 } };

static void traces_evt_callback(tl_evt_packet_t *evt_pkt);

void ble_debug_init()
{
	shci_c2_debug_init_cmd_packet_t dbg_cmd_pkt = {
		{ { 0, 0, 0 } }, /**< Does not need to be initialized */
		{ NULL, (uint8_t *)&g_debug_traces_config, (uint8_t *)&g_debug_general_config, 0,
		  NBR_OF_TRACES_CONFIG_PARAMETERS, NBR_OF_GENERAL_CONFIG_PARAMETERS }
	};

	struct tl_traces_conf tr_conf = { .evt_callback = traces_evt_callback };
	tl_traces_init(&tr_conf);

	enum shci_cmd_status status = shci_c2_debug_init(&dbg_cmd_pkt);
	if (status != SHCI_SUCCESS) {
		BW_LOG("Fail : shci_c2_debug_init command, result: %x\n", status);
		bw_error_handler();
	} else {
		BW_LOG("Success : shci_c2_debug_init command\n");
	}
}

static void traces_evt_callback(tl_evt_packet_t *evt_pkt)
{
}
