#pragma once

/*!< 无线频率 */
#define RF_OFFSET                                   (400000)
#if defined( USE_BAND_433 )
#define RF_FREQUENCY                                (470000000) // Hz
#elif defined( USE_BAND_780 )
#define RF_FREQUENCY                                780000000 // Hz
#elif defined( USE_BAND_868 )
#define RF_FREQUENCY                                868000000 // Hz
#elif defined( USE_BAND_915 )
#define RF_FREQUENCY                                915000000 // Hz
#else
    #error "Please define a frequency band in the compiler options."
#endif

/*!< 发射功率 */
#define TX_OUTPUT_POWER                             17        // dBm

#if defined( USE_MODEM_LORA )
#define LORA_BANDWIDTH                              2         /*!< [0: 125 kHz,1: 250 kHz,2: 500 kHz,3: Reserved] */
#define LORA_SPREADING_FACTOR                       7         /*!< [SF7..SF12] */
#define LORA_CODINGRATE                             1         /*!< [1: 4/5,2: 4/6,3: 4/7,4: 4/8] */
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         0x20         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  (bool)FALSE
#define LORA_IQ_INVERSION_ON                        (bool)FALSE
#elif defined( USE_MODEM_FSK )
#define FSK_FDEV                                    25e3      // Hz
#define FSK_DATARATE                                50e3      // bps
#define FSK_BANDWIDTH                               50e3      // Hz
#define FSK_AFC_BANDWIDTH                           83.333e3  // Hz
#define FSK_PREAMBLE_LENGTH                         5         // Same for Tx and Rx
#define FSK_FIX_LENGTH_PAYLOAD_ON                   (bool)FALSE
#else
    #error "Please define a modem in the compiler options."
#endif

#define RX_TIMEOUT_VALUE                            60*1000*1000