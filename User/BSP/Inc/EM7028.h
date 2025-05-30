#ifndef _EM7028_H_
#define _EM7028_H_

#include "i2c.h"
// #include "Sensor.h"

void EM7028_Init(void);
uint8_t EM7028_Get_ID(uint8_t *rec_data);

#endif