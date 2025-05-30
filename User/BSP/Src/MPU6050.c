#include "MPU6050.h"
#include "string.h"

#define MP6050_Read 1
#define MP6050_Write 0

iic_bus_t *MPU6050_bus;

enum{
    MPU_SAMPLE_RATE_REG	= 0X19,
    MPU_CFG_REG	= 0X1A,
    MPU_GYRO_CFG_REG = 0X1B,
    MPU_ACCEL_CFG_REG = 0X1C,
    MPU_MOTION_DET_REG = 0X1F,
    MPU_MOTION_DUR_REG = 0X20,
    MPU_FIFO_EN_REG	= 0X23,
    I2C_MST_CTRL = 0x24,
    MPU_INTBP_CFG_REG = 0X37,
    MPU_INT_EN_REG = 0X38,
    ACCEL_XOUTH = 0X3B,
    ACCEL_XOUTL = 0X3C,
    ACCEL_YOUTH = 0X3D,
    ACCEL_YOUTL = 0X3E,
    ACCEL_ZOUTH = 0X3F,
    ACCEL_ZOUTL = 0X40,
    TEMP_OUTH = 0X41,
    TEMP_OUTL = 0X42,
    USER_CTRL = 0X6A,
    PWR_MGMT_1 = 0X6B,
    PWR_MGMT_2 = 0X6C,
    WhoAmI = 0x75,
}MPU_REG;

extern iic_bus_t *sensor_bus;
extern void Sensor_IIC_Init();

uint8_t MPU6050_Read_Reg(uint8_t reg_addr, uint8_t *rec_data)
{
    uint8_t ret;
    IICStart(MPU6050_bus);
    i2c_write_byte(MPU6050_bus, MPU6050_bus->addr | MP6050_Write);
    ret = IICWaitAck(MPU6050_bus);
    if(ret != 0)
    {
        IICStop(MPU6050_bus);
        return ret;
    }

    i2c_write_byte(MPU6050_bus, reg_addr);
    ret = IICWaitAck(MPU6050_bus);
    if(ret != 0)
    {
        IICStop(MPU6050_bus);
        return ret;
    }

    IICStart(MPU6050_bus);
    i2c_write_byte(MPU6050_bus, MPU6050_bus->addr | MP6050_Read);
    ret = IICWaitAck(MPU6050_bus);
    if(ret != 0)
    {
        IICStop(MPU6050_bus);
        return ret;
    }
    *rec_data = i2c_read_byte(MPU6050_bus);
    IICSendNotAck(MPU6050_bus);
    IICStop(MPU6050_bus);
    return 0;
}

uint8_t MPU6050_Write_Reg(uint8_t reg_addr, uint8_t write_data)
{
    uint8_t ret;
    IICStart(MPU6050_bus);
    i2c_write_byte(MPU6050_bus, MPU6050_bus->addr | MP6050_Write);
    ret = IICWaitAck(MPU6050_bus);
    if(ret != 0)
    {
        IICStop(MPU6050_bus);
        return ret;
    }
    i2c_write_byte(MPU6050_bus, reg_addr);
    ret = IICWaitAck(MPU6050_bus);
    if(ret != 0)
    {
        IICStop(MPU6050_bus);
        return ret;
    }
    i2c_write_byte(MPU6050_bus, write_data);
    ret = IICWaitAck(MPU6050_bus);
    if(ret != 0)
    {
        IICStop(MPU6050_bus);
        return ret;
    }
    IICStop(MPU6050_bus);
    return 0;
}

/**************************************************************************/
/*
    @brief  设置MPU6050的低通滤波器

    @param  lpf: Hz

    @return 0 if success
*/
/**************************************************************************/
u8 MPU_Set_LPF(u16 lpf)
{
	u8 data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6; 
	return MPU6050_Write_Reg(MPU_CFG_REG,data);
}

/**************************************************************************/
/*
    @brief  设置MPU6050的采样率

    @param  rate: 4~1000 Hz

    @return 0 if success
*/
/**************************************************************************/
u8 MPU_Set_Rate(u16 rate)
{
	u8 data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	data=MPU6050_Write_Reg(MPU_SAMPLE_RATE_REG,data);
 	return MPU_Set_LPF(rate/2);
}

/**************************************************************************/
/*
    @brief  设置MPU6050陀螺仪传感器满量程范围

    @param  fsr:0,+250dps;1,500dps;2,+1000dps;3,+2000dps

    @return 0 if success
*/
/**************************************************************************/
u8 MPU_Set_Gyro_Fsr(u8 fsr)
{
	return MPU6050_Write_Reg(MPU_GYRO_CFG_REG,fsr<<3); 
}

/**************************************************************************/
/*
    @brief  设置MPU6050的数字低通滤波器

    @param  fsr:低通滤波器频率(Hz)

    @return 0 if success
*/
/**************************************************************************/
u8 MPU_Set_Accel_Fsr(u8 fsr)
{
	return MPU6050_Write_Reg(MPU_ACCEL_CFG_REG,fsr<<3);
}

/**************************************************************************/
/*!
    @brief  initialize the motion function of MPU6050

    @param  NULL
*/
/**************************************************************************/
void MPU_Motion_Init(void)			
{
    MPU6050_Write_Reg(MPU_MOTION_DET_REG,0x01);    //set the acceleration threshold is (LSB*2)mg
    MPU6050_Write_Reg(MPU_MOTION_DUR_REG,0x01);    //Acceleration detection time is ()ms 
    MPU6050_Write_Reg(MPU_INTBP_CFG_REG,0X90);     //INT Pin active low level, reset until 50us
    MPU6050_Write_Reg(MPU_INT_EN_REG,0x40);       	//enable INT
}

/**************************************************************************/
/*!
    @brief  initialize the iic port connect with MPU6050

    @param  NULL
*/
/**************************************************************************/
void MPU_INT_Pin_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin : PB12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

uint8_t MPU6050_Init()
{
    Sensor_IIC_Init();
    static iic_bus_t tmp;
    uint8_t ret;
    memcpy(&tmp, sensor_bus, sizeof(iic_bus_t));
    MPU6050_bus = &tmp;
    MPU6050_bus->addr = 0xD0;

    ret = MPU6050_Write_Reg(PWR_MGMT_1, 0X80);
    delay_ms(500);
    MPU6050_Write_Reg(MPU_GYRO_CFG_REG, 0X08);
    ret = MPU6050_Wake();
    if(ret) return ret;

    // MPU6050_Write_Reg(MPU_FIFO_EN_REG, 0x00);
    // MPU6050_Write_Reg(I2C_MST_CTRL, 0x00);
    // MPU6050_Write_Reg(MPU_INT_EN_REG, 0x00);
    // ret = MPU6050_Write_Reg(MPU_SAMPLE_RATE_REG, 0x01);
    // if(ret) return ret;
    

    //  MPU_INT_Pin_Init();
    return 0;
}

uint8_t MPU6050_Get_ACCEL_XOUTH(uint8_t *rec_data)
{
    uint8_t ret;
    ret = MPU6050_Read_Reg(ACCEL_XOUTH, rec_data);
    return ret;
}
uint8_t MPU6050_Get_ACCEL_XOUTL(uint8_t *rec_data)
{
    uint8_t ret;
    ret = MPU6050_Read_Reg(ACCEL_XOUTL, rec_data);
    return ret;
}

uint8_t MPU6050_Get_ACCEL_YOUTH(uint8_t *rec_data)
{
    uint8_t ret;
    ret = MPU6050_Read_Reg(ACCEL_YOUTH, rec_data);
    return ret;
}
uint8_t MPU6050_Get_ACCEL_YOUTL(uint8_t *rec_data)
{
    uint8_t ret;
    ret = MPU6050_Read_Reg(ACCEL_YOUTL, rec_data);
    return ret;
}

uint8_t MPU6050_Get_ACCEL_ZOUTH(uint8_t *rec_data)
{
    uint8_t ret;
    ret = MPU6050_Read_Reg(ACCEL_ZOUTH, rec_data);
    return ret;
}
uint8_t MPU6050_Get_ACCEL_ZOUTL(uint8_t *rec_data)
{
    uint8_t ret;
    ret = MPU6050_Read_Reg(ACCEL_ZOUTL, rec_data);
    return ret;
}

uint8_t MPU6050_Get_ACCEL(int16_t *accel_x, int16_t *accel_y, int16_t *accel_z)
{
    uint8_t ret, rec_data[2];
    ret = MPU6050_Get_ACCEL_XOUTH(&rec_data[0]);
    if(ret)
    {
        return ret;
    }
    ret = MPU6050_Get_ACCEL_XOUTL(&rec_data[1]);
    if(ret)
    {
        return ret;
    }
    *accel_x = (rec_data[0] << 8) + rec_data[1];

    ret = MPU6050_Get_ACCEL_YOUTH(&rec_data[0]);
    if(ret)
    {
        return ret;
    }
    ret = MPU6050_Get_ACCEL_YOUTL(&rec_data[1]);
    if(ret)
    {
        return ret;
    }
    *accel_y = (rec_data[0] << 8) + rec_data[1];

    ret = MPU6050_Get_ACCEL_ZOUTH(&rec_data[0]);
    if(ret)
    {
        return ret;
    }
    ret = MPU6050_Get_ACCEL_ZOUTL(&rec_data[1]);
    if(ret)
    {
        return ret;
    }
    *accel_z = (rec_data[0] << 8) + rec_data[1];

    return 0;
}

uint8_t MPU6050_Get_ID(uint8_t *rec_data)
{
    uint8_t ret;
    ret = MPU6050_Read_Reg(WhoAmI, rec_data);
    return ret;
}

uint8_t MPU6050_Get_SleepState(uint8_t *rec_data)
{
    uint8_t ret;
    ret = MPU6050_Read_Reg(PWR_MGMT_1, rec_data);
    return ret;
}

uint8_t MPU6050_Wake()
{
    uint8_t ret;
    ret = MPU6050_Write_Reg(PWR_MGMT_1, 0x20);
    return ret;
}

uint8_t MPU6050_Get_TempH(uint8_t *rec_data)
{
    uint8_t ret;
    ret = MPU6050_Read_Reg(TEMP_OUTH, rec_data);
    return ret;
}
uint8_t MPU6050_Get_TempL(uint8_t *rec_data)
{
    uint8_t ret;
    ret = MPU6050_Read_Reg(TEMP_OUTL, rec_data);
    return ret;
}
uint8_t MPU6050_Get_Temp(int16_t *Temp)
{
    uint8_t ret = 0, rec_data[2];
    ret = MPU6050_Get_TempH(&rec_data[0]);
    if(ret) return ret;
    ret = MPU6050_Get_TempL(&rec_data[1]);
    *Temp = (rec_data[0] << 8) + rec_data[1];
    return ret;
}

uint8_t MPU6050_Get_User_CTRL(uint8_t *rec_data)
{
    uint8_t ret;
    ret = MPU6050_Read_Reg(USER_CTRL, rec_data);
    return ret;
}

uint8_t MPU6050_SelfTest_Enable()
{
    uint8_t ret;
    ret = MPU6050_Write_Reg(MPU_ACCEL_CFG_REG, 0xE0);
    ret = MPU6050_Write_Reg(MPU_GYRO_CFG_REG,  0xE0);
    return ret;
}

uint8_t MPU6050_Print_RegState()
{
    uint8_t ret, rec_data;
    for(uint8_t i = 13; i <= 117; i++)
    {
        ret = MPU6050_Read_Reg(i, &rec_data);
        if(ret == 0)
        {
            MC_printf(" Reg: %hhu Data: %hhu\r\n", i, rec_data);
        }
    }
    return ret;
}