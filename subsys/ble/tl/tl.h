#ifndef BLE_TL_H
#define BLE_TL_H

#include "lib/utils.h"

#define TL_BLECMD_PKT_TYPE (0x01)
#define TL_ACL_DATA_PKT_TYPE (0x02)
#define TL_BLEEVT_PKT_TYPE (0x04)
#define TL_SYSCMD_PKT_TYPE (0x10)
#define TL_SYSRSP_PKT_TYPE (0x11)
#define TL_SYSEVT_PKT_TYPE (0x12)
#define TL_TRACES_APP_PKT_TYPE (0x40)
#define TL_TRACES_WL_PKT_TYPE (0x41)

#define TL_CMD_HDR_SIZE (4)
#define TL_EVT_HDR_SIZE (3)
#define TL_EVT_CS_PAYLOAD_SIZE (4)

#define TL_BLEEVT_CC_OPCODE (0x0E)
#define TL_BLEEVT_CS_OPCODE (0x0F)
#define TL_BLEEVT_VS_OPCODE (0xFF)

#define TL_BLEEVT_CC_PACKET_SIZE (TL_EVT_HDR_SIZE + sizeof(tl_cc_evt_t))
#define TL_BLEEVT_CC_BUFFER_SIZE (sizeof(tl_packet_header_t) + TL_BLEEVT_CC_PACKET_SIZE)

typedef PACKED_STRUCT
{
    uint32_t *next;
    uint32_t *prev;
}
tl_packet_header_t;

typedef PACKED_STRUCT
{
    uint8_t status;
    uint8_t num_cmd;
    uint16_t cmd_code;
}
tl_cs_evt_t;

typedef PACKED_STRUCT
{
    uint8_t num_cmd;
    uint16_t cmd_code;
    uint8_t payload[2];
}
tl_cc_evt_t;

typedef PACKED_STRUCT
{
    uint16_t sub_evt_code;
    uint8_t payload[2];
}
tl_async_evt_t;

typedef PACKED_STRUCT
{
    uint8_t evt_code;
    uint8_t payload_len;
    uint8_t payload[2];
}
tl_evt_t;

typedef PACKED_STRUCT
{
    uint8_t type;
    tl_evt_t evt;
}
tl_evt_serial_t;

typedef PACKED_STRUCT
{
    tl_packet_header_t header;
    tl_evt_serial_t evt_serial;
}
tl_evt_packet_t;

typedef PACKED_STRUCT
{
    uint16_t cmd_code;
    uint8_t payload_len;
    uint8_t payload[255];
}
tl_cmd_t;

typedef PACKED_STRUCT
{
    uint8_t type;
    tl_cmd_t cmd;
}
tl_cmd_serial_t;

typedef PACKED_STRUCT
{
    tl_packet_header_t header;
    tl_cmd_serial_t cmd_serial;
}
tl_cmd_packet_t;

typedef PACKED_STRUCT
{
    uint8_t type;
    uint16_t handle;
    uint16_t length;
    uint8_t acl_data[1];
}
tl_acl_data_serial_t;

typedef PACKED_STRUCT
{
    tl_packet_header_t header;
    tl_acl_data_serial_t acl_data_serial;
}
tl_acl_data_packet_t;

typedef void (*evt_callback_t)(tl_evt_packet_t *);
typedef void (*cmd_callback_t)();

typedef struct
{
    tl_cmd_packet_t *p_cmd_buffer;
    evt_callback_t evt_callback;
    cmd_callback_t cmd_callback;
} tl_sys_conf_t;

typedef struct
{
    uint8_t *p_async_evt_pool;
    uint32_t async_evt_pool_size;
    uint8_t *p_ble_spare_evt_buffer;
    uint8_t *p_sys_spare_evt_buffer;
    uint8_t *p_traces_evt_pool;
    uint32_t traces_evt_pool_size;
} tl_mm_conf_t;

typedef struct
{
    evt_callback_t evt_callback;
} tl_traces_conf_t;

typedef void (*acl_data_ack_callback_t)();

typedef struct
{
    tl_cmd_packet_t *p_cmd_buffer;
    uint8_t *p_hci_acl_data_buffer;
    evt_callback_t evt_callback;
    acl_data_ack_callback_t acl_data_ack_callback;
} tl_ble_conf_t;

void tl_init();
void tl_enable();
void tl_sys_init(tl_sys_conf_t *conf);
void tl_sys_send_cmd();
void tl_mm_init(tl_mm_conf_t *conf);
void tl_mm_evt_done(tl_evt_packet_t *p_hci_evt);
void tl_traces_init(tl_traces_conf_t *conf);
void tl_ble_init(tl_ble_conf_t *conf);
void tl_ble_send_cmd();
void tl_ble_send_acl_data();

#endif
