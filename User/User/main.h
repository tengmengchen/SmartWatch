#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include "stm32f4xx_hal.h"
#include "stm32f411xe.h"    
#include "FreeRTOS.h"
#include "task.h"
#include "Usart.h"

void Error_Handler(void);
int MC_printf(const char* s, ...);
/* SPL */
// extern float SPL_GET_Altitude(void);
// extern int SPL_Read_Traw(void);
// extern uint8_t SPL_Start_One_Pressure_Measure(void);
// extern void SPL_IIC_Init(void);
// extern uint8_t SPL_GET_ProductID(void);
// extern uint8_t SPL_Read_CTRL(void);
// extern uint8_t SPL_Start_Continuous_Pres_Temp_Measure(void);
// extern void SPL_WriteOneReg(uint8_t raddr, uint8_t dat);

/* AHT21 */
// extern void AHT21_Init(void);
// extern uint8_t Check_AHT21_Enable(void);

#endif
