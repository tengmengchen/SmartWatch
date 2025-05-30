#include "WDG_task.h"

void WDG_task(void)
{
  while(1)
  {
    Feed_WDG();
    vTaskDelay(100);
  }
}