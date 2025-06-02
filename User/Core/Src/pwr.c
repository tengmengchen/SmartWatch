#include "pwr.h"

void Low_Power_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;  // 上升沿触发中断
    GPIO_InitStruct.Pull = GPIO_NOPULL;          // 无上下拉，可根据需要改为PULLUP或PULLDOWN
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI4_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);
}

void Enter_Stop_Mode(void)
{
    // 配置 Stop 模式相关寄存器
    __HAL_RCC_PWR_CLK_ENABLE();
    CLEAR_BIT(PWR->CR, PWR_CR_PDDS); // 选择 Stop 模式（不是 Standby）
    SET_BIT(PWR->CR, PWR_CR_LPDS);   // 使用低功耗电压调节器

    // 设置 Cortex-M4 的 SLEEPDEEP 和 SLEEPONEXIT 位
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk;
    Lcd_BackLight_Off();

    // __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_All); // 或者具体清除某些线，根据你的需要
    // __HAL_RCC_PWR_CLK_ENABLE();

    // // HAL_PWREx_EnableLowPowerRunMode();  // 如果支持低功耗运行模式
    // // HAL_PWREx_EnableLowPowerRegulator();  // 启用低功耗电压调节器

    // CLEAR_BIT(PWR->CR, PWR_CR_PDDS); // 使能进入 Stop 模式
    // SET_BIT(PWR->CR, PWR_CR_LPDS);   // 低功耗电压调节器

    // // 设置 Cortex-M4 的 SLEEPDEEP 位
    // SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    // __WFI();

    // // 清除 SLEEPDEEP 位，恢复正常运行
    // SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;

    // SystemClock_Config();
}

void Exit_Stop_Mode(void)
{
    // 清除 SLEEPONEXIT 防止再次睡眠
    SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;
    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
    // 恢复主时钟
    SystemClock_Config();
    Lcd_BackLight_On();
}