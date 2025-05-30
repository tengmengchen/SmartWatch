#ifndef _MPU6050_H_
#define _MPU6050_H_

#include "i2c.h"

uint8_t MPU6050_Init(void);
uint8_t MPU6050_Get_ID(uint8_t *rec_data);
uint8_t MPU6050_Get_ACCEL(int16_t *accel_x, int16_t *accel_y, int16_t *accel_z);
uint8_t MPU6050_Wake();
uint8_t MPU6050_Get_SleepState(uint8_t *rec_data);
uint8_t MPU6050_Get_Temp(int16_t *Temp);
uint8_t MPU6050_Get_User_CTRL(uint8_t *rec_data);
uint8_t MPU6050_SelfTest_Enable();
uint8_t MPU6050_Print_RegState();
#endif
