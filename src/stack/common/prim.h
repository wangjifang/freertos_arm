/**
 * @brief       :
 *
 * @file        : prim.h
 * @author      : gang.cheng
 *
 * Version      : v0.0.1
 * Date         : 2015/5/7
 * Change Logs  :
 *
 * Date        Version      Author      Notes
 * 2015/5/7    v0.0.1      gang.cheng    first version
 */

#ifndef __PRIM_H__
#define __PRIM_H__

#include "pbuf.h"


typedef struct _a2n_data_request_
{
    uint64_t dst_addr;
    uint8_t nsdu_length;
    void *nsdu;
} a2n_data_request_t;

typedef struct _n2a_data_confirm_
{
    nwk_id_t src_addr;
    bool status;
    uint8_t nsdu_length;
    void *nsdu;
} n2a_data_confirm_t;

typedef struct _n2a_data_indication_
{
    nwk_id_t src_addr;
    nwk_id_t dst_addr;
    uint8_t nsdu_length;
    int8_t rssi;
    void *nsdu;
} n2a_data_indication_t;

typedef union
{
    a2n_data_request_t a2n_data_request_arg;
    n2a_data_confirm_t n2a_data_confirm_arg;
    n2a_data_indication_t n2a_data_indication_arg;
} nwk_prim_arg_t;

typedef struct _n2m_data_request_
{
    uint64_t dst_addr;
    uint64_t src_addr;
    uint8_t msdu_length;
    void *msdu;

    uint8_t src_mode : 4,
            dst_mode : 4;
} n2m_data_request_t;

typedef struct _m2n_data_confirm_
{
    uint64_t dst_addr;
    uint8_t src_mode : 4,
            dst_mode : 4;
    bool status;
    uint8_t msdu_length;
    void *msdu;
} m2n_data_confirm_t;

typedef struct _m2n_data_indication_
{
    uint64_t src_addr;          //收到的数据帧的mac源地址
    uint64_t dst_addr;          //收到的数据帧的mac目的地址

    uint8_t src_mode : 4,
            dst_mode : 4;

    uint8_t msdu_length;        //mac层给nwk层的数据载荷长度
    void *msdu;                 //mac层给nwk层的数据载荷指针
} m2n_data_indication_t;


typedef struct
{
    uint8_t status;
} m2n_assoc_confirm_t;

typedef union
{
    n2m_data_request_t n2m_data_request_arg;            //nwk层给mac层发数据
    m2n_data_confirm_t m2n_data_confirm_arg;
    m2n_data_indication_t m2n_data_indication_arg;      //mac层给nwk层发数据
    m2n_assoc_confirm_t m2n_assoc_confirm_arg;
} mac_prim_arg_t;


typedef union
{
    mac_prim_arg_t mac_prim_arg;        //mac层原语参数
    nwk_prim_arg_t nwk_prim_arg;        //nwk层原语参数
} prim_arg_t;


typedef struct
{
    prim_arg_t prim_arg;
    pbuf_t *pbuf;
} prim_args_t;


#endif

