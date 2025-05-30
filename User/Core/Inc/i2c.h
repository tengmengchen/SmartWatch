#ifndef _I2C_H_
#define _I2C_H_

#include "main.h"
#include "stm32f411xe.h"
#include "delay.h"

typedef struct {
    GPIO_TypeDef *IIC_SDA_PORT;
	GPIO_TypeDef *IIC_SCL_PORT;
	uint16_t IIC_SDA_PIN;
	uint16_t IIC_SCL_PIN;
    uint8_t addr;

} iic_bus_t;

void IIC_Init(iic_bus_t *bus);
void IICSendAck(iic_bus_t *bus);
void IICSendNotAck(iic_bus_t *bus);
void IICStart(iic_bus_t *bus);
void IICStop(iic_bus_t *bus);
uint8_t i2c_read_bit(iic_bus_t *bus);
uint8_t i2c_read_byte(iic_bus_t *bus);
int i2c_read_bytes(iic_bus_t *bus, uint8_t *data, int len);
void i2c_write_bit(iic_bus_t *bus, uint8_t data);
void i2c_write_byte(iic_bus_t *bus,uint8_t data);
void i2c_write_bytes(iic_bus_t *bus, uint8_t *data, int len);
unsigned char IICWaitAck(iic_bus_t *bus);
void IICSendByte(iic_bus_t *bus,unsigned char cSendByte);
uint8_t IIC_Write_One_Byte(iic_bus_t *bus, uint8_t daddr,uint8_t reg,uint8_t data);
uint8_t IIC_Write_Multi_Byte(iic_bus_t *bus, uint8_t daddr,uint8_t reg,uint8_t length,uint8_t buff[]);
unsigned char IIC_Read_One_Byte(iic_bus_t *bus, uint8_t daddr,uint8_t reg);
uint8_t IIC_Read_Multi_Byte(iic_bus_t *bus, uint8_t daddr, uint8_t reg, uint8_t length, uint8_t buff[]);


#endif
