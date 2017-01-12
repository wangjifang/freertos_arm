/**
* @brief       : 
*
* @file        : data_type_def.h
* @author      : gang.cheng
*
* Version      : v0.0.1
* Date         : 2015/5/7
* Change Logs  :
*
* Date        Version      Author      Notes
* 2015/5/7    v0.0.1      gang.cheng    first version
*/

#ifndef __DATA_TYPE_DEF_H__
#define __DATA_TYPE_DEF_H__

#define DEBUG_PRINT             (0u)	//串口打印

#define HAVE_LONG_LONG          (1u)
#define IS_LITTLE_ENDIAN        (1u)

#include <stdint.h>

/* DATA TYPES (Compiler Specific) */
typedef unsigned char           bool_t;  	/* Boolean type */
typedef char                    char_t;
typedef float                   fp32_t;
typedef double                  fp64_t;

typedef uint16_t                 nwk_id_t;

#if HAVE_LONG_LONG > 0
//typedef signed long long        int64_t;
//typedef unsigned long long      uint64_t;
#else
typedef struct
{
#if IS_LITTLE_ENDIAN > 0
    uint32_t lo;
    uint32_t hi;
#else
    uint32_t hi;
    uint32_t lo;
#endif
} int64_t;
typedef int64_t uint64_t;
#endif

#ifndef __INLINE
#define __INLINE
#endif

/**
* STANDARD BITS
*/
#ifndef BIT0
#define BIT0                    (0x01u)
#define BIT1                    (0x02u)
#define BIT2                    (0x04u)
#define BIT3                    (0x08u)
#define BIT4                    (0x10u)
#define BIT5                    (0x20u)
#define BIT6                    (0x40u)
#define BIT7                    (0x80u)
#endif

#if DEBUG_PRINT > 0
#include <stdio.h>
#define PRINTF(...)              printf(__VA_ARGS__)
#else
#define PRINTF(...)              
#endif

#ifndef st
#define st(x)                   do { x } while (__LINE__ == -1)
#endif

#ifndef BV
#define BV(n)                   (1 << (n))
#endif

#ifndef BM
#define BM(n)                   (1 << (n))
#endif

#ifndef BF
#define BF(x,b,s)               (((x) & (b)) >> (s))
#endif

#ifndef MIN
#define MIN(n,m)                (((n) < (m)) ? (n) : (m))
#endif

#ifndef MAX
#define MAX(n,m)                (((n) < (m)) ? (m) : (n))
#endif

#ifndef ABS
#define ABS(n)                  (((n) < 0) ? -(n) : (n))
#endif

#define ARRAY_LEN(arr)          (sizeof(arr))/(sizeof(arr[0]))

#define HI_UINT16(a)            (((uint16_t)(a) >> 8) & 0xFF)
#define LO_UINT16(a)            ((uint16_t)(a) & 0xFF)

#define HI_1_UINT32(a)            (((uint32_t)(a) >> 24) & 0xFF)
#define HI_2_UINT32(a)            (((uint32_t)(a) >> 16) & 0xFF)
#define HI_3_UINT32(a)            (((uint32_t)(a) >> 8) & 0xFF)
#define HI_4_UINT32(a)            ((uint32_t)(a) & 0xFF)

#define LO_1_UINT8(a)            (uint8_t)((a)& 0xFF)
#define LO_2_UINT8(a)            (uint8_t)(((a)& 0xFF00) >> 8)
#define LO_3_UINT8(a)            (uint8_t)(((a)& 0xFF0000) >> 16)
#define LO_4_UINT8(a)            (uint8_t)(((a)& 0xFF000000) >> 24)

#define S2B_UINT32(a)         \
(((uint32_t)(a) & 0xFF000000)  >> 24)\
    + (((uint32_t)(a) & 0x00FF0000)  >> 8)\
        + (((uint32_t)(a) & 0x0000FF00)  << 8)\
            + (((uint32_t)(a) & 0x000000FF)  << 24)
                
                
#define S2B_UINT16(a)           ((((uint16_t)(a) & 0xFF00) >> 8) \
                + (((uint16_t)(a) & 0x00FF) << 8))

#define BUILD_UINT16(loByte, hiByte) \
((uint16_t)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))

#ifndef _NOP
#define  _NOP()                 asm("NOP");
#endif

#define WHILE(exp)                                  \
do                                              \
    {                                               \
        uint32_t i = 0;								\
            while((exp))                                \
                {                                           \
                    if (i++ >700000)                        \
                        {                                       \
                            DBG_ASSERT(FALSE __DBG_LINE);		\
                        }                                       \
                }                                           \
    } while(__LINE__ == -1)
        
#endif
        