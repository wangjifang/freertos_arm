#pragma once
#include "./stack/stack.h"

#pragma pack(1)
typedef struct 
{
    uint64_t    id;
    uint8_t     rf_channel;            //*< rf信道
    uint8_t     rf_power;              //*< rf功率
    uint8_t     bandwidth;             //*< 带宽
    uint8_t     speading_factor;       //*< 扩频因子
    uint8_t    mark;
} device_info_t;

typedef struct 
{
    uint8_t version;
    uint8_t type;
    uint16_t length;
    uint16_t seq;
    uint64_t gateway;
    uint64_t src;
}agreement_head_t;

#pragma pack()

void wireless_config_handle(pbuf_t *pbuf);

device_info_t read_device_info(void);

device_info_t read_device_info_ram(void);