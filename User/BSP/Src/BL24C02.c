#include "BL24C02.h"

#define BL_CLK_ENABLE __HAL_RCC_GPIOA_CLK_ENABLE()
#define BL24C02_WRITE 0
#define BL24C02_READ 1

iic_bus_t BL24C02_bus;

void BL24C02_Init(void)
{
    BL24C02_bus.IIC_SDA_PORT = GPIOA;
    BL24C02_bus.IIC_SCL_PORT = GPIOA;
    BL24C02_bus.IIC_SDA_PIN = GPIO_PIN_11;
    BL24C02_bus.IIC_SCL_PIN = GPIO_PIN_12;
    BL24C02_bus.addr = EEPROM_ADDR;

    BL_CLK_ENABLE;
    IIC_Init(&BL24C02_bus);

}

uint8_t BL24C02_Write_Byte(uint8_t daddr, uint8_t data)
{
    int ack = 0;
    IICStart(&BL24C02_bus);
    i2c_write_byte(&BL24C02_bus, BL24C02_bus.addr | BL24C02_WRITE);
    ack = IICWaitAck(&BL24C02_bus);
    if(ack != 0)
    {
        IICStop(&BL24C02_bus);
        return 2;
    }
    i2c_write_byte(&BL24C02_bus, daddr);
    ack = IICWaitAck(&BL24C02_bus);
    if(ack != 0)
    {
        IICStop(&BL24C02_bus);
        return 3;
    }
	i2c_write_byte(&BL24C02_bus, data);
    ack = IICWaitAck(&BL24C02_bus);
    if(ack != 0)
    {
        IICStop(&BL24C02_bus);
        return 4;
    }
    IICStop(&BL24C02_bus);

    return ack == 0;
}

uint8_t BL24C02_Write_Bytes(uint8_t daddr_start, uint8_t *data, int len)
{
    int ack = 0, i = 0;
    IICStart(&BL24C02_bus);
    i2c_write_byte(&BL24C02_bus, BL24C02_bus.addr | BL24C02_WRITE);
    ack = IICWaitAck(&BL24C02_bus);
    if(ack != 0)
    {
        IICStop(&BL24C02_bus);
        return 2;
    }
    i2c_write_byte(&BL24C02_bus, daddr_start);
    ack = IICWaitAck(&BL24C02_bus);
    if(ack != 0)
    {
        IICStop(&BL24C02_bus);
        return 3;
    }
    
    for(; i < len; i++)
    {
        i2c_write_byte(&BL24C02_bus, *(data+i));
        ack = IICWaitAck(&BL24C02_bus);
        if(ack != 0)
        {
            IICStop(&BL24C02_bus);
            return 4;
        }
    }
    IICStop(&BL24C02_bus);
    delay_us(1);
    return 0;
}

uint8_t BL24C02_Read_Byte(uint8_t daddr)
{
    int ack = 0;
    uint8_t ret;
    IICStart(&BL24C02_bus);
    i2c_write_byte(&BL24C02_bus, BL24C02_bus.addr | BL24C02_WRITE);
    ack = IICWaitAck(&BL24C02_bus);
    if(ack != 0)
    {
        IICStop(&BL24C02_bus);
        return 2;
    }
    i2c_write_byte(&BL24C02_bus, daddr);
    ack = IICWaitAck(&BL24C02_bus);
    if(ack != 0)
    {
        IICStop(&BL24C02_bus);
        return 3;
    }
    delay_us(1);
    IICStart(&BL24C02_bus);
    i2c_write_byte(&BL24C02_bus, BL24C02_bus.addr | BL24C02_READ);
    ack = IICWaitAck(&BL24C02_bus);
    if(ack != 0)
    {
        IICStop(&BL24C02_bus);
        return 4;
    }

    ret = i2c_read_byte(&BL24C02_bus);
    IICStop(&BL24C02_bus);
	return ret;
}

uint8_t BL24C02_Read_Bytes(uint8_t daddr_start, uint8_t *data, int len)
{
    int ack = 0, i = 0;
    IICStart(&BL24C02_bus);
    i2c_write_byte(&BL24C02_bus, BL24C02_bus.addr | BL24C02_WRITE);
    ack = IICWaitAck(&BL24C02_bus);
    if(ack != 0)
    {
        IICStop(&BL24C02_bus);
        return 2;
    }
    i2c_write_byte(&BL24C02_bus, daddr_start);
    ack = IICWaitAck(&BL24C02_bus);
    if(ack != 0)
    {
        IICStop(&BL24C02_bus);
        return 3;
    }

    delay_us(1);
    IICStart(&BL24C02_bus);
    i2c_write_byte(&BL24C02_bus, BL24C02_bus.addr | BL24C02_READ);
    ack = IICWaitAck(&BL24C02_bus);
    if(ack != 0)
    {
        IICStop(&BL24C02_bus);
        return 4;
    }

    for(; i < len; i++)
    {
        *(data+i) = i2c_read_byte(&BL24C02_bus);
        IICSendAck(&BL24C02_bus);
    }
    IICSendNotAck(&BL24C02_bus);
    IICStop(&BL24C02_bus);
    delay_us(1);
    return 0;
}
