#ifndef _AHT21_TASK_H
#define _AHT21_TASK_H

#include "main.h"
#include "AHT21.h"
#include "semphr.h"
#include "HWDataAccess.h"

extern SemaphoreHandle_t AHT21_Start_Signal;
extern SemaphoreHandle_t AHT21_Stop_Signal;

void AHT21_task(void);

#endif
