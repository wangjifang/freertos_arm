#pragma once

#define LORA_MAX_RECV_LENGTH    (255u)

typedef enum
{
    TX_DONE,
    RX_DONE,
    TX_TIME_OUT,
    RX_TIME_OUT,
    RX_ERROR,
}lora_msg_type;


typedef void (*lora_recv_cb_t)(uint8_t * buf, uint8_t length, int16_t rssi);

void lora_app_init(void);
void lora_send(uint8_t *pbuf,uint8_t length);
void lora_recv(lora_recv_cb_t recv_cb);