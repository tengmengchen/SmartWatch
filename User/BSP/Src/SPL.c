#include "Sensor.h"

#define KP 524288

#define KT 524288

#define COEF_C0						0x10
#define COEF_C0_C1				0x11
#define COEF_C1						0x12
#define COEF_C00_H				0x13
#define COEF_C00_L				0x14
#define COEF_C00_C10			0x15
#define COEF_C10_M				0x16
#define COEF_C10_L				0x17
#define COEF_C01_H				0x18
#define COEF_C01_L				0x19
#define COEF_C11_H				0x1A
#define COEF_C11_L				0x1B
#define COEF_C20_H				0x1C
#define COEF_C20_L				0x1D
#define COEF_C21_H				0x1E
#define COEF_C21_L				0x1F
#define COEF_C30_H				0x20
#define COEF_C30_L				0x21

enum{
    PRS_B2 = 0X00,
    PRS_B1 = 0X01,
    PRS_B0 = 0X02,
    TMP_B2 = 0X03,
    TMP_B1 = 0X04,
    TMP_B0 = 0X05,
    PRS_CFG = 0X06,
    TMP_CFG = 0X07,
    MEAS_CFG = 0X08,
    CFG_REG = 0X09, //FIFO CFG
    PR_ID = 0X0D,
}SPL_Reg;

int c0,c1,c00, c10, c20, c30, c01, c11, c21;

uint8_t SPL_Read_Byte()
{
    uint8_t ret;
    ret = i2c_read_byte(spl_bus);
    return ret;
}

void SPL_WriteOneReg(uint8_t raddr, uint8_t dat);
uint8_t SPL_Read_Bytes(uint8_t reg_addr, uint8_t *rec_data, int len)
{
    int i = 0;
    uint8_t ack = 1;
    IICStart(spl_bus);
    i2c_write_byte(spl_bus, spl_bus->addr | SPL_Write);
    ack = IICWaitAck(spl_bus);
    if(ack!=0) 
    {
        IICStop(spl_bus);
        return 2;
    }

    i2c_write_byte(spl_bus, reg_addr);
    ack = 1;
    ack = IICWaitAck(spl_bus);
    if(ack!=0) 
    {
        IICStop(spl_bus);
        return 3;
    }

    IICStart(spl_bus);
    i2c_write_byte(spl_bus, spl_bus->addr | SPL_Read);
    ack = 1;
    ack = IICWaitAck(spl_bus);
    if(ack!=0) 
    {
        IICStop(spl_bus);
        return 4;
    }
    for(; i < len; i++)
    {
        rec_data[i] = SPL_Read_Byte();
        if(i != len-1) IICSendAck(spl_bus);
        else IICSendNotAck(spl_bus);
    }
    IICStop(spl_bus);
    return 0;
}

// void SPL_WriteOneReg(uint8_t raddr, uint8_t dat)
// {
//     uint8_t ack;
//     IICStart(spl_bus);
//     i2c_write_byte(spl_bus, spl_bus->addr | SPL_Write);
//     ack = IICWaitAck(spl_bus);
//     i2c_write_byte(spl_bus, raddr);
//     ack = IICWaitAck(spl_bus);
//     i2c_write_byte(spl_bus, dat);
//     ack = IICWaitAck(spl_bus);
//     IICStop(spl_bus);
// 	//IIC_Write_One_Byte(spl_bus, addr,addr,dat);
// }

uint8_t SPL_Write_Byte(uint8_t reg_addr, uint8_t data_addr)
{
    uint8_t ack;
    i2c_write_byte(spl_bus, reg_addr);
    ack = IICWaitAck(spl_bus);
    if(ack != 0)return 1;
    i2c_write_byte(spl_bus, data_addr);
    ack = IICWaitAck(spl_bus);
    return ack;
}

uint8_t SPL_Write_Bytes(uint8_t *reg_addr, uint8_t *data_addr, int len)
{
    int i = 0, ack;
    IICStart(spl_bus);
    i2c_write_byte(spl_bus, spl_bus->addr | SPL_Write);
    ack = IICWaitAck(spl_bus);
    if(ack != 0)
    {
        IICStop(spl_bus);
        return 2;
    }

    for(; i < len ;i++)
    {
        ack = SPL_Write_Byte(reg_addr[i], data_addr[i]);
        if(ack != 0)
        {
            IICStop(spl_bus);
            return 2;
        }
    }
    IICStop(spl_bus);
    return 0;
}

uint8_t SPL_Start_One_Pressure_Measure()
{
    uint8_t data_addr = 1, reg_addr = MEAS_CFG, ack;
    ack = SPL_Write_Bytes(&reg_addr, &data_addr, 1);
    return ack;
}

uint8_t SPL_Start_Continuous_Pres_Temp_Measure()
{
    uint8_t data_addr = 7, reg_addr = MEAS_CFG, ack = 0;
    SPL_WriteOneReg(MEAS_CFG, data_addr);
    // ack = SPL_Write_Bytes(&reg_addr, &data_addr, 1);
    return ack;
}

int get_c0()
{
	uint8_t buff[2];
	int c0 = 0;
    SPL_Read_Bytes(COEF_C0, buff, 2);
	c0 = buff[0];
	c0 = (c0 << 4) | (buff[1] >> 4);
	if(c0 & (1<<11))
	{c0 |= 0xF000;}
	return c0;
}

int get_c1()
{
	uint8_t buff[2];
	int c1 = 0;
    SPL_Read_Bytes(COEF_C0_C1, buff, 2);
	c1 = buff[0] & 0x0F;
	c1 = (c1 << 8) | buff[1] ;
	if(c1 & (1<<11))
	{c1 |= 0xF000;}
	return c1;
}

int get_c00()
{
	uint8_t buff[3];
	int c00 = 0;
    SPL_Read_Bytes(COEF_C00_H, buff, 3);
	c00 = buff[0];
	c00 = c00<<8 | buff[1];
	c00 = (c00<<4) | (buff[2]>>4);
	if(c00 & (1<<19))
	{c00 |= 0xFFF00000;}
	return c00;
}

int get_c10()
{
	uint8_t buff[3];
	int c10 = 0;
    SPL_Read_Bytes(COEF_C00_C10, buff, 3);
	c10 = buff[0] & 0x0F;
	c10 = c10<<8 | buff[1];
	c10 = c10<<8 | buff[2];
	if(c10 & (1<<19))
	{c10 |= 0xFFF00000;}
	return c10;
}

int get_c01()
{
	uint8_t buff[2];
	int c01 = 0;
    SPL_Read_Bytes(COEF_C01_H, buff, 2);
	c01 = buff[0];
	c01 = c01<<8 | buff[1];
	return c01;
}

int get_c11()
{
	uint8_t buff[2];
	int c11 = 0;
    SPL_Read_Bytes(COEF_C11_H, buff, 2);
	c11 = buff[0];
	c11 = c11<<8 | buff[1];
	return c11;
}

int get_c20()
{
	uint8_t buff[2];
	int c20 = 0;
    SPL_Read_Bytes(COEF_C20_H, buff, 2);
	c20 = buff[0];
	c20 = c20<<8 | buff[1];
	return c20;
}

int get_c21()
{
	uint8_t buff[2];
	int c21 = 0;
    SPL_Read_Bytes(COEF_C21_H, buff, 2);
	c21 = buff[0];
	c21 = c21<<8 | buff[1];
	return c21;
}

int get_c30()
{
	uint8_t buff[2];
	int c30 = 0;
    SPL_Read_Bytes(COEF_C30_H, buff, 2);
	c30 = buff[0];
	c30 = c30<<8 | buff[1];
	return c30;
}

void SPL_IIC_Init()
{
    Sensor_IIC_Init();
    spl_bus = (iic_bus_t *)pvPortMalloc(sizeof(iic_bus_t));
    memcpy(spl_bus, sensor_bus, sizeof(iic_bus_t));  
    spl_bus->addr = 0xEC;

    SPL_WriteOneReg(PRS_CFG, 0x01);		// Pressure 2x oversampling

	SPL_WriteOneReg(TMP_CFG, 0x80);		// External Temperature 1x oversampling

	SPL_WriteOneReg(MEAS_CFG, 0x07);	// continuous pressure and temperature measurement

	SPL_WriteOneReg(CFG_REG, 0x00);		//  

    c0 = get_c0();
	c1 = get_c1();
	c01 = get_c01();
	c11 = get_c11(); 
	c20 = get_c20();
	c21 = get_c21();
	c30 = get_c30();
	c00 = get_c00();
	c10 = get_c10();
}

uint8_t SPL_Read_CTRL()
{
    uint8_t rec_data, ack;
    static uint8_t CTRL = 0;
   
    ack = SPL_Read_Bytes(MEAS_CFG, &rec_data, 1);
    if(ack != 0 )
    {
        return 9;
    }
    CTRL = rec_data & 0x07;
    
    return CTRL;
}

int SPL_Read_Praw()
{
    uint8_t *rec_data;
    static int Pressure = 0;
    rec_data = pvPortMalloc(4);
    SPL_Read_Bytes(PRS_B2, rec_data, 3);
    Pressure = (rec_data[0] << 16) + (rec_data[1] << 8) + rec_data[2];
    if(Pressure & (1 << 23) == 1) 
    {
        Pressure = 0;
    }
    vPortFree(rec_data);
    return Pressure;
}

int SPL_Read_Traw()
{
    uint8_t *rec_data, ret;
    static int Temperature = 0;

    SPL_Read_Bytes(MEAS_CFG, &ret, 1);
    if(ret != 1)
    {
        return -1;
    }

    rec_data = pvPortMalloc(4);
    SPL_Read_Bytes(TMP_B2, rec_data, 3);
    Temperature = (rec_data[0] << 16) + (rec_data[1] << 8) + rec_data[2];
    if(Temperature & (1 << 24) == 1) 
    {
        Temperature = 0;
    }
    vPortFree(rec_data);
    return Temperature;
}

float SPL_GET_Pcomp()
{
    int Traw = SPL_Read_Traw(), Praw = SPL_Read_Praw();
    float Pcomp, Praw_src, Traw_src;
    Praw_src = (float)Praw / KP;
    Traw_src = (float)Traw / KT;
    Pcomp = c00 + Praw_src * (c10 + Praw_src * (c20 + Praw_src * c30)) + Traw_src * c01 + Traw_src * Praw_src * (c11 + Praw_src * c21);
    return Pcomp;
}

float SPL_GET_Altitude()
{
    float Altitude, Pcomp;
    Pcomp = SPL_GET_Pcomp();
    Altitude = 44330 * (1 - powf(Pcomp / (float)1013.25, 1 / (float)5.255));
    return Altitude;
}

uint8_t SPL_GET_ProductID(void)
{
    uint8_t ret = 0, rec_data;
    ret = SPL_Read_Bytes(PR_ID, &rec_data, 1);
    if(ret == 2 || ret == 3 || ret == 4)
        return 2;
    else return rec_data;
}


/* test */


uint8_t IIC_Write_One_Byte(iic_bus_t *bus, uint8_t reg, uint8_t data)
{				   	  	    																 
  IICStart(bus);  
	
	IICSendByte(bus,bus->addr<<1);	    
	if(IICWaitAck(bus))	//等待应答
	{
		IICStop(bus);		 
		return 1;		
	}
	IICSendByte(bus,reg);
	IICWaitAck(bus);	   	 										  		   
	IICSendByte(bus,data);     						   
	IICWaitAck(bus);  		    	   
  IICStop(bus);
	delay_us(1);
	return 0;
}

void SPL_WriteOneReg(uint8_t addr, uint8_t dat)
{
	IIC_Write_One_Byte(spl_bus, addr ,dat);
}
