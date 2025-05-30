#include "RTC.h"
#include "stm32f411xe.h"

RTC_HandleTypeDef hrtc;

void RTC_Unlock(void) {
    // 使能对RTC寄存器的访问（需要先解锁备份域）
    PWR->CR |= PWR_CR_DBP;
    
    // 写入解锁密钥到 RTC_WPR 以解除写保护
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;
}

void RTC_Init(void) {
    // 1. 使能 PWR 和 RTC 时钟
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess(); // 使能访问 RTC 备份域
    __HAL_RCC_RTC_ENABLE();
    // 1. 解除备份域保护
    PWR->CR |= PWR_CR_DBP;

    // 2. 使能 LSE 时钟（或选择 LSI/HSE 作为 RTC 时钟）
    RCC->BDCR |= RCC_BDCR_LSEON;
    while (!(RCC->BDCR & RCC_BDCR_LSERDY));  // 等待 LSE 稳定

    // 3. 选择 RTC 时钟源
    RCC->BDCR |= RCC_BDCR_RTCSEL_0;  // 选择 LSE 作为 RTC 时钟源
    RCC->BDCR |= RCC_BDCR_RTCEN;     // 使能 RTC
    
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;
    
    // 3. 进入初始化模式
    RTC->ISR |= RTC_ISR_INIT;
    while (!(RTC->ISR & RTC_ISR_INITF)); // 等待 INITF 置 1
    
    // 4. 配置 RTC 分频器，生成 1Hz 时钟
    RTC->PRER = (127 << 16) | (255); // PREDIV_A = 127, PREDIV_S = 255
    
    // 5. 配置时间格式 (24小时制)
    RTC->CR &= ~RTC_CR_FMT;
    
    // 6. 设置初始时间和日期
    RTC->TR = (0 << 22) | (1 << 20) | (6 << 16); // 00:30:45 (24小时制)
    RTC->DR = (2 << 20) | (5 << 16) | (4 << 13) | (0 << 12) | (5 << 8) | (2 << 4) | (9 << 0); // 2023年4月15日，星期五
    
    // 7. 退出初始化模式
    RTC->ISR &= ~RTC_ISR_INIT;
    
    // 8. 重新启用 RTC 寄存器写保护
    RTC->WPR = 0xFF;
}

void RTC_GetTime(void) {
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;
    
    // 读取时间
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN); // 读取日期（必须读取，否则时间可能不更新）

    MC_printf(" 时间: %02d:%02d:%02d 日期: %04d-%02d-%02d\n", 
        sTime.Hours, sTime.Minutes, sTime.Seconds,
        2000 + sDate.Year, sDate.Month, sDate.Date);
}
