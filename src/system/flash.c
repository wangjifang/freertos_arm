#include "board.h"
#include "stm32l1xx_flash.h"
#define EEPROM_BASE_ADDR    0x08080000    
#define EEPROM_BYTE_SIZE    0x0FFF
#define PEKEY1  0x89ABCDEF      //FLASH_PEKEYR  
#define PEKEY2  0x02030405      //FLASH_PEKEYR  

void flash_read(uint32_t addr,uint8_t *buffer,uint16_t length)  
{         
    uint8_t *flash_addr = (uint8_t *)addr;  
    while(length--)
    {
        *buffer++ = *flash_addr++;
    }
} 

void eeprom_readbytes(uint16_t addr,uint8_t *buffer,uint16_t length)  
{  
    uint8_t *eeprom_addr = (uint8_t *)(EEPROM_BASE_ADDR + addr);  
    while(length--){  
        *buffer++ = *eeprom_addr++;  
    }     
}  

void eeprom_writebytes(uint16_t addr,uint8_t *buffer,uint16_t length)  
{  
    uint32_t eeprom_addr = (uint32_t)(EEPROM_BASE_ADDR + addr);  
    DATA_EEPROM_Unlock();
    while(length--)
    {  
        __disable_irq( );  
        DATA_EEPROM_FastProgramByte(eeprom_addr++ ,*buffer++);
        __enable_irq( ); 
    }  
    DATA_EEPROM_Lock();
}  