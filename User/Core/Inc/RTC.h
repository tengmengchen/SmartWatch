#ifndef _RTC_H_
#define _RTC_H_

#include "main.h"

#define RTC_MAGIC_FLAG 0x32F2

void RTC_Init(void);
void RTC_GetTime(void);
#endif
