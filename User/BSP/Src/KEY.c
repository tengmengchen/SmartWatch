#include "KEY.h"
#include "stm32f411xe.h"

TIM_HandleTypeDef htim2;

uint8_t TIM2_Init()
{ 
     // 1. 使能 TIM2 时钟
     __HAL_RCC_TIM2_CLK_ENABLE();

     // 2. 配置 TIM2 基础参数
     
     htim2.Instance = TIM2;
     htim2.Init.Prescaler = 5000 - 1; // 预分频器，时钟 50MHz，这里降为 10kHz
     htim2.Init.CounterMode = TIM_COUNTERMODE_UP; // 递增计数模式
     htim2.Init.Period = 100 - 1;  // 10 毫秒定时
     htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
     htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; // 关闭自动重装
 
     // 3. 初始化定时器
     HAL_TIM_Base_Init(&htim2);
 
     // 4. 使能单次模式
     htim2.Instance->CR1 |= TIM_CR1_OPM; // 设置 One-Pulse Mode
     // 5. 使能 TIM2 更新中断
    __HAL_TIM_ENABLE_IT(&htim2, TIM_IT_UPDATE);

    // 6. 设置中断优先级并使能 NVIC（嵌套向量中断控制器）
    HAL_NVIC_SetPriority(TIM2_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);

    HAL_TIM_Base_Start(&htim2);
}

uint8_t KEY_Init()
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING; // 下降沿触发中断
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);//使能中断
    
    TIM2_Init();

    return 0;
}