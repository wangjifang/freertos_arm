/**
* @brief       : 
*
* @file        : debug.c
* @author      : gang.cheng
*
* Version      : v0.0.1
* Date         : 2015/5/7
* Change Logs  :
*
* Date        Version      Author      Notes
* 2015/5/7    v0.0.1      gang.cheng    first version
*/
#include "debug.h"

#define DEBUG_PREFIX_LEN                (128u)      // log日志的前缀
#define DEBUG_BUF_SQQ_LEN               (1024u)
#define DEBUG_PRINTF_ID                 (UART_1)

typedef uint8_t debug_entry_t;

uint8_t global_debug_level;

static uint16_t dbg_line;


void debug_init(uint8_t debug_lev)
{  
    global_debug_level = debug_lev;
}

#if DEBUG_INFO_PRINT_EN > 0
void debug_log(uint8_t dbg_lev, const char *fn, uint16_t line, ...)
{
    //    if(!(global_debug_level&dbg_lev))
    //    {
    //        return;
    //    }
    //    
    //    uint8_t dbg_prefix_buf[DEBUG_PREFIX_LEN];
    //	char *pbuf = (char *)&dbg_prefix_buf[0];
    //	(void)memset(dbg_prefix_buf, 0, DEBUG_PREFIX_LEN);
    //    
    //	switch (dbg_lev)
    //	{
    //	case DBG_LEVEL_TRACE:
    //		(void)tfp_sprintf(pbuf, "[%s, %u][TRACE] ", fn, line);
    //		break;
    //        
    //	case DBG_LEVEL_INFO:
    //        (void)tfp_sprintf(pbuf, "[%s, %u][INFO ] ", fn, line);
    //		break;
    //        
    //	case DBG_LEVEL_WARNING:
    //		(void)tfp_sprintf(pbuf, "[%s, %u][WARN ] ", fn, line);
    //		break;
    //        
    //	case DBG_LEVEL_ERROR:
    //		(void)tfp_sprintf(pbuf, "[%s, %u][ERROR] ", fn, line);
    //		break;
    //        
    //	default:
    //		break;
    //	}
    //	
    //	pbuf += strlen((char_t *)dbg_prefix_buf);
    //    
    //	va_list args;
    //	va_start(args, line);
    //	const char_t *fmt = va_arg(args, const char_t*);
    //    tfp_vsprintf(pbuf, (char *)fmt, args);
    //	va_end(args);
    //    
    //    xSemaphoreTake(debug_mutex, portMAX_DELAY);
    //    uint8_t data_len = strlen((char_t *)dbg_prefix_buf);
    //    for(uint8_t i=0;i<data_len;i++)
    //    {
    //        //@todo: 后期打开串口打印接口
    ////        uart_send_char(DEBUG_PRINTF_ID, dbg_prefix_buf[i]);
    //    }
    //    xSemaphoreGive(debug_mutex);
}
#endif

uint16_t debug_info_get(void)
{
    uint16_t line;
    line = dbg_line;
    return line;
}

void debug_info_clr(void)
{
    dbg_line = 0xFFFF;
}


#if DEBUG_INFO_PRINT_EN > 0
void DBG_ASSERT(uint8_t cond _DBG_LINE_)
{
    do 
    {
        if((cond) == FALSE)
        {
            portDISABLE_INTERRUPTS();
            while(1);
        }
    } while(__LINE__ == -1);
}

#else
void DBG_ASSERT(uint8_t cond _DBG_LINE_)                        		       
{  
    do
    {
        if((cond) == FALSE)
        {
            dbg_line = line;
            while(1)
            {
                NVIC_SystemReset();
            }
        }
    } while(__LINE__ == -1);
}

#endif
