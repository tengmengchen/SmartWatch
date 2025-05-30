#include "LVGL_task.h"

void LVGL_task(void)
{
    while(1)
    {
      lv_timer_handler();
      vTaskDelay(5);
    } 
}

void vApplicationTickHook(void)
{
    lv_tick_inc(1);
}

