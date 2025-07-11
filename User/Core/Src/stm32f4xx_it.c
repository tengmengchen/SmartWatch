/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
#include "stm32f411xe.h"
#include "lvgl.h"
#include "PageChange.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#include "key.h"
//#include "power.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */
extern DMA_HandleTypeDef hdma_spi1_tx;
/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
//void SVC_Handler(void)
//{
//  /* USER CODE BEGIN SVCall_IRQn 0 */

//  /* USER CODE END SVCall_IRQn 0 */
//  /* USER CODE BEGIN SVCall_IRQn 1 */

//  /* USER CODE END SVCall_IRQn 1 */
//}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

extern void Enter_Stop_Mode(void);
extern void Exit_Stop_Mode(void);
void EXTI4_IRQHandler(void)
{
    // 检查是否是PA4中断
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_4) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4); // 清除中断标志
        if(!(SCB->SCR & SCB_SCR_SLEEPDEEP_Msk))
        {
          Enter_Stop_Mode();
        }
        else
        {
          Exit_Stop_Mode();
        }
    }
}

extern int EI_Flag;
extern TIM_HandleTypeDef htim2;
void EXTI9_5_IRQHandler(void)
{
  if(EXTI->PR & (1 << 5))
  {
    EXTI->PR |= (1 << 5);
    HAL_TIM_Base_Start(&htim2);
    
  }

}

void TIM2_IRQHandler(void)
{
	EI_Flag = 1;
  if (TIM2->SR & TIM_SR_UIF) {  // 检测输入捕获事件
    TIM2->SR &= ~TIM_SR_UIF;   // 清除标志
    htim2.State = HAL_TIM_STATE_READY;
    if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == 0)//确认消抖
    {
      Page_Back(&Top_Page);
    }
  }
  
}

/**
  * @brief This function handles Pendable request for system service.
  */
//void PendSV_Handler(void)
//{
//  /* USER CODE BEGIN PendSV_IRQn 0 */

//  /* USER CODE END PendSV_IRQn 0 */
//  /* USER CODE BEGIN PendSV_IRQn 1 */

//  /* USER CODE END PendSV_IRQn 1 */
//}

/**
  * @brief This function handles System tick timer.
  */
extern void HAL_IncTick(void);
extern void xPortSysTickHandler (void); 
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */
//  static uint16_t key1_long_press;
//  if(!KEY1)
//  {
//		if(boot_in_menu_flag)
//		{
//			key1_long_press += 1;
//			if(key1_long_press >= 3000)
//			{
//				Power_DisEnable();
//			}
//		}
//  }
//  else
//  {
//    key1_long_press = 0;
//  }
  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  // lv_tick_inc(1);
  
  /* USER CODE BEGIN SysTick_IRQn 1 */
	if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)
    {
        xPortSysTickHandler();
    }
  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/* USER CODE BEGIN 1 */
/**
  * @brief This function handles DMA2 stream2 global interrupt.
  */
 void DMA2_Stream2_IRQHandler(void)
 {
   /* USER CODE BEGIN DMA2_Stream2_IRQn 0 */
 
   /* USER CODE END DMA2_Stream2_IRQn 0 */
   HAL_DMA_IRQHandler(&hdma_spi1_tx);
   /* USER CODE BEGIN DMA2_Stream2_IRQn 1 */
 
   /* USER CODE END DMA2_Stream2_IRQn 1 */
 }
/* USER CODE END 1 */
