/*
/ _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
\____ \| ___ |    (_   _) ___ |/ ___)  _ \
_____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
(C)2013 Semtech

Description: Target board general functions implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
#include "board.h"

/*!
* Unique Devices IDs register set ( STM32L1xxx )
*/
#define         ID1                                 ( 0x1FF80050 )
#define         ID2                                 ( 0x1FF80054 )
#define         ID3                                 ( 0x1FF80064 )

/*!
* LED GPIO pins objects
*/
Gpio_t Led1;
Gpio_t Led2;
Gpio_t Led3;

/*!
* Hex coder selector GPIO pins objects
*/
Gpio_t Sel1;
Gpio_t Sel2;
Gpio_t Sel3;
Gpio_t Sel4;

#if defined( USE_DEBUG_PINS )
Gpio_t DbgPin1;
Gpio_t DbgPin2;
Gpio_t DbgPin3;
Gpio_t DbgPin4;
#endif

I2c_t I2c;
Uart_t Uart[2];
/*!
* Initializes the unused GPIO to a know status
*/
static void BoardUnusedIoInit( void );

/*!
* Flag to indicate if the MCU is Initialized
*/
static bool McuInitialized = FALSE;


void BoardInitPeriph( void )
{
    
}

void BoardInitMcu( void )
{
    if( McuInitialized == FALSE )
    {
        // We use IRQ priority group 4 for the entire project
        // When setting the IRQ, only the preemption priority is used
        NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
        
        // Disable Systick
        SysTick->CTRL  &= ~SysTick_CTRL_TICKINT_Msk;    // Systick IRQ off 
        SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;            // Clear SysTick Exception pending flag
        
//        SpiInit( &SX1278.Spi, RADIO_MOSI, RADIO_MISO, RADIO_SCLK, NC );
//        SX1278IoInit( );
        
        Uart[UART_1].timeout_callback = uart2_timeout_cb;
        UartInit(&Uart[UART_1],UART_1,UART1_TX,UART1_RX);
        UartConfig(&Uart[UART_1],RX_TX,38400,UART_8_BIT,UART_1_STOP_BIT,NO_PARITY,NO_FLOW_CTRL);
        
#if( LOW_POWER_MODE_ENABLE )
        TimerSetLowPowerEnable( TRUE );
#else
        TimerSetLowPowerEnable( FALSE );
#endif
        
        if( TimerGetLowPowerEnable( ) == TRUE )
        {
            //            RtcInit( );
        }
        else
        {
            TimerHwInit( );
        }
        McuInitialized = TRUE;
    }
    BoardUnusedIoInit();
}

void BoardDeInitMcu( void )
{
   
}

uint32_t BoardGetRandomSeed( void )
{
    return ( ( *( uint32_t* )ID1 ) ^ ( *( uint32_t* )ID2 ) ^ ( *( uint32_t* )ID3 ) );
}

void BoardGetUniqueId( uint8_t *id )
{
    id[7] = ( ( *( uint32_t* )ID1 )+ ( *( uint32_t* )ID3 ) ) >> 24;
    id[6] = ( ( *( uint32_t* )ID1 )+ ( *( uint32_t* )ID3 ) ) >> 16;
    id[5] = ( ( *( uint32_t* )ID1 )+ ( *( uint32_t* )ID3 ) ) >> 8;
    id[4] = ( ( *( uint32_t* )ID1 )+ ( *( uint32_t* )ID3 ) );
    id[3] = ( ( *( uint32_t* )ID2 ) ) >> 24;
    id[2] = ( ( *( uint32_t* )ID2 ) ) >> 16;
    id[1] = ( ( *( uint32_t* )ID2 ) ) >> 8;
    id[0] = ( ( *( uint32_t* )ID2 ) );
}

uint8_t BoardGetBatteryLevel( void )
{
    uint8_t batteryLevel = 0;
    uint8_t i, result = 0;
    uint32_t tmpreg = 0;
    
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_PWR, ENABLE );
    
    PWR_PVDCmd( ENABLE ); // activate the voltage reference
    DelayMs( 1 );
    
    tmpreg = PWR->CR; // get the CR register for a read-modify-write
    
    for( i = 0; i <= 6; i++ ) 
    {
        PWR->CR = ( ( tmpreg & 0xFFFFFF1F ) | ( i << 5 ) ); // set PVD level from 0 to 6
        DelayMs( 1 );
        if( PWR_GetFlagStatus( PWR_FLAG_PVDO ) == 1 )
        {
            result |= 0x01 << i; // use 'result' as a bit array
        }
    }
    PWR_PVDCmd( DISABLE ); // shut down voltage reference
    
    switch( result ) { // transcribe result and detect invalid codes
    case 0x7F: 
        batteryLevel = 0x1F;    // Vbat < 1.9V
        break;
    case 0x7E: 
        batteryLevel = 0x3F;    // 1.9 < Vbat < 2.1
        break;
    case 0x7C: 
        batteryLevel = 0x5F;    // 2.1 < Vbat < 2.3
        break;
    case 0x78: 
        batteryLevel = 0x7F;    // 2.3 < Vbat < 2.5
        break;
    case 0x70: 
        batteryLevel = 0x9F;    // 2.5 < Vbat < 2.7
        break;
    case 0x60: 
        batteryLevel = 0xBF;    // 2.7 < Vbat < 2.9
        break;
    case 0x40: 
        batteryLevel = 0xDF;    // 2.9 < Vbat < 3.1
        break;
    case 0x00: 
        batteryLevel = 0xFE;    // Vbat > 3.1V
        break;
    default:   
        batteryLevel = 0xFF;    // Fail
        break;
    }
    return batteryLevel;
}

static void BoardUnusedIoInit( void )
{
    
}
