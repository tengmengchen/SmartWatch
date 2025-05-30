#ifndef _SENSOR_H_
#define _SENSOR_H_

#include "i2c.h"
#include "freertos.h"
#include <string.h>
#include <math.h>

#define IIC_Sensor_CLK_ENABLE __HAL_RCC_GPIOB_CLK_ENABLE()

#define SPL_Write 0
#define SPL_Read 1

// iic_bus_t *sensor_bus = NULL, *spl_bus = NULL, *AHT_bus = NULL;
iic_bus_t *sensor_bus = NULL;
void Sensor_IIC_Init(void);


/* SPL */
// float SPL_GET_Altitude(void);
// int SPL_Read_Traw(void);
// uint8_t SPL_Start_One_Pressure_Measure(void);
// uint8_t SPL_Start_Continuous_Pres_Temp_Measure(void);
// void SPL_IIC_Init(void);
// uint8_t SPL_GET_ProductID(void);
// void SPL_WriteOneReg(uint8_t raddr, uint8_t dat);
// uint8_t SPL_Read_CTRL(void);
// void SPL_WriteOneReg(uint8_t addr, uint8_t dat);

/* AHT21 */
// void AHT21_Init(void);
// uint8_t Check_AHT21_Enable(void);

#endif
