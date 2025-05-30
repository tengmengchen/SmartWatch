
#include "AHT21.h"
#define AHT21_Write 0
#define AHT21_Read 1

#define Check_State 0x71
#define Start_Measure 0xAC

iic_bus_t *AHT_bus = NULL;
extern iic_bus_t *sensor_bus;
extern void Sensor_IIC_Init(void);

uint8_t Check_AHT21_Enable()
{
    uint8_t ack = 1, state;
    IICStart(AHT_bus);
    i2c_write_byte(AHT_bus,AHT_bus->addr | AHT21_Read);
    ack = IICWaitAck(AHT_bus);
    if(ack != 0)
    {
        IICStop(AHT_bus);
        return 2;
    }
    state = i2c_read_byte(AHT_bus);
    IICSendNotAck(AHT_bus);
    IICStop(AHT_bus);
    return state;
}

void AHT21_Init()
{
    Sensor_IIC_Init();
    static iic_bus_t tmp;
    AHT_bus = &tmp;
    memcpy(AHT_bus, sensor_bus, sizeof(iic_bus_t));  
    AHT_bus->addr = 0x70;

    delay_ms(40);
	
	if(Check_AHT21_Enable()&0x08!=0x08)
	{
		IICStart(AHT_bus);
		IICSendByte(AHT_bus,0x70);
		IICWaitAck(AHT_bus);
		IICSendByte(AHT_bus,0xBE);
		IICWaitAck(AHT_bus);
		IICSendByte(AHT_bus,0x08);
		IICWaitAck(AHT_bus);
		IICSendByte(AHT_bus,0x00);
		IICWaitAck(AHT_bus);
		IICStop(AHT_bus);
		delay_ms(10);
	}

    AHT21_Start_Signal = xSemaphoreCreateBinary();
    AHT21_Stop_Signal  = xSemaphoreCreateBinary();
}

uint8_t AHT21_test()
{
    uint8_t ack = 1;
    IICStart(AHT_bus);
    i2c_write_byte(AHT_bus, AHT_bus->addr | AHT21_Write);
    ack = IICWaitAck(AHT_bus);
    if(ack != 0)
    {
        IICStop(AHT_bus);
        return 2;
    }
    IICStop(AHT_bus);
    return 0;
}

uint8_t AHT21_Start_Measure()
{
    uint8_t ack = 1;
    IICStart(AHT_bus);
    i2c_write_byte(AHT_bus, AHT_bus->addr | AHT21_Write);
    ack = IICWaitAck(AHT_bus);
    if(ack != 0)
    {
        IICStop(AHT_bus);
        return 2;
    }
    i2c_write_byte(AHT_bus,Start_Measure);
    IICWaitAck(AHT_bus);
    i2c_write_byte(AHT_bus,0x33);
    IICWaitAck(AHT_bus);
    i2c_write_byte(AHT_bus,0x00);
    IICWaitAck(AHT_bus);
    IICStop(AHT_bus);
    return 0;
}

uint8_t AHT21_Read_Temperature(double *ret_Hum, double *ret_Temp)
{
    unsigned int data[5];
    uint8_t ack = 1;
    double Hum = 0, Temp = 0;
   
    IICStart(AHT_bus);
    i2c_write_byte(AHT_bus, AHT_bus->addr | AHT21_Read);
    ack = IICWaitAck(AHT_bus);
    if(ack != 0)
    {
        IICStop(AHT_bus);
        return 3;
    }
    i2c_read_byte(AHT_bus);
    IICSendAck(AHT_bus);
    data[0] = i2c_read_byte(AHT_bus);
    IICSendAck(AHT_bus);
    data[1] = i2c_read_byte(AHT_bus);
    IICSendAck(AHT_bus);
    data[2] = i2c_read_byte(AHT_bus);
    IICSendAck(AHT_bus);
    data[3] = i2c_read_byte(AHT_bus);
    IICSendAck(AHT_bus);
    data[4] = i2c_read_byte(AHT_bus);
    IICSendNotAck(AHT_bus);
    IICStop(AHT_bus);

    Hum = (data[0]<<12);
    Hum += (data[1]<<4);
    Hum += (data[2]>>4);
    Hum = (Hum / pow(2,20)) * 100;
    *ret_Hum = Hum;

    Temp = ((data[2]&(0x0F))<<16);
    Temp += (data[3]<<8);
    Temp += data[4];
    Temp = (Temp / pow(2,20)) * 200 -50;
    *ret_Temp = Temp;

    return 0;
}

// uint8_t AHT21_Enable()
// {
//     IICStart(AHT_bus);
//     i2c_write_byte(AHT_bus,AHT_bus->addr | AHT21_Write);
    
// }
