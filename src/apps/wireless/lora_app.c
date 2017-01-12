#include "osel_arch.h"
#include "board.h"
#include "sx1278.h"
#include "lora_cfg.h"
#include "lora_app.h"
#include "wireless_config.h"

#define loraTask_STACK_SIZE          ( configMINIMAL_STACK_SIZE * 3 )
#define loraTask_PRIORITY            ( tskIDLE_PRIORITY + 2 )

static RadioEvents_t RadioEvents;
static QueueHandle_t lora_rq;
static lora_recv_cb_t lora_recv_cb = NULL;

#pragma pack(2)
typedef struct
{
  uint8_t event;
  uint8_t data[LORA_MAX_RECV_LENGTH];
  uint16_t length;
  int16_t rssi;
}lora_msg_t;
#pragma pack()

static void OnTxDone( void ){
  Radio.Sleep();
  lora_msg_t lora_msg;
  lora_msg.event = TX_DONE;
  xQueueSend( lora_rq, ( void * ) &lora_msg, ( TickType_t ) 0 );
}

#define CRC_MARK   (1u)
static void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr ){
  Radio.Sleep();
  lora_msg_t lora_msg;
  
  if((size > LORA_MAX_RECV_LENGTH) || (*payload != (size-1)))
  {
    lora_msg.event = RX_ERROR;
  }
  else
  {
#if CRC_MARK == 1
    uint8_t crc = payload[size - 1];
    if(crc != crc_compute(payload, size-1))
    {
      lora_msg.event = RX_ERROR;
      xQueueSend( lora_rq, ( void * ) &lora_msg, ( TickType_t ) 0 );
      return;
    }
#endif
    osel_memset((uint8_t *)&lora_msg, 0, sizeof(lora_msg_t));
    lora_msg.event = RX_DONE;
    osel_memcpy(lora_msg.data, payload, size);
    lora_msg.length = size;
    lora_msg.rssi = rssi;
  }
  xQueueSend( lora_rq, ( void * ) &lora_msg, ( TickType_t ) 0 );
}

static void OnTxTimeout( void )
{
  Radio.Sleep();
  lora_msg_t lora_msg;
  lora_msg.event = TX_TIME_OUT;
  xQueueSend( lora_rq, ( void * ) &lora_msg, ( TickType_t ) 0 );
}

static void OnRxTimeout( void ){
  Radio.Sleep();
  lora_msg_t lora_msg;
  lora_msg.event = RX_TIME_OUT;
  xQueueSend( lora_rq, ( void * ) &lora_msg, ( TickType_t ) 0 );
}

static void OnRxError( void ){
  Radio.Sleep();
  lora_msg_t lora_msg;
  lora_msg.event = RX_ERROR;
  xQueueSend( lora_rq, ( void * ) &lora_msg, ( TickType_t ) 0 );
}

static void lora_config(void)
{
  RadioEvents.TxDone = OnTxDone;
  RadioEvents.RxDone = OnRxDone;
  RadioEvents.TxTimeout = OnTxTimeout;
  RadioEvents.RxTimeout = OnRxTimeout;
  RadioEvents.RxError = OnRxError;
  Radio.Init( &RadioEvents );
  
  device_info_t device_info = read_device_info_ram();
  uint32_t rf = RF_FREQUENCY + (RF_OFFSET * device_info.rf_channel);
  Radio.SetChannel( rf );
  //    static uint8_t version;
  //    version = Radio.Read(REG_LR_VERSION);
}

static void loraTask( void *pvParameters )
{    
  lora_msg_t lora_msg;
  while(TRUE)
  {
    if (xQueueReceive(lora_rq,&lora_msg, portMAX_DELAY))
    {
      if(lora_msg.event == RX_DONE)
      {
        if(lora_recv_cb!=NULL)
        {
          lora_recv_cb(lora_msg.data , lora_msg.length, lora_msg.rssi);
        }
        else
        {
          Radio.Sleep();
        }
      }
      else if(lora_msg.event == TX_DONE )
      {
        lora_recv(lora_recv_cb);
      }
      else
      {
        lora_recv(lora_recv_cb);
      }
    }
  }
}


void lora_send(uint8_t *pbuf,uint8_t length)
{
  device_info_t device_info = read_device_info();
  Radio.SetTxConfig( MODEM_LORA, device_info.rf_power, 0, device_info.bandwidth,
                    device_info.speading_factor, LORA_CODINGRATE,
                    LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                    (bool)TRUE, (bool)FALSE, 0, LORA_IQ_INVERSION_ON, 2000000 );
  Radio.Send( pbuf, length );
}

void lora_recv(lora_recv_cb_t recv_cb)
{
  lora_recv_cb = recv_cb;
  device_info_t device_info = read_device_info();
  Radio.SetRxConfig( MODEM_LORA, device_info.bandwidth, device_info.speading_factor,
                    LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                    LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                    0, (bool)TRUE, (bool)FALSE, 0, LORA_IQ_INVERSION_ON, (bool)TRUE );
  Radio.Rx(RX_TIMEOUT_VALUE);
}

void lora_app_init(void)
{
  lora_config();
  lora_rq = xQueueCreate( 2, sizeof( lora_msg_t ) );
  if(pdTRUE != xTaskCreate( loraTask, "loraTask", loraTask_STACK_SIZE, NULL, loraTask_PRIORITY, NULL ))
  {
    DBG_ASSERT(FALSE __DBG_LINE);
  }
}