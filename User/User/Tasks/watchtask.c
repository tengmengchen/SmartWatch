#include "watchtask.h"

#define START_STK_SIZE 		256  
#define START_TASK_PRIO		8

TaskHandle_t AHT21Task_Handler;
TaskHandle_t LVGLTask_Handler;
TaskHandle_t WDGTask_Handler;
TaskHandle_t SPLTask_Handler;
TaskHandle_t MPU6050Task_Handler;

 void MPU6050_test_task(void);

void start_task()
{
    xTaskCreate((TaskFunction_t )AHT21_task,          
                (const char*    )"AHT21_task",        
                (uint16_t       )START_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )START_TASK_PRIO,       
                (TaskHandle_t*  )&AHT21Task_Handler);

    xTaskCreate((TaskFunction_t )LVGL_task,            
            (const char*    )"LVGL_task",        
            (uint16_t       )2048,        
            (void*          )NULL,                  
            (UBaseType_t    )START_TASK_PRIO + 3,       
            (TaskHandle_t*  )&LVGLTask_Handler);   
    
    // xTaskCreate((TaskFunction_t )WDG_task,            
    //         (const char*    )"WDG_task",        
    //         (uint16_t       )START_STK_SIZE,        
    //         (void*          )NULL,                  
    //         (UBaseType_t    )START_TASK_PRIO + 1,       
    //         (TaskHandle_t*  )&WDGTask_Handler); 

    xTaskCreate((TaskFunction_t )SPL06_task,            
            (const char*    )"SPL06_task",        
            (uint16_t       )START_STK_SIZE,        
            (void*          )NULL,                  
            (UBaseType_t    )START_TASK_PRIO,       
            (TaskHandle_t*  )&SPLTask_Handler); 
            
    xTaskCreate((TaskFunction_t )MPU6050_test_task,            
            (const char*    )"MPU6050_test_task",        
            (uint16_t       )START_STK_SIZE,        
            (void*          )NULL,                  
            (UBaseType_t    )START_TASK_PRIO,       
            (TaskHandle_t*  )&MPU6050Task_Handler);   

    vTaskStartScheduler(); 
}


 /**
  * @brief  This function is a MPU6050 test task.
  * @retval None
  */
 void MPU6050_test_task(void)
 {
  vTaskDelay(1000);
    
   uint8_t MPU_ID, ret;
  //  ret = MPU6050_Wake();
  //  if(ret == 0)
  //  {
  //   MC_printf(" MPU6050 WAKE Success!\r\n");
  // }
  // vTaskDelay(1000);
  // else
  // {
  //     MC_printf(" MPU6050 WAKE Failed!\r\n");
  // }

   ret = MPU6050_Get_ID(&MPU_ID);
   if(ret == 0)
   {
      MC_printf(" Get MPU6050 ID: %hhu\r\n", MPU_ID);//(int)MPU_ID&(0x000000ff)
   }
   else
   {
      MC_printf(" Get MPU6050 ID Failed!\r\n");
   }

   MPU6050_SelfTest_Enable();
   MPU6050_Print_RegState();
   
   while(1)
   {
     int16_t accel_x, accel_y, accel_z;

     ret = MPU6050_Get_ACCEL(&accel_x, &accel_y, &accel_z);
     
     if(ret == 0)
    {
      MC_printf(" Get MPU6050 ACCEL: X:%hd Y:%hd Z:%hd\r\n",accel_x, accel_y, accel_z);
    }
    else
    {
      MC_printf(" Get MPU6050 ACCEL Failed!\r\n");
    }
    
    uint8_t sleep_state;
    ret = MPU6050_Get_SleepState(&sleep_state);
    if(ret == 0)
    {
      MC_printf(" Get MPU6050 SleepState: %hhu\r\n", sleep_state);
    }

    int16_t temp;
    float temp_result;
    ret = MPU6050_Get_Temp(&temp);
    temp_result = (float)(temp) / 340.0 + 36.53;
    if(ret == 0)
    {
      MC_printf(" Get MPU6050 Temp: %f\r\n", temp_result);
    }
    MPU6050_Print_RegState();
     vTaskDelay(5000);
     
   }
 }