#ifndef _SPL06_TASK_H_
#define _SPL06_TASK_H_

#include "SPL06.h"
#include "semphr.h"
#include "HWDataAccess.h"

void SPL06_task(void);

extern SemaphoreHandle_t SPL06_Start_Signal, SPL06_Stop_Signal;

#endif
