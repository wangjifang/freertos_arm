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
#ifndef __BOARD_H__
#define __BOARD_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "stm32l1xx.h"
#include "timer.h"
#include "delay.h"
#include "gpio.h"
#include "adc.h"
#include "spi.h"
#include "i2c.h"
#include "uart.h"
#include "flash.h"
#include "radio.h"
#include "sx1278/sx1278.h"
#include "gps.h"
//#include "gps-board.h"
#include "rtc-board.h"
#include "timer-board.h"
#include "sx1278-board.h"
#include "uart-board.h"

#if defined( USE_USB_CDC )
#include "usb-cdc-board.h"
#endif

/*!
 * Define indicating if an external IO expander is to be used
 */
//#define BOARD_IOE_EXT

/*!
 * Generic definition
 */
#ifndef SUCCESS
#define SUCCESS                                     1
#endif

#ifndef FAIL
#define FAIL                                        0  
#endif

/*!
 * Board IO Extender pins definitions
 */
/*!
 * Board MCU pins definitions
 */
#define LED_1                                       PB_9
#define LED_2                                       PB_8
#define LED_3                                       PB_5

#define RADIO_RESET                                 PA_8

#define RADIO_MOSI                                  PB_15
#define RADIO_MISO                                  PB_14
#define RADIO_SCLK                                  PB_13
#define RADIO_NSS                                   PB_12

#define RADIO_DIO_0                                 PA_7
#define RADIO_DIO_1                                 PB_0
#define RADIO_DIO_2                                 PB_1
#define RADIO_DIO_3                                 PB_11
#define RADIO_DIO_4                                 PA_4
#define RADIO_DIO_5                                 PB_10

#define RADIO_ANT_SWITCH_LF                         PC_0
#define RADIO_ANT_SWITCH_HF                         PC_1

#define OSC_LSE_IN                                  PC_14
#define OSC_LSE_OUT                                 PC_15

#define OSC_HSE_IN                                  PH_0
#define OSC_HSE_OUT                                 PH_1

#define NC_1                                        PC_13
#define NC_2                                        PC_3
#define NC_3                                        PB_1
#define NC_4                                        PA_10

#define J1_1                                        PB_12
#define J1_2                                        PB_15
#define J1_3                                        PB_14
#define J1_4                                        PB_13

#define J2_2                                        PB_7
#define J2_3                                        PB_6

#define SEL_1                                       PA_8
#define SEL_2                                       PA_9
#define SEL_3                                       PC_6
#define SEL_4                                       PC_7

#define USB_TX                                      PA_2
#define USB_RX                                      PA_3
#define UART1_TX                                    PA_9            
#define UART1_RX                                    PA_10

#define BOOT_1                                      PB_2

#define JTAG_TMS                                    PA_13
#define JTAG_TCK                                    PA_14
#define JTAG_TDI                                    PA_15
#define JTAG_TDO                                    PB_3
#define JTAG_NRST                                   PB_4

#define NC_7                                        PB_0
#define NC_8                                        PC_8
#define NC_9                                        PC_9
#define NC_10                                       PC_10
#define NC_11                                       PC_11
#define NC_12                                       PC_12
#define NC_13                                       PD_2

#define I2C_SCL                                     PB_10
#define I2C_SDA                                     PB_11

/*!
 * LED GPIO pins objects
 */
extern Gpio_t IrqMpl3115;
extern Gpio_t IrqMag3110;
extern Gpio_t GpsPowerEn;
extern Gpio_t NcIoe3;
extern Gpio_t NcIoe4;
extern Gpio_t NcIoe5;
extern Gpio_t NcIoe6;
extern Gpio_t NcIoe7;
extern Gpio_t NIrqSX9500;
extern Gpio_t Irq1Mma8451;
extern Gpio_t Irq2Mma8451;
extern Gpio_t TxEnSX9500;
extern Gpio_t Led1;
extern Gpio_t Led2;
extern Gpio_t Led3;


/*!
 * Debug GPIO pins objects
 */
#if defined( USE_DEBUG_PINS )
extern Gpio_t DbgPin1;
extern Gpio_t DbgPin2;
extern Gpio_t DbgPin3;
extern Gpio_t DbgPin4;
#endif

/*!
 * MCU objects
 */
extern Adc_t Adc;
extern I2c_t I2c;
extern Uart_t Uart[2];
#if defined( USE_USB_CDC )
extern Uart_t UartUsb;
#endif

extern Gpio_t GpsPps;
extern Gpio_t GpsRx;
extern Gpio_t GpsTx;

/*!
 * \brief Initializes the target board peripherals.
 */
void BoardInitMcu( void );

/*!
 * \brief Initializes the boards peripherals.
 */
void BoardInitPeriph( void );

/*!
 * \brief De-initializes the target board peripherals to decrease power
 *        consumption.
 */
void BoardDeInitMcu( void );

/*!
 * \brief Get the current battery level
 *
 * \retval value  battery level ( 0: very low, 254: fully charged )
 */
uint8_t BoardGetBatteryLevel( void );

/*!
 * Returns a pseudo random seed generated using the MCU Unique ID
 *
 * \retval seed Generated pseudo random seed
 */
uint32_t BoardGetRandomSeed( void );

/*!
 * \brief Gets the board 64 bits unique ID 
 *
 * \param [IN] id Pointer to an array that will contain the Unique ID
 */
void BoardGetUniqueId( uint8_t *id );

#endif // __BOARD_H__
