/**
 * @brief       : 
 *
 * @file        : stack.h
 * @author      : gang.cheng
 *
 * Version      : v0.0.1
 * Date         : 2015/5/7
 * Change Logs  :
 *
 * Date        Version      Author      Notes
 * 2015/5/7    v0.0.1      gang.cheng    first version
 */

#ifndef __STACK_H
#define __STACK_H

#include "./common/pbuf.h"
#include "./common/sbuf.h"
#include "./common/prim.h"

typedef struct
{
    uint32_t event;
    void *param;
} esn_msg_t;

void stack_init(void);

/**
 * @}
 */
#endif
