/*
/ _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
\____ \| ___ |    (_   _) ___ |/ ___)  _ \
_____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
(C)2013 Semtech

Description: Implements the generic UART driver

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
#include "board.h"
#include "uart-board.h"
#include "stack.h"
#if defined( USE_USB_CDC )
#include "uart-usb-board.h"
#endif

#include "uart.h"

#include <stdarg.h>
/*!
* Number of times the UartPutBuffer will try to send the buffer before
* returning ERROR
*/
#define TX_BUFFER_RETRY_COUNT                           10
#define UART_NUM                                        2

uint8_t tx_buf[UART_SIZE];
uint8_t rx_buf[UART_SIZE];

#define uartTask_STACK_SIZE          ( configMINIMAL_STACK_SIZE * 5 )
#define uartTask_PRIORITY            ( tskIDLE_PRIORITY + 3 )

static QueueHandle_t uart_rq;
static uart_interupt_cb_t uart_interrupt_cb = NULL;

#define UART_MSG                    (30)
typedef enum
{
    UART_RECV,
}UART_MSG_E;

void uart2_timeout_cb(TimerHandle_t timer)
{
    xTimerStop(timer, 0);    
    esn_msg_t esn_msg;
    esn_msg.event = (UART_RECV << 8) | UART_MSG ;
    xQueueSend(uart_rq, &esn_msg, portMAX_DELAY);
}

static void uart_callback(void *obj, UartNotifyId_t id)
{
    Uart_t *temp = (Uart_t *)obj;
    if(id == UART_NOTIFY_RX)
    {
        xTimerReset(temp->start_timer, 0);
    }
}

void uart_recv_disable(uint8_t uart_id)
{
    switch(uart_id)
    {
    case UART_1:
	USART_ITConfig( USART1, USART_IT_RXNE, DISABLE );
	break;
    case UART_2:
	USART_ITConfig( USART2, USART_IT_RXNE, DISABLE );
	break;
    }
}

void uart_recv_enable(uint8_t uart_id)
{
    switch(uart_id)
    {
    case UART_1:
	USART_ITConfig( USART1, USART_IT_RXNE, ENABLE );
	break;
    case UART_2:
	USART_ITConfig( USART2, USART_IT_RXNE, ENABLE );
	break;
    }
}

void uart_send_string(uint8_t uart_id, uint8_t *string, uint16_t length)
{
    if(UART_SIZE > length)
        UartPutBuffer(&Uart[uart_id], string, length );
}

void uart_int_cb_reg(uart_interupt_cb_t cb)
{
    if (cb != NULL)
    {
	uart_interrupt_cb = cb;
    }
}

static void uartTask( void *pvParameters )
{
    esn_msg_t esn_msg;
    uint8_t type;
    while(TRUE)
    {
        if (xQueueReceive(uart_rq,&esn_msg, portMAX_DELAY))
        {
            type = esn_msg.event >> 8;
            switch(type)
            {
            case UART_RECV:
                if (uart_interrupt_cb != NULL)
                {
                    uint8_t ch = 0;
                    while(IsFifoEmpty( &Uart[UART_1].FifoRx ) == FALSE)
                    {
                        ch = FifoPop( &Uart[UART_1].FifoRx );
                        uart_interrupt_cb(UART_1, ch);
                    }
                    while(IsFifoEmpty( &Uart[UART_2].FifoRx ) == FALSE)
                    {
                        ch = FifoPop( &Uart[UART_2].FifoRx );
                        uart_interrupt_cb(UART_2, ch);
                    }
                }
                break;
            }
        }
        //vTaskDelay(200 / portTICK_RATE_MS);
    }
}

void UartInit( Uart_t *obj, uint8_t uartId, PinNames tx, PinNames rx )
{
    uart_rq = xQueueCreate( 2, sizeof( esn_msg_t ) );
    if(pdTRUE != xTaskCreate( uartTask, "uartTask", uartTask_STACK_SIZE, NULL, uartTask_PRIORITY, NULL ))
    {
        DBG_ASSERT(FALSE __DBG_LINE);
    }
    
    char name[] = "uart_timeout_%d";
    sprintf(name,name,uartId);
    obj->start_timer = NULL;
    obj->start_timer = xTimerCreate(name,
                                20 / portTICK_RATE_MS,
                                pdTRUE,
                                NULL,
                                obj->timeout_callback);
    if (obj->start_timer == NULL) 
    {
        DBG_ASSERT(FALSE __DBG_LINE);
    }
    FifoInit(&obj->FifoTx,tx_buf,UART_SIZE);
    FifoInit(&obj->FifoRx,rx_buf,UART_SIZE);
    obj->IrqNotify = uart_callback;
    
    if( obj->IsInitialized == FALSE )
    {
        obj->IsInitialized = TRUE;
        
        if( uartId == UART_USB_CDC )
        {
#if defined( USE_USB_CDC )
            UartUsbInit( obj, uartId, NC, NC );
#endif
        }
        else
        {
            UartMcuInit( obj, uartId, tx, rx );
        }
    }
}

void UartConfig( Uart_t *obj, UartMode_t mode, uint32_t baudrate, WordLength_t wordLength, StopBits_t stopBits, Parity_t parity, FlowCtrl_t flowCtrl )
{
    if( obj->IsInitialized == FALSE )
    {
        // UartInit function must be called first.
        while( 1 );
    }
    if( obj->UartId == UART_USB_CDC )
    {
#if defined( USE_USB_CDC )
        UartUsbConfig( obj, mode, baudrate, wordLength, stopBits, parity, flowCtrl );
#endif
    }
    else
    {
        UartMcuConfig( obj, mode, baudrate, wordLength, stopBits, parity, flowCtrl );
    }
}

void UartDeInit( Uart_t *obj )
{
    obj->IsInitialized = FALSE;
    if( obj->UartId == UART_USB_CDC )
    {
#if defined( USE_USB_CDC )
        UartUsbDeInit( obj );
#endif
    }
    else
    {
        UartMcuDeInit( obj );
    }
}

uint8_t UartPutChar( Uart_t *obj, uint8_t data )
{
    if( obj->UartId == UART_USB_CDC )
    {
#if defined( USE_USB_CDC )
        return UartUsbPutChar( obj, data );
#else
        return 255; // Not supported
#endif
    }
    else
    {
        return UartMcuPutChar( obj, data );
    }
}

uint8_t UartGetChar( Uart_t *obj, uint8_t *data )
{
    if( obj->UartId == UART_USB_CDC )
    {
#if defined( USE_USB_CDC )
        return UartUsbGetChar( obj, data );
#else
        return 255; // Not supported
#endif
    }
    else
    {
        return UartMcuGetChar( obj, data );
    }
}

uint8_t UartPutBuffer( Uart_t *obj, uint8_t *buffer, uint16_t size )
{
    if( obj->UartId == UART_USB_CDC )
    {
#if defined( USE_USB_CDC )
        return UartUsbPutBuffer( obj, buffer, size );
#else
        return 255; // Not supported
#endif
    }
    else
    {
        uint8_t retryCount;
        uint16_t i;
        
        for( i = 0; i < size; i++ )
        {
            retryCount = 0;
            while( UartPutChar( obj, buffer[i] ) != 0 )
            {
                retryCount++;
                
                // Exit if something goes terribly wrong
                if( retryCount > TX_BUFFER_RETRY_COUNT )
                {
                    return 1; // Error
                }
            }
        }
        return 0; // OK
    }
}

uint8_t UartGetBuffer( Uart_t *obj, uint8_t *buffer, uint16_t size, uint16_t *nbReadBytes )
{
    uint16_t localSize = 0;
    
    while( localSize < size )
    {
        if( UartGetChar( obj, buffer + localSize ) == 0 )
        {
            localSize++;
        }
        else
        {
            break;
        }
    }
    
    *nbReadBytes = localSize;
    
    if( localSize == 0 )
    {
        return 1; // Empty
    }
    return 0; // OK
}

void uart_printf (char *fmt, ...)
{
#define CMD_BUFFER_LEN 256
    char buffer[CMD_BUFFER_LEN+1];  // CMD_BUFFER_LEN长度自己定义吧
    uint16_t i = 0;
    
    va_list arg_ptr;
    va_start(arg_ptr, fmt);  
    vsnprintf(buffer, CMD_BUFFER_LEN+1, fmt, arg_ptr);
    while ((i < CMD_BUFFER_LEN) && buffer[i])
    {
        UartPutChar(&Uart[UART_2], (uint8_t)buffer[i++]);
    }
    va_end(arg_ptr);
} 

