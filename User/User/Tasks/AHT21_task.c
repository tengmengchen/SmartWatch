#include "AHT21_task.h"

SemaphoreHandle_t AHT21_Start_Signal, AHT21_Stop_Signal;


void AHT21_task(void)
{
    while(1)
    {
        if(xSemaphoreTake(AHT21_Start_Signal, portMAX_DELAY) == pdTRUE)
        {
            while(1)
            {
                HW_Interface.HW_AHT21_Interface.Get_AHT21_Data();
                vTaskDelay(2000);
                if(xSemaphoreTake(AHT21_Stop_Signal, 0) == pdTRUE)
                {
                    break;
                }
            }
        }
    }
}
