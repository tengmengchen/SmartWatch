#include "main.h"
#include "delay.h"
#include "BL24C02.h"
#include "WDG.h"
#include "AHT21.h"
#include "SPL06.h"
#include "lcd.h"
#include "lcd_init.h"
#include "spi.h"
#include "tim.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lvgl.h"
#include "lv_demo_widgets.h"
//#include "lv_demo_stress.h"
#include "LSM303.h"
#include "dma.h"
#include "ui.h"
#include "EM7028.h"
#include "BLE.h"
#include "MPU6050.h"
#include "KEY.h"
#include "HWDataAccess.h"
#include "RTC.h"

#include "watchtask.h"

#define START_STK_SIZE 		256  
#define START_TASK_PRIO		8

int EI_Flag = 0;

extern lv_obj_t *HomePage_Scr;

TaskHandle_t StartTask_Handler;
TaskHandle_t BL24C02Task_Handler;
TaskHandle_t EM7028Task_Handler;
TaskHandle_t BLETask_Handler;
TaskHandle_t LSM303Task_Handler;

void SystemClock_Config(void);
void BL24C02_test_task(void);
void lcd_init(void);
void EM7028_test_task(void);
void MPU6050_test_task(void);
void BLE_test_task(void);
void LSM303_test_task(void);

int main(void)
{	
  uint8_t ret;

    // SCB->VTOR = 0x0800C000U; //0x73FFF
    HAL_Init();
    
    /* Configure the system clock */
    SystemClock_Config();

    MX_DMA_Init();
   MX_USART1_UART_Init();


  //  BL24C02_Init();
  RTC_Init();
    SPL06_Init();
  AHT21_Init();
  // WDG_Init();
		// EM7028_Init();
    MPU6050_Init();
    // LSM303_Init();
    // BLE_Init(); // 蓝牙在APP程序中没有用到

    KEY_Init();


    MX_SPI1_Init();
    lcd_init();
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();
    HomePage_Init();

    start_task();  

		// xTaskCreate((TaskFunction_t )BL24C02_test_task,            
    //             (const char*    )"BL24C02_test_task",        
    //             (uint16_t       )START_STK_SIZE,        
    //             (void*          )NULL,                  
    //             (UBaseType_t    )START_TASK_PRIO,       
    //             (TaskHandle_t*  )&BL24C02Task_Handler);   

  //  xTaskCreate((TaskFunction_t )WDG_test_task,            
  //              (const char*    )"WDG_test_task",        
  //              (uint16_t       )START_STK_SIZE,        
  //              (void*          )NULL,                  
  //              (UBaseType_t    )START_TASK_PRIO,       
  //              (TaskHandle_t*  )&WDGTask_Handler);   	

    // xTaskCreate((TaskFunction_t )EM7028_test_task,            
    //             (const char*    )"EM7028_test_task",        
    //             (uint16_t       )START_STK_SIZE,        
    //             (void*          )NULL,                  
    //             (UBaseType_t    )START_TASK_PRIO,       
    //             (TaskHandle_t*  )&EM7028Task_Handler);  
				
    // xTaskCreate((TaskFunction_t )LSM303_test_task,            
    //             (const char*    )"LSM303_test_task",        
    //             (uint16_t       )START_STK_SIZE,        
    //             (void*          )NULL,                  
    //             (UBaseType_t    )START_TASK_PRIO,       
    //             (TaskHandle_t*  )&LSM303Task_Handler); 

    // xTaskCreate((TaskFunction_t )BLE_test_task,            
    //             (const char*    )"BLE_test_task",        
    //             (uint16_t       )START_STK_SIZE,        
    //             (void*          )NULL,                  
    //             (UBaseType_t    )START_TASK_PRIO,       
    //             (TaskHandle_t*  )&BLETask_Handler);   
    // vTaskStartScheduler(); 


								
}	

void lcd_init()
{
   GPIO_InitTypeDef  GPIO_InitStructure = {0};
 
  __HAL_RCC_GPIOB_CLK_ENABLE();
 
 GPIO_InitStructure.Pin = GPIO_PIN_0;	 
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP; 		 //推挽输出
 GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;//速度50MHz
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);	  //初始化GPIOB
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

/**
  * @brief  This function is a LSM303 test task.
  * @retval None
  */
 void LSM303_test_task(void)
 {
  uint8_t ret, rec_data;
  double X_Accel, Y_Accel, Z_Accel;
  double roll, pitch;
   while(1)
   {
    ret = LSM303_Read_CTRL_REG1_A(&rec_data);
    if(ret == 0)
    {
      MC_printf(" LSM303 Get Reg Data: %hhu\r\n", rec_data);
    }
    ret = LSM303_Read_Accel(&X_Accel, &Y_Accel, &Z_Accel);
    if(ret == 0)
    {
      MC_printf(" LSM303 Get Accel Data: %lf %lf %lf\r\n", X_Accel, Y_Accel, Z_Accel);
    }
    else
    {
      MC_printf(" LSM303 Get Accel Data Failed!\r\n");
    }

    ret = LSM303_Get_Pose(&roll, &pitch);
    if(ret == 0)
    {
      MC_printf(" LSM303 Get Pose: %lf %lf\r\n", roll, pitch);
    }
    else
    {
      MC_printf(" LSM303 Get Pose Failed!\r\n");
    }
    if(EI_Flag)
    {
      EI_Flag = 0;
      MC_printf(" Get GPIO Flag!\r\n");
    }
      vTaskDelay(10000);  
   }
 }

/**
  * @brief  This function is a BLE test task.
  * @retval None
  */
 void BLE_test_task(void)
 {
   delay_ms(1000);
   BLE_Enable();
  //  delay_ms(1000);
  //  MC_printf(" AT+B500\r\n");
  //  delay_ms(1000);
   while(1)
   {
     //MC_printf(" AT+BMBLE-TMC\r\n");
     vTaskDelay(10000);
   }
 }

///**
//  * @brief  This function is a EM7028 test task.
//  * @retval None
//  */
void EM7028_test_task(void)
{
   uint8_t ret, PID, i = 0;
   

   while(1)
   {
			while(1)
      {
        ret = EM7028_Get_ID(&PID);
        if(PID == 0x36)
        {
          MC_printf("EM7028: %d\r\n", PID);
          break;
        }
				MC_printf("EM7028: %d\r\n", PID);
        delay_ms(100);
      }
      
     
     vTaskDelay(500);
   }
}

/**
  * @brief  This function is a LVGL test task.
  * @retval None
  */
//  void MPU6050_test_task(void)
//  {
//   uint8_t ret, rec_data;
//     while(1)
//     {
//       ret = MPU6050_Get_ID(&rec_data);
//       //delay_ms(5);
//       vTaskDelay(2000);
//     } 
//  }

/**
  * @brief  This function is a BL24C02 test task.
  * @retval None
  */
 void BL24C02_test_task(void)
 {
  uint8_t ret = 0, i = 0, len = 17;
  uint8_t data[20], receive_data[20];
  for(; i < len; i++)
  {
     data[i] = i + 65;
  }
  ret = BL24C02_Write_Byte((uint8_t) 0x00, (uint8_t) 65);
  if(ret == 2)
  {
    HAL_UART_Transmit(&huart1, (uint8_t *)"1", 1, 0xffff);
  }
  else if(ret == 3)
  {
    HAL_UART_Transmit(&huart1, (uint8_t *)"2", 1, 0xffff);
  }
  else if(ret == 4)
  {
    HAL_UART_Transmit(&huart1, (uint8_t *)"3", 1, 0xffff);
  }
	
  delay_ms(10);
  ret = BL24C02_Read_Byte((uint8_t) 0x00);
  if(ret == 2)
  {
    HAL_UART_Transmit(&huart1, (uint8_t *)"4", 1, 0xffff);
  }
  else if(ret == 3)
  {
    HAL_UART_Transmit(&huart1, (uint8_t *)"5", 1, 0xffff);
  }
  else if(ret == 4)
  {
    HAL_UART_Transmit(&huart1, (uint8_t *)"6", 1, 0xffff);
  }
  HAL_UART_Transmit(&huart1, (uint8_t *)&ret, 1, 0xffff);


  ret = BL24C02_Write_Bytes((uint8_t) 0x00, (uint8_t *)data, len);
  if(ret == 2)
  {
    HAL_UART_Transmit(&huart1, (uint8_t *)"7", 1, 0xffff);
  }
  else if(ret == 3)
  {
    HAL_UART_Transmit(&huart1, (uint8_t *)"8", 1, 0xffff);
  }
  else if(ret == 4)
  {
    HAL_UART_Transmit(&huart1, (uint8_t *)"9", 1, 0xffff);
  }
	delay_ms(1000);
  ret = BL24C02_Read_Bytes((uint8_t) 0x00, (uint8_t *)receive_data, len);
  if(ret == 2)
  {
    HAL_UART_Transmit(&huart1, (uint8_t *)"a", 1, 0xffff);
  }
  else if(ret == 3)
  {
    HAL_UART_Transmit(&huart1, (uint8_t *)"b", 1, 0xffff);
  }
  else if(ret == 4)
  {
    HAL_UART_Transmit(&huart1, (uint8_t *)"c", 1, 0xffff);
  }
  else if(ret == 5)
  {
    HAL_UART_Transmit(&huart1, (uint8_t *)"d", 1, 0xffff);
  }
  for(i = 0; i < 16; i++)
  {
    HAL_UART_Transmit(&huart1, (uint8_t *)&receive_data[i], 1, 0xffff);
  }
  
  
  while(1){delay_ms(1000);}
 }

