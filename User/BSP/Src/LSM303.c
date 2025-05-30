#include "LSM303.h"
#include <string.h>
#include <math.h>

#define LSM303_Read  1
#define LSM303_Wrtie 0

#define Resolution_A 0

#define RAD_TO_DEG 57.2957795  // 180/π，将弧度转换为角度

enum{
    CTRL_REG1_A = 0x20,
    CTRL_REG4_A = 0x23,
    OUT_X_L_A = 0x28,
    OUT_X_H_A = 0x29,
    OUT_Y_L_A = 0x2A,
    OUT_Y_H_A = 0x2B,
    OUT_Z_L_A = 0x2C,
    OUT_Z_H_A = 0x2D,
}LSM303_Reg;


int16_t LSM303_Resolution_A[4] = {2,4,8,16};

iic_bus_t *LSM303_bus_A;
iic_bus_t *LSM303_bus_M;

extern iic_bus_t *sensor_bus;
extern void Sensor_IIC_Init();

uint8_t LSM303_WriteReg(iic_bus_t *LSM303_bus, uint8_t reg_addr, uint8_t write_data);
uint8_t LSM303_ReadReg(iic_bus_t *LSM303_bus, uint8_t reg_addr, uint8_t *rec_data);

uint8_t LSM303_Init()
{
    Sensor_IIC_Init();
    static iic_bus_t tmp;
    memcpy(&tmp, sensor_bus, sizeof(iic_bus_t));
    LSM303_bus_A = &tmp;
    LSM303_bus_A -> addr = 0x32;
    
    uint8_t ret, rec_data;
    while((ret = LSM303_Write_CTRL_REG1_A(0x17)) != 0){}
    LSM303_ReadReg(LSM303_bus_A, CTRL_REG4_A, &rec_data);
    rec_data |= (1 << 3);
    while((ret = LSM303_WriteReg(LSM303_bus_A, CTRL_REG4_A, rec_data)) != 0){}
    return 0;
}

uint8_t LSM303_WriteReg(iic_bus_t *LSM303_bus, uint8_t reg_addr, uint8_t write_data)
{
    uint8_t ret;
    IICStart(LSM303_bus);
    i2c_write_byte(LSM303_bus, LSM303_bus->addr | LSM303_Wrtie);
    ret = IICWaitAck(LSM303_bus);
    if(ret != 0)
    {
        IICStop(LSM303_bus);
        return ret;
    }
    i2c_write_byte(LSM303_bus, reg_addr);
    ret = IICWaitAck(LSM303_bus);
    if(ret != 0)
    {
        IICStop(LSM303_bus);
        return ret;
    }
    i2c_write_byte(LSM303_bus, write_data);
    ret = IICWaitAck(LSM303_bus);
    if(ret != 0)
    {
        IICStop(LSM303_bus);
        return ret;
    }
    IICStop(LSM303_bus);
    return 0;
}
uint8_t LSM303_ReadReg(iic_bus_t *LSM303_bus, uint8_t reg_addr, uint8_t *rec_data)
{
    uint8_t ret;
    IICStart(LSM303_bus);
    i2c_write_byte(LSM303_bus, LSM303_bus->addr | LSM303_Wrtie);
    ret = IICWaitAck(LSM303_bus);
    if(ret != 0)
    {
        IICStop(LSM303_bus);
        return ret;
    }

    i2c_write_byte(LSM303_bus, reg_addr);
    ret = IICWaitAck(LSM303_bus);
    if(ret != 0)
    {
        IICStop(LSM303_bus);
        return ret;
    }

    IICStart(LSM303_bus);
    i2c_write_byte(LSM303_bus, LSM303_bus->addr | LSM303_Read);
    ret = IICWaitAck(LSM303_bus);
    if(ret != 0)
    {
        IICStop(LSM303_bus);
        return ret;
    }
    *rec_data = i2c_read_byte(LSM303_bus);
    IICSendNotAck(LSM303_bus);
    IICStop(LSM303_bus);
    return 0;
}

uint8_t LSM303_Write_CTRL_REG1_A(uint8_t write_data)
{
    uint8_t ret;
    ret = LSM303_WriteReg(LSM303_bus_A, CTRL_REG1_A, write_data);
    return ret;
}

uint8_t LSM303_Read_CTRL_REG1_A(uint8_t *rec_data)
{
    uint8_t ret;
    ret = LSM303_ReadReg(LSM303_bus_A, CTRL_REG1_A, rec_data);
    return ret;
}

uint8_t LSM303_Read_Accel(double *X_Accel, double *Y_Accel, double *Z_Accel)
{
    uint8_t ret, rec_data[2];

    int16_t Resolution = LSM303_Resolution_A[Resolution_A];
    int16_t _X_Accel, _Y_Accel, _Z_Accel;

    ret = LSM303_ReadReg(LSM303_bus_A, OUT_X_H_A, &rec_data[0]);
    ret = LSM303_ReadReg(LSM303_bus_A, OUT_X_L_A, &rec_data[1]);
    _X_Accel = (rec_data[0] << 8) + rec_data[1];
    *X_Accel = ((double)_X_Accel / 32767.0) * Resolution;

    ret = LSM303_ReadReg(LSM303_bus_A, OUT_Y_H_A, &rec_data[0]);
    ret = LSM303_ReadReg(LSM303_bus_A, OUT_Y_L_A, &rec_data[1]);
    _Y_Accel = (rec_data[0] << 8) + rec_data[1];
    *Y_Accel = ((double)_Y_Accel / 32767.0) * Resolution;

    ret = LSM303_ReadReg(LSM303_bus_A, OUT_Z_H_A, &rec_data[0]);
    ret = LSM303_ReadReg(LSM303_bus_A, OUT_Z_L_A, &rec_data[1]);
    _Z_Accel = (rec_data[0] << 8) + rec_data[1];
    *Z_Accel = ((double)_Z_Accel / 32767.0) * Resolution;

    return ret;
}

uint8_t LSM303_Get_Pose(double *roll, double *pitch)
{
    uint8_t ret;
    double X_Accel, Y_Accel, Z_Accel;
    ret = LSM303_Read_Accel(&X_Accel, &Y_Accel, &Z_Accel);
    if(ret) return ret;

    *roll = atan2(Y_Accel, Z_Accel) * RAD_TO_DEG;
    *pitch = atan2(-X_Accel, sqrt(Y_Accel * Y_Accel + Z_Accel * Z_Accel)) * RAD_TO_DEG;

    return 0;
}