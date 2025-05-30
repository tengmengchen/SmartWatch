#include "SPL06.h"

#define Write 0x00
#define Read 0x01

#define KT							524288.0		
#define KP							1572864.0

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

iic_bus_t *SPL_bus = NULL;
extern iic_bus_t *sensor_bus;
extern void Sensor_IIC_Init(void);

int twosComplementToSignMagnitude(int num)
{
    int invertedNum = ~num + 1;

    return -invertedNum;
}

uint8_t SPL06_Read_Byte(uint8_t daddr, uint8_t *recv_data)
{
    uint8_t ack;
    IICStart(SPL_bus);
    i2c_write_byte(SPL_bus, SPL_bus->addr | Write);
    ack = IICWaitAck(SPL_bus);
    if(ack != 0)
    {
        IICStop(SPL_bus);
        return 2;
    }
    i2c_write_byte(SPL_bus, daddr);
    IICWaitAck(SPL_bus);

    IICStart(SPL_bus);
    i2c_write_byte(SPL_bus, SPL_bus->addr | Read);
    ack = IICWaitAck(SPL_bus);
    if(ack != 0)
    {
        IICStop(SPL_bus);
        return 2;
    }
    *recv_data = i2c_read_byte(SPL_bus);
    IICSendNotAck(SPL_bus);
    IICStop(SPL_bus);
    return 0;
}

uint8_t SPL06_Write_Byte(uint8_t raddr, uint8_t data)
{
    uint8_t ack;
    IICStart(SPL_bus);
    i2c_write_byte(SPL_bus, SPL_bus->addr | Write);
    ack = IICWaitAck(SPL_bus);
    if(ack != 0)
    {
			  IICStop(SPL_bus);
        return 2;
    }
    i2c_write_byte(SPL_bus, raddr);
    IICWaitAck(SPL_bus);
    i2c_write_byte(SPL_bus, data);
    IICWaitAck(SPL_bus);
    IICStop(SPL_bus);
    return 0;
}

uint8_t SPL06_Get_ID(uint8_t *ID)
{
    uint8_t ret;
    ret = SPL06_Read_Byte(PR_ID, ID);
    return ret;
}

int get_c0()
{
	uint8_t buff[2];
	int c0 = 0;
    SPL06_Read_Byte(COEF_C0, &buff[0]);
    SPL06_Read_Byte(COEF_C0_C1, &buff[1]);
	c0 = buff[0];
	c0 = (c0 << 4) | (buff[1] >> 4);
	if(c0 & (1<<11))
	{
        //c0 = twosComplementToSignMagnitude(c0);
        c0 |= 0xFFFFF000;
    }
	return c0;
}

int get_c1()
{
	uint8_t buff[2];
	int c1 = 0;
    SPL06_Read_Byte(COEF_C0_C1, &buff[0]);
    SPL06_Read_Byte(COEF_C1, &buff[1]);
	c1 = buff[0] & 0x0F;
	c1 = (c1 << 8) | buff[1] ;
	if(c1 & (1<<11))
	{
        //c1 = twosComplementToSignMagnitude(c1); 
        c1 |= 0XFFFFF000;
    }
	return c1;
}

int get_c00()
{
	uint8_t buff[3];
	int c00 = 0;
    SPL06_Read_Byte(COEF_C00_H, &buff[0]);
    SPL06_Read_Byte(COEF_C00_L, &buff[1]);
    SPL06_Read_Byte(COEF_C00_C10, &buff[2]);
	c00 = buff[0];
	c00 = c00<<8 | buff[1];
	c00 = (c00<<4) | (buff[2]>>4);
	if(c00 & (1<<19))
	{
        //c00 = twosComplementToSignMagnitude(c00);
        c00 |= 0xFFF00000;
    }
	return c00;
}

int get_c10()
{
	uint8_t buff[3];
	int c10 = 0;
    SPL06_Read_Byte(COEF_C00_C10, &buff[0]);
    SPL06_Read_Byte(COEF_C10_M, &buff[0]);
    SPL06_Read_Byte(COEF_C10_L, &buff[0]);
	c10 = buff[0] & 0x0F;
	c10 = c10<<8 | buff[1];
	c10 = c10<<8 | buff[2];
	if(c10 & (1<<19))
	{
        //c10 = twosComplementToSignMagnitude(c10); 0001 000
        c10 |= 0xFFF00000;
    }
	return c10;
}

int get_c01()
{
	uint8_t buff[2];
	int c01 = 0;
    SPL06_Read_Byte(COEF_C01_H, &buff[0]);
    SPL06_Read_Byte(COEF_C01_L, &buff[1]);
	c01 = buff[0];
	c01 = c01<<8 | buff[1];
    if(c01 & (1<<15))
    {
        c01 |= 0xFFFF0000;
    }
	return c01;
}

int get_c11()
{
	uint8_t buff[2];
	int c11 = 0;
    SPL06_Read_Byte(COEF_C11_H, &buff[0]);
    SPL06_Read_Byte(COEF_C11_L, &buff[1]);
	c11 = buff[0];
	c11 = c11<<8 | buff[1];
    if(c11 & (1<<15))
    {
        c11 |= 0xFFFF0000;
    }
	return c11;
}

int get_c20()
{
	uint8_t buff[2];
	int c20 = 0;
    SPL06_Read_Byte(COEF_C20_H, &buff[0]);
    SPL06_Read_Byte(COEF_C20_L, &buff[1]);
	c20 = buff[0];
	c20 = c20<<8 | buff[1];
    if(c20 & (1<<15))
    {
        c20 |= 0xFFFF0000;
    }
	return c20;
}

int get_c21()
{
	uint8_t buff[2];
	int c21 = 0;
    SPL06_Read_Byte(COEF_C21_H, &buff[0]);
    SPL06_Read_Byte(COEF_C21_L, &buff[1]);
	c21 = buff[0];
	c21 = c21<<8 | buff[1];
    if(c21 & (1<<15))
    {
        c21 |= 0xFFFF0000;
    }
	return c21;
}

int get_c30()
{
	uint8_t buff[2];
	int c30 = 0;
    SPL06_Read_Byte(COEF_C30_H, &buff[0]);
    SPL06_Read_Byte(COEF_C30_L, &buff[1]);
	c30 = buff[0];
	c30 = c30<<8 | buff[1];
    if(c30 & (1<<15))
    {
        c30 |= 0xFFFF0000;
    }
	return c30;
}

// 启动连续采样, 并非一次
uint8_t SPL_Start_One_Measure()
{
    uint8_t data = 7, reg_addr = MEAS_CFG, ack, state, err_cnt = 0;
    ack = SPL06_Write_Byte(reg_addr, data);
    // data = 2;
    // ack = SPL06_Write_Byte(reg_addr, data);
    while((SPL06_Read_Byte(reg_addr, &state)>>5 & 1) == 0 || (SPL06_Read_Byte(reg_addr, &state)>>4 & 1) == 0)
    {
        err_cnt++;
        if(err_cnt == 5)
        {
            ack = 1;
            break;
        }
        delay_us(50);
        continue;
    }
    return ack;
}

void SPL06_Init()
{
    uint8_t state;
    Sensor_IIC_Init();
    static iic_bus_t tmp;
    memcpy(&tmp, sensor_bus, sizeof(iic_bus_t));  
    SPL_bus = &tmp;
    SPL_bus->addr = 0xEC;

    SPL06_Write_Byte(PRS_CFG, 0x01);		// Pressure 2x oversampling

	SPL06_Write_Byte(TMP_CFG, 0x80);		// External Temperature 1x oversampling

	SPL06_Write_Byte(MEAS_CFG, 0x07);	// continuous pressure and temperature measurement

	SPL06_Write_Byte(CFG_REG, 0x00);		//   

    SPL06_Read_Byte(MEAS_CFG, &state);
    while(((state >> 7) & 1) == 0)
    {
        SPL06_Read_Byte(MEAS_CFG, &state);
        continue;
    }

    c0 = get_c0();
	c1 = get_c1();
	c01 = get_c01();
	c11 = get_c11(); 
	c20 = get_c20();
	c21 = get_c21();
	c30 = get_c30();
	c00 = get_c00();
	c10 = get_c10();
    //MC_printf("c0:%d c1:%d\r\n",c0,c1);
    SPL_Start_One_Measure();
    
    SPL06_Start_Signal = xSemaphoreCreateBinary();
    SPL06_Stop_Signal  = xSemaphoreCreateBinary();
}

int SPL06_Read_Praw()
{
    uint8_t *rec_data;
    static int Pressure = 0;
    rec_data = pvPortMalloc(4);
    SPL06_Read_Byte(PRS_B2, &rec_data[0]);
    SPL06_Read_Byte(PRS_B1, &rec_data[1]);
    SPL06_Read_Byte(PRS_B0, &rec_data[2]);
    Pressure = (rec_data[0] << 16);
    Pressure += (rec_data[1] << 8);
    Pressure += rec_data[2];

    if(Pressure & (1 << 23)) 
    {
        Pressure |= 0xFF000000;
    }
    vPortFree(rec_data);
    return Pressure;
}

uint8_t SPL06_Read_Traw(int *Temp)
{
    uint8_t rec_data[3], ret;
    static int Temperature = 0;
    ret = SPL06_Read_Byte(TMP_B2, &rec_data[0]);
    if(ret == 2)return ret;
    ret = SPL06_Read_Byte(TMP_B1, &rec_data[1]);
    if(ret == 2)return ret;
    ret = SPL06_Read_Byte(TMP_B0, &rec_data[2]);
    if(ret == 2)return ret;
    Temperature = (rec_data[0] << 16);
    Temperature += (rec_data[1] << 8);
    Temperature += rec_data[2];
    if(Temperature & (1 << 23) == 1) 
    {
        Temperature |= 0XFF000000;
    }
    *Temp = Temperature;
    return 0;
}

uint8_t SPL06_Read_Tcomp(float *Temp)
{
    uint8_t ret;
    int Traw_sc;
    float Tcomp;
    ret = SPL06_Read_Traw(&Traw_sc);
    Tcomp = (float)c0 * 0.5 + (float)c1 * (float)(Traw_sc) / KT;
    *Temp = Tcomp;
    return ret;
}

float SPL06_GET_Pcomp()
{
    int Traw , Praw = SPL06_Read_Praw();
    float Pcomp, Praw_src, Traw_src;
    SPL06_Read_Traw(&Traw);
    Praw_src = (float)Praw / KP;
    Traw_src = (float)Traw / KT;
    Pcomp = c00 + Praw_src * (c10 + Praw_src * (c20 + Praw_src * c30)) + Traw_src * c01 + Traw_src * Praw_src * (c11 + Praw_src * c21);
    return Pcomp;
}

float SPL06_GET_Altitude()
{
    float Altitude, Pcomp, tmp;
    Pcomp = SPL06_GET_Pcomp();
    tmp = (1.0 - powf(Pcomp / (float)101325, 0.1903));
    Altitude = 44330.0 * tmp;
    return Altitude;
}


