#include "mac.h"
#define macTask_STACK_SIZE          ( configMINIMAL_STACK_SIZE * 10 )
#define macTask_PRIORITY            ( tskIDLE_PRIORITY + 2 )

static mac_info_t mac_info;
static LoRaMacCallbacks_t LoRaMacCallbacks;
static QueueHandle_t mac_queue = NULL;
/*!
* \brief Function to be executed on MAC layer event
*/
static void OnMacEvent( LoRaMacEventFlags_t *flags, LoRaMacEventInfo_t *info )
{
    if( flags->Bits.JoinAccept == 1 )
    {
        mac_info.IsNetworkJoined = TRUE;
    }
    else
    {

    }
}

static void macTask( void *pvParameters )
{
    esn_msg_t esn_msg;
    while(1)
    {
        if (xQueueReceive(mac_queue, &esn_msg, portMAX_DELAY))
        {
            switch (esn_msg.event)
            {
            case MAC_DATA_EVENT:
                break;
            default:
                break;
            }
        }
    }
}

static bool init(mac_info_t mac_info_handle)
{
    LoRaMacCallbacks.MacEvent = OnMacEvent;
    LoRaMacCallbacks.GetBatteryLevel = BoardGetBatteryLevel;
    osel_memcpy((uint8_t *)&mac_info, (uint8_t *)&mac_info_handle, sizeof(mac_info_t));

    LoRaMacInit( &LoRaMacCallbacks );
    LoRaMacInitNwkIds( mac_info.nwk_id, mac_info.dev_id, mac_info.nwk_sky, mac_info.app_sky );
    LoRaMacSetAdrOn(mac_info.SetAdrOn);
    
    if(pdTRUE != xTaskCreate( macTask, "macTask", macTask_STACK_SIZE, NULL, macTask_PRIORITY, NULL ))
        DBG_ASSERT(FALSE __DBG_LINE);

    mac_queue = xQueueCreate(10, sizeof(esn_msg_t));
    if(NULL == mac_queue)
        DBG_ASSERT(FALSE __DBG_LINE);

    if(!mac_info.IsNetworkJoined)
    {
        //todo:LoRaMacJoinReq
    }
    return TRUE;
}

static mac_info_t info_look(void)
{
    return mac_info;
}

static uint8_t send(uint8_t app_port, uint8_t *buf, uint8_t length, bool confirm)
{
    uint8_t state = LORAMAC_STATUS_OK;
    if(!mac_info.IsNetworkJoined)
        return LORAMAC_STATUS_NO_NETWORK_JOINED;
    
    if(FALSE == confirm)
    {
        state = LoRaMacSendFrame( app_port, buf, length );
    }
    else
    {
        state = LoRaMacSendConfirmedFrame( app_port, buf, length, MAC_RSEND );
    }
    return state;
}

static inline bool can_send(void)
{
    extern uint32_t LoRaMacState;
    if( ( ( LoRaMacState & 0x00000001 ) == 0x00000001 ) ||
        ( ( LoRaMacState & 0x00000010 ) == 0x00000010 ) )
    {
        return FALSE;
    }
    if(FALSE == mac_info.IsNetworkJoined)
    {
        return FALSE;
    }
    return TRUE;
}

const mac_t mac =
{
    init,
    send,
    can_send,
    info_look,
};
