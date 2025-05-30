#ifndef _AHT21_H_
#define _AHT21_H_

#include "i2c.h"
#include "freertos.h"
#include <string.h>
#include <math.h>
#include "AHT21_task.h"

void AHT21_Init(void);
uint8_t Check_AHT21_Enable(void);
uint8_t AHT21_Read_Temperature(double *ret_Hum, double *ret_Temp);
uint8_t AHT21_Start_Measure(void);
uint8_t AHT21_test(void);

#endif
