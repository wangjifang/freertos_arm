#ifndef __OSEL_ARCH_H__
#define __OSEL_ARCH_H__

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "lib.h"

#define hal_int_state_t         char
#define HAL_ENTER_CRITICAL(s)   s=0;taskENTER_CRITICAL()
#define HAL_EXIT_CRITICAL(s)    s=s;taskEXIT_CRITICAL()

#define osel_memset             memset1
#define osel_memcpy             memcpy1
#define osel_memcpyr            memcpyr
#define osel_memcmp             memcmp1
#define osel_mem_alloc          pvPortMalloc

#define osel_delay              vTaskDelay

#define RAND_LOCAL_MAX 2147483647L
static uint32_t next = 1;

typedef enum
{
	MSG_LOW_PRIO,
	MSG_HIGH_PRIO,
} osel_eblock_prio_t;

typedef portBASE_TYPE osel_signal_t;

typedef void *		osel_param_t;

typedef struct
{
    osel_signal_t event;
    osel_param_t param;
} osel_event_t;

static inline void memset1( uint8_t *dst, uint8_t value, uint16_t size )
{
    while( size-- )
    {
        *dst++ = value;
    }
}

static inline int32_t rand1( void )
{
    return ( ( next = next * 1103515245L + 12345L ) % RAND_LOCAL_MAX );
}

static inline void srand1( uint32_t seed )
{
    next = seed;
}
// Standard random functions redefinition end

static inline int32_t randr( int32_t min, int32_t max )
{
    return ( int32_t )rand1( ) % ( max - min + 1 ) + min;
}

static inline void memcpy1( uint8_t *dst, const uint8_t *src, uint16_t size )
{
    while( size-- )
    {
        *dst++ = *src++;
    }
}

static inline void memcpyr( uint8_t *dst, const uint8_t *src, uint16_t size )
{
    dst = dst + ( size - 1 );
    while( size-- )
    {
        *dst-- = *src++;
    }
}

static inline bool_t memcmp1(void *const dst, const void *const src, uint16_t len)
{
    DBG_ASSERT(dst != NULL __DBG_LINE);
    DBG_ASSERT(src != NULL __DBG_LINE);
    uint8_t *tmp = (uint8_t *)dst;
    uint8_t *s = (uint8_t *)src;
    while (len--)
    {
        if (*tmp++ != *s++)
        {
            return FALSE;
        }
    }
    return TRUE;
}
#endif
