/*!
 * [mac.send]
 * LoRaMAC layer send frame
 *
 * \param [IN] app_port
 * \param [IN] buf       数据
 * \param [IN] length    数据长度
 * \param [IN] confirm   是否需要确认[TRUE:需要应答]
 *
 * \retval status          []
 */
#pragma once
#include "board.h"
#include "osel_arch.h"
#include "stack.h"
#include "LoRaMac-api-v3.h"
#include "Comissioning.h"

#define MAC_RSEND   (3u)
#define MAC_EVENT   (BIT2<<8)
typedef enum
{
    MAC_DATA_EVENT = (MAC_EVENT | 1),
} mac_event_enum_t;

typedef struct
{
    uint32_t dev_id;
    uint32_t nwk_id;
    uint8_t nwk_sky[16];
    uint8_t app_sky[16];

    bool IsNetworkJoined;   //Indicates if the MAC layer has already joined a network.
    bool SetAdrOn;          //启用自适应速率
}mac_info_t;

typedef struct {
	bool        (*init)(mac_info_t );
    uint8_t     (*send)(uint8_t , uint8_t *, uint8_t , bool );
    bool        (*can_send)();
    mac_info_t  (*info_look)();
} mac_t;

extern const mac_t mac;


