#ifndef _LSM303_H_
#define _LSM303_H_

#include "i2c.h"

uint8_t LSM303_Init();
uint8_t LSM303_Read_CTRL_REG1_A(uint8_t *rec_data);
uint8_t LSM303_Read_Accel(double *X_Accel, double *Y_Accel, double *Z_Accel);
uint8_t LSM303_Write_CTRL_REG1_A(uint8_t write_data);
uint8_t LSM303_Get_Pose(double *roll, double *pitch);
#endif
