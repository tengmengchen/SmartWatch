#include "EM7028.h"
#include "string.h"

#define EM7028_Write 0x00
#define EM7028_Read 0x01

iic_bus_t *EM7028_bus;

enum{
    PID = 0x00,
    HRS_CFG = 0x01,
    HRS2_DATA_OFFSET = 0x08,
    HRS2_GAIN_CTRL = 0x0A,
    HRS1_CTRL = 0x0D,
    INT_CTRL = 0x0E
}EM7028_REG;

extern iic_bus_t *sensor_bus;
extern void Sensor_IIC_Init();

uint8_t EM7028_WriteOneReg(uint8_t reg, uint8_t data)
{
    uint8_t ret;
    IICStart(EM7028_bus);
    i2c_write_byte(EM7028_bus, EM7028_bus->addr | EM7028_Write);//| EM7028_Write
    ret = IICWaitAck(EM7028_bus);
    if(ret != 0)
    {
        return 2;
    }
    i2c_write_byte(EM7028_bus,reg);
    IICWaitAck(EM7028_bus);
    i2c_write_byte(EM7028_bus, data);
    IICWaitAck(EM7028_bus);
    IICStop(EM7028_bus);
		return 0;
}

void EM7028_Init()
{
    Sensor_IIC_Init();
    static iic_bus_t tmp;
    memcpy(&tmp, sensor_bus, sizeof(iic_bus_t));
    EM7028_bus = &tmp;
    EM7028_bus->addr = 0x00;//0X48

    uint8_t PID = 0;
    while(1)
    {
        EM7028_Get_ID(&PID);
        if(PID == 0x36)
        {
            break;
        }
        delay_ms(100);
        MC_printf("EM7028_ADDR : %d \r\n", EM7028_bus->addr);
        EM7028_bus->addr++;
    }

    EM7028_WriteOneReg(HRS_CFG,0x00);//en continue mode 		

	EM7028_WriteOneReg(HRS2_DATA_OFFSET, 0x00);//no data shift

	EM7028_WriteOneReg(HRS2_GAIN_CTRL, 0x7f);

	EM7028_WriteOneReg(HRS1_CTRL, 0x47);

    EM7028_WriteOneReg(INT_CTRL, 0x00);
}

uint8_t EM7028_Enable()
{
    uint8_t PID, i = 10;
    while(i--)
    {
        EM7028_Get_ID(&PID);
        if(PID == 0X36)
        {
            break;
        }
    }
    if(i == 0) return 2;

    EM7028_WriteOneReg(HRS_CFG, 0x08);
    return 0;
}

uint8_t EM7028_Disable()
{
    uint8_t PID, i = 10;
    while(i--)
    {
        EM7028_Get_ID(&PID);
        if(PID == 0X36)
        {
            break;
        }
    }
    if(i == 0) return 2;

    EM7028_WriteOneReg(HRS_CFG, 0x00);
    return 0;
}

uint8_t EM7028_Get_ID(uint8_t *rec_data)
{
    uint8_t ret;
    IICStart(EM7028_bus);
    i2c_write_byte(EM7028_bus, EM7028_bus->addr );//| EM7028_Write
    ret = IICWaitAck(EM7028_bus);
    if(ret != 0)
    {
        if(EM7028_bus->addr == 0xff) return 0;
        return 2;
    }
    i2c_write_byte(EM7028_bus,PID);
    IICWaitAck(EM7028_bus);
    IICStop(EM7028_bus);

    IICStart(EM7028_bus);
    i2c_write_byte(EM7028_bus, EM7028_bus->addr + 1);
    ret = IICWaitAck(EM7028_bus);
    if(ret != 0)
    {
        return 3;
    }
    *rec_data = i2c_read_byte(EM7028_bus);
    IICSendNotAck(EM7028_bus);
    IICStop(EM7028_bus);
    return 0;
}
