#include "board.h"
#include "wireless_config.h"
#include "lora_app.h"
#define FLASH_SAVE_ADDR  (0x08000000 + 100*1000)
#define EEPROM_SAVE_ADDR (0)
static device_info_t device_info;
TimerHandle_t rest_timer;

device_info_t read_device_info_ram(void)
{
    return device_info; 
}

device_info_t read_device_info(void)
{
    osel_memset((uint8_t *)&device_info, 0 ,sizeof(device_info_t));
    eeprom_readbytes(EEPROM_SAVE_ADDR,(uint8_t *)&device_info, sizeof(device_info_t));
    if(device_info.mark != 0xfe || device_info.bandwidth > 2 || device_info.speading_factor>12 || device_info.speading_factor<7)
    {
        device_info.mark = 0xfe;
        device_info.id = 0xff;
        device_info.rf_channel = 20;
        device_info.rf_power = 17;
        device_info.bandwidth = 2;
        device_info.speading_factor = 10;
        eeprom_writebytes(EEPROM_SAVE_ADDR,(uint8_t *)&device_info, sizeof(device_info_t));
    }
    return device_info; 
}

static void north_send(uint8_t *data, uint8_t type, uint8_t len , uint64_t gateway, uint64_t src)
{
    uint8_t tmp[UART_SIZE];
    osel_memset(tmp , 0, UART_SIZE);
    
    uint8_t offset = 0;
    tmp[offset++] = 0xd5;
    tmp[offset++] = 0xc8;
    
    uint16_t length = 22 + len + 1;                     //协议头+报文体+crc
    uint16_t s2b = S2B_UINT16(length);
    osel_memcpy(&tmp[offset], (uint8_t *)&s2b, sizeof(uint16_t));
    offset += sizeof(uint16_t);
    
    agreement_head_t agreement_head;
    osel_memset((uint8_t *)&agreement_head , 0, sizeof(agreement_head_t));
    agreement_head.version = 0x00;
    agreement_head.type = type;
    
    s2b = S2B_UINT16(length);
    agreement_head.length = s2b;

    agreement_head.seq = 0x00;
    agreement_head.gateway = gateway;
    agreement_head.src = src;
    
    osel_memcpy(&tmp[offset], (uint8_t *)&agreement_head, sizeof(agreement_head_t));
    offset += sizeof(agreement_head_t);
    
    osel_memcpy(&tmp[offset], data, len);
    offset += len;
    
    uint16_t crc_len = offset-4;
    tmp[offset++] = crc_compute(&tmp[4],crc_len);               //todo : crc
    
    serial_write(UART_2, tmp, offset);
}

static void device_config_ack(void)
{
    uint8_t tmp[1];
    tmp[0] = 0x02;
    north_send(tmp, (0x11 | 0x80), 1 , device_info.id, device_info.id);
}

static void rest_callback(TimerHandle_t timer)
{
    NVIC_SystemReset();
}

static void device_config_deal(uint8_t *data)
{
    static uint8_t *p;
    p = data;
    if(*p == 0x02)              //设备属性配置
    {
        hal_int_state_t s = 0;
        HAL_ENTER_CRITICAL(s);	
        rest_timer = NULL;
        rest_timer = xTimerCreate("rest_timer",200 / portTICK_RATE_MS,pdTRUE,NULL,rest_callback);
        if (rest_timer == NULL) 
        {
            DBG_ASSERT(FALSE __DBG_LINE);
        }
        p++;
        osel_memcpy((uint8_t *)&device_info, p, sizeof(device_info_t) - 1);
        //device_info.id = S2B_UINT32(device_info.id);
        device_info.mark = 0xfe;
        eeprom_writebytes(EEPROM_SAVE_ADDR,(uint8_t *)&device_info, sizeof(device_info_t));
        device_config_ack();
        xTimerStart(rest_timer,0);
        HAL_EXIT_CRITICAL(s);
    }
}

static void device_config_select(uint8_t *data)
{
    static uint8_t *p;
    p = data;
    if(*p == 0x01)              //设备属性查询
    {
        uint8_t tmp[30];
        uint8_t offset = 0;
        tmp[offset++] = 0x01;
        
        device_info_t info;
        osel_memcpy((uint8_t *)&info, (uint8_t *)&device_info, sizeof(device_info_t));
        //info.id = S2B_UINT32(info.id);
        osel_memcpy(&tmp[offset], (uint8_t *)&info, sizeof(device_info_t));
        offset += sizeof(device_info_t);
        north_send(tmp, (0x12 | 0x80), offset , device_info.id, device_info.id);
    }
}

void wireless_config_handle(pbuf_t *pbuf)
{
    uint8_t version = 0;
    uint8_t type = 0;
    version = *pbuf->data_p;
    type = *(pbuf->data_p + 1);
    if(version == 0 && type == 0x11)
    {
        device_config_deal(pbuf->data_p + 22);          //偏移感知交通串口协议22个字节
    }
    else if(version == 0 && type == 0x12)
    {
        device_config_select(pbuf->data_p + 22);        //偏移感知交通串口协议22个字节
    }
    else
    {
        lora_send(pbuf->data_p, pbuf->data_len);
    }
    pbuf_free(&pbuf __PLINE2);
}