#include "WDG.h"

typedef struct {
    GPIO_TypeDef *WDI_PORT;
    GPIO_TypeDef *WDGEN_PORT;
	uint16_t WDI_PIN;
    uint16_t WDGEN_PIN;
}WDG_STRUCT;
static WDG_STRUCT WDG_struct;

void WDG_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    WDG_CLK_ENABLE;

    WDG_struct.WDI_PORT = GPIOB;
    WDG_struct.WDGEN_PORT = GPIOB;
	WDG_struct.WDI_PIN = GPIO_PIN_2;
    WDG_struct.WDGEN_PIN = GPIO_PIN_1;

    GPIO_InitStructure.Pin = WDG_struct.WDI_PIN ;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(WDG_struct.WDI_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = WDG_struct.WDGEN_PIN ;
    HAL_GPIO_Init(WDG_struct.WDGEN_PORT, &GPIO_InitStructure);
}

void Feed_WDG(void)
{
    HAL_GPIO_TogglePin(WDG_struct.WDI_PORT, WDG_struct.WDI_PIN);
}
