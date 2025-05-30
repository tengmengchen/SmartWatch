#ifndef _WDG_H_
#define _WDG_H_

#include "main.h"
#include "stm32f411xe.h"
#include "delay.h"

#define WDG_CLK_ENABLE __HAL_RCC_GPIOB_CLK_ENABLE()

void WDG_Init(void);
void Feed_WDG(void);

#endif
