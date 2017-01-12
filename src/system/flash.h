#pragma once

void flash_read(uint32_t addr,uint8_t *buffer,uint16_t length);

void eeprom_readbytes(uint16_t addr,uint8_t *buffer,uint16_t length);

void eeprom_writebytes(uint16_t addr,uint8_t *buffer,uint16_t length);  