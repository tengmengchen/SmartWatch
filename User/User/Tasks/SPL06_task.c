#include "SPL06_task.h"

SemaphoreHandle_t SPL06_Start_Signal, SPL06_Stop_Signal;

void SPL06_task(void)
{
  uint8_t ID, ret;
  char str[30];
  float Altitude;
//   SPL_Start_One_Measure();
  while(1)
  {
    if(xSemaphoreTake(SPL06_Start_Signal, portMAX_DELAY) == pdTRUE)
    {
        while(1)
        {
            // ret = SPL06_Get_ID(&ID);
            // if(ret != 0)
            // {
            //   MC_printf("SPL06 GET ID Error\r\n");
            // }
            // else
            // {
            //   MC_printf(" SPL06 ID: %x\r\n",ID);
            // }

            //SPL06_Read_Tcomp(&Altitude);
            HW_Interface.HW_SPL06_Interface.Get_SPL06_Data();
            // sprintf(str," Altitude:%f\r\n",Altitude);
            // MC_printf("%s\r\n", str);
            vTaskDelay(2000);
            if(xSemaphoreTake(SPL06_Stop_Signal, 0) == pdTRUE)
            {
                break;
            }
        }
    }
  }
  
  
}