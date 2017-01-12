#include "app.h"
#include "osel_arch.h"
#include "board.h"
#include "lora_app.h"
#include "./stack/stack.h"
#include "wireless_config.h"
#define APP_MSG                    (1)

typedef enum
{
    APP_UART_RECV,
}APP_MSG_E;

#define appTask_STACK_SIZE          ( configMINIMAL_STACK_SIZE * 5 )
#define appTask_PRIORITY            ( tskIDLE_PRIORITY + 1 )

static QueueHandle_t app_rq;

static uint16_t head_package(uint8_t *package)
{
    uint16_t length = 0;
    TimerTime_t time = TimerHwGetTime();
    package[length++] = 0xd5;
    package[length++] = 0xc8;
    length+=2;
    osel_memcpy(&package[length], (uint8_t *)&time, sizeof(TimerTime_t));
    length+=sizeof(TimerTime_t);
    return length;
}

static void appTask( void *pvParameters )
{
    esn_msg_t esn_msg;
    uint8_t type;
    while(TRUE)
    {
        if (xQueueReceive(app_rq,&esn_msg, portMAX_DELAY))
        {
            type = esn_msg.event >> 8;
            switch(type)
            {
            case APP_UART_RECV:
//                wireless_config_handle(esn_msg.param);
                break;
            }
        }
        //vTaskDelay(200 / portTICK_RATE_MS);
    }
}

static void lora_recv_event(uint8_t * buf, uint8_t length, int16_t rssi)
{
    uint16_t len = 0;
    uint8_t package[LORA_MAX_RECV_LENGTH];
    len = head_package(package);
    osel_memcpy(&package[len], (uint8_t *)&rssi, sizeof(int16_t));
    len += sizeof(int16_t);
    osel_memcpy(&package[len], buf, length);
    len += length;
    uint16_t s2b_16 = S2B_UINT16(len-4);
    osel_memcpy(&package[2], (uint8_t *)&s2b_16, sizeof(uint16_t));
	
    serial_write(UART_2, package, len);
    lora_recv(lora_recv_event);
}

static void uart1_event(void)
{
    uint8_t frame[UART_SIZE];
    uint8_t uart_head[4];
    uint16_t frame_len,out_frame_len;
    serial_read(UART_1, &uart_head[0], 4);
    osel_memcpy((uint8_t *)&frame_len, &uart_head[2], 2);
    frame_len = S2B_UINT16(frame_len);
    if (frame_len > UART_SIZE)
    {
        return ;
    }
    out_frame_len = serial_read(UART_1, frame, frame_len);
    if (out_frame_len != frame_len)
    {
        return ;
    }

    pbuf_t *frm_buf = pbuf_alloc(out_frame_len __PLINE1);
    if (frm_buf == NULL)
    {
        DBG_ASSERT(FALSE __DBG_LINE);
    }
    frm_buf->data_len = out_frame_len;
    frm_buf->data_p = frm_buf->head;
    osel_memcpy(&frm_buf->data_p[0], frame, out_frame_len);
    
    esn_msg_t esn_msg;
    esn_msg.event = (APP_UART_RECV << 8) | APP_MSG ;
    esn_msg.param = frm_buf;
    xQueueSend(app_rq, &esn_msg, portMAX_DELAY);
}

static void uart_frame_cfg(void)
{
    serial_reg_t serial;
    serial.sd.valid = TRUE;
    serial.sd.len = 2;
    serial.sd.pos = 0;
    serial.sd.data[0] = 0xd5;
    serial.sd.data[1] = 0xc8;
    serial.ld.valid = TRUE;
    serial.ld.little_endian = FALSE;
    serial.argu.len_max = UART_SIZE;
    serial.argu.len_min = 2;
    serial.ed.valid = FALSE;
    serial.echo_en = FALSE;
    serial.func_ptr = uart1_event;
    serial_fsm_init(UART_1);
    serial_reg(UART_1, serial);
}

void app_init(void)
{
    uart_frame_cfg();
    device_info_t device_info = read_device_info();
//    lora_app_init();
//    lora_recv(lora_recv_event);
    app_rq = xQueueCreate( 4, sizeof( esn_msg_t ) );
    if(pdTRUE != xTaskCreate( appTask, "appTask", appTask_STACK_SIZE, NULL, appTask_PRIORITY, NULL ))
    {
        DBG_ASSERT(FALSE __DBG_LINE);
    }
}
