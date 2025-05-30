#ifndef _SPL06_H_
#define _SPL06_H_

#include "i2c.h"
#include "freertos.h"
#include <string.h>
#include <math.h>
#include "SPL06_task.h"

void SPL06_Init(void);
uint8_t SPL06_Get_ID(uint8_t *ID);
uint8_t SPL_Start_One_Measure(void);
uint8_t SPL06_Read_Tcomp(float *Temp);
float SPL06_GET_Altitude(void);

#endif
