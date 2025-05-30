#include "Sensor.h"

iic_bus_t tmp;
void Sensor_IIC_Init()
{
    if(sensor_bus != NULL) return;

    
    sensor_bus = &tmp;
    sensor_bus->IIC_SDA_PORT = GPIOB;
    sensor_bus->IIC_SDA_PIN = GPIO_PIN_13;
    sensor_bus->IIC_SCL_PORT = GPIOB;
    sensor_bus->IIC_SCL_PIN = GPIO_PIN_14;
    
    IIC_Sensor_CLK_ENABLE;
    IIC_Init(sensor_bus);

    
}
