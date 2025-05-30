#ifndef _BL24C02_H_
#define _BL24C02_H_

#include "i2c.h"

#define EEPROM_ADDR 0xA0

void BL24C02_Init(void);
uint8_t BL24C02_Write_Byte(uint8_t daddr, uint8_t data);
uint8_t BL24C02_Read_Byte(uint8_t daddr);
uint8_t BL24C02_Write_Bytes(uint8_t daddr_start, uint8_t *data, int len);
uint8_t BL24C02_Read_Bytes(uint8_t daddr_start, uint8_t *data, int len);
#endif
