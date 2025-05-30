#include "i2c.h"
 
/**
  * @brief IIC init
  * @param None
  * @retval None
  */
void IIC_Init(iic_bus_t *bus)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    __HAL_RCC_GPIOB_CLK_ENABLE();
		//bus->CLK_ENABLE();
    GPIO_InitStructure.Pin = bus->IIC_SDA_PIN ;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(bus->IIC_SDA_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = bus->IIC_SCL_PIN ;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(bus->IIC_SCL_PORT, &GPIO_InitStructure);

    
		
	
}

/**
  * @brief SDA线输入模式配置
  * @param None
  * @retval None
  */
 void SDA_Input_Mode(iic_bus_t *bus)
 {
     GPIO_InitTypeDef GPIO_InitStructure = {0};
 
     GPIO_InitStructure.Pin = bus->IIC_SDA_PIN;
     GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
     GPIO_InitStructure.Pull = GPIO_PULLUP;
     GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
     HAL_GPIO_Init(bus->IIC_SDA_PORT, &GPIO_InitStructure);
 }
 
 /**
   * @brief SDA线输出模式配置
   * @param None
   * @retval None
   */
 void SDA_Output_Mode(iic_bus_t *bus)
 {
     GPIO_InitTypeDef GPIO_InitStructure = {0};
 
     GPIO_InitStructure.Pin = bus->IIC_SDA_PIN;
     GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
     GPIO_InitStructure.Pull = GPIO_NOPULL;
     GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
     HAL_GPIO_Init(bus->IIC_SDA_PORT, &GPIO_InitStructure);
 }
 
 /**
   * @brief SDA线输出一个位
   * @param val 输出的数据
   * @retval None
   */
 void SDA_Output(iic_bus_t *bus, uint16_t val)
 {
     if ( val )
     {
         bus->IIC_SDA_PORT->BSRR |= bus->IIC_SDA_PIN;
     }
     else
     {
         bus->IIC_SDA_PORT->BSRR = (uint32_t)(bus->IIC_SDA_PIN) << 16U;
     }
 }

/**
  * @brief SCL线输出一个位
  * @param val 输出的数据
  * @retval None
  */
 void SCL_Output(iic_bus_t *bus, uint16_t val)
 {
     if ( val )
     {
         bus->IIC_SCL_PORT->BSRR |= bus->IIC_SCL_PIN;
     }
     else
     {
          bus->IIC_SCL_PORT->BSRR = (uint32_t)(bus->IIC_SCL_PIN) << 16U;
     }
 }
 
 /**
   * @brief SDA输入一位
   * @param None
   * @retval GPIO读入一位
   */
 uint8_t SDA_Input(iic_bus_t *bus)
 {
     if(HAL_GPIO_ReadPin(bus->IIC_SDA_PORT, bus->IIC_SDA_PIN) == GPIO_PIN_SET){
         return 1;
     }else{
         return 0;
     }
 }

 /**
  * @brief IIC发送确认信号
  * @param None
  * @retval None
  */
 void IICSendAck(iic_bus_t *bus)
 {
     SDA_Output(bus,0);
         delay_us(1);
     SCL_Output(bus,1);
         delay_us(2);
     SCL_Output(bus,0);
         delay_us(2);
     
 }
 
 /**
   * @brief IIC发送非确认信号
   * @param None
   * @retval None
   */
 void IICSendNotAck(iic_bus_t *bus)
 {
     SDA_Output(bus,1);
         delay_us(1);
     SCL_Output(bus,1);
         delay_us(2);
     SCL_Output(bus,0);
         delay_us(2);
 
 }

 /**
   * @brief iic启动信号
   * @param None
   * @retval 
   */
  void IICStart(iic_bus_t *bus)
  {
      SDA_Output(bus,1);
      //delay1(DELAY_TIME);
          delay_us(2);
      SCL_Output(bus,1);
          delay_us(2);
      SDA_Output(bus,0);
          delay_us(2);
      SCL_Output(bus,0);
          delay_us(2);
  }

  /**
  * @brief IIC结束信号
  * @param None
  * @retval None
  */
void IICStop(iic_bus_t *bus)
{
    SCL_Output(bus,0);
		delay_us(2);
    SDA_Output(bus,0);
		delay_us(2);
    SCL_Output(bus,1);
		delay_us(2);
    SDA_Output(bus,1);
		delay_us(2);

}

 /**
  * @brief IIC读取一个bit
  * @param None
  * @retval None
  */
uint8_t i2c_read_bit(iic_bus_t *bus)
{
    SCL_Output(bus,0);
    delay_us(2);
    SCL_Output(bus,1);
    delay_us(2);
    return SDA_Input(bus);
}

 /**
  * @brief iic读取一个字节
  * @param None
  * @retval None
  */
uint8_t i2c_read_byte(iic_bus_t *bus)
{
    int i = 0;
    uint8_t ret = 0;
    SDA_Input_Mode(bus);
    for(; i < 8; i++)
    {
        ret = (ret << 1) | (i2c_read_bit(bus));
        SCL_Output(bus,0);
    }
    SCL_Output(bus,0);
    SDA_Output_Mode(bus);
    return ret;
}

 /**
  * @brief IIC读取若干字节
  * @param None
  * @retval None
  */
int i2c_read_bytes(iic_bus_t *bus, uint8_t *data, int len)
{
    int i = 0;
    for(; i < len; i++)
    {
        *(data + i) = i2c_read_byte(bus);
    }
    return len;
}

 /**
  * @brief IIC写一个bit
  * @param None
  * @retval None
  */
void i2c_write_bit(iic_bus_t *bus, uint8_t data)
{
    SCL_Output(bus,0);
    delay_us(2);
    SDA_Output(bus,data);
    delay_us(2);
    SCL_Output(bus,1);
    delay_us(2);
}

 /**
  * @brief IIC写一个字节
  * @param None
  * @retval None
  */
void i2c_write_byte(iic_bus_t *bus,uint8_t data)
{
    int i = 7;
    for(; i >= 0; i--)
    {
        i2c_write_bit(bus, (data>>i)&1);
    }
    SCL_Output(bus,0);
    delay_us(2);
}

/**
  * @brief IIC write bytes
  * @param None
  * @retval None
  */
void i2c_write_bytes(iic_bus_t *bus, uint8_t *data, int len)
{
    int i = 0;
    for(; i < len; i++)
    {
        i2c_write_byte(bus, *(data+i));
    }
}

/**
  * @brief IIC等待确认信号
  * @param None
  * @retval None
  */
 unsigned char IICWaitAck(iic_bus_t *bus)
 {
     unsigned short cErrTime = 5;
     SDA_Input_Mode(bus);
     delay_us(4);
     SCL_Output(bus,1);
     while(SDA_Input(bus))
     {
         cErrTime--;
        //  SCL_Output(bus,0);
         delay_us(4);
         if (0 == cErrTime)
         {
             SDA_Output_Mode(bus);
             IICStop(bus);
             return ERROR;
         }
     }
     SDA_Output_Mode(bus);
     SCL_Output(bus,0);
     delay_us(4);
     return SUCCESS;
 }

/**
  * @brief IIC发送一个字节
  * @param cSendByte 需要发送的字节
  * @retval None
  */
 void IICSendByte(iic_bus_t *bus,unsigned char cSendByte)
 {
     unsigned char  i = 8;
     while (i--)
     {
         SCL_Output(bus,0);
         delay_us(2);
         SDA_Output(bus, cSendByte & 0x80);
                 delay_us(1);
         cSendByte += cSendByte;
                 delay_us(1);
         SCL_Output(bus,1);
                 delay_us(1);
     }
     SCL_Output(bus,0);
         delay_us(2);
 }
 
 /**
  * @brief IIC接收一个字节
  * @param None
  * @retval 接收到的字节
  */
unsigned char IICReceiveByte(iic_bus_t *bus)
{
    unsigned char i = 8;
    unsigned char cR_Byte = 0;
    SDA_Input_Mode(bus);
    while (i--)
    {
        cR_Byte += cR_Byte;
        SCL_Output(bus,0);
				delay_us(2);
        SCL_Output(bus,1);
				delay_us(1);
        cR_Byte |=  SDA_Input(bus);
    }
    SCL_Output(bus,0);
    SDA_Output_Mode(bus);
    return cR_Byte;
}

 uint8_t IIC_Write_One_Byte(iic_bus_t *bus, uint8_t daddr,uint8_t reg,uint8_t data)
{				   	  	    																 
  IICStart(bus);  
	
	IICSendByte(bus,daddr<<1);	    
	if(IICWaitAck(bus))	//等待应答
	{
		IICStop(bus);		 
		return 1;		
	}
	IICSendByte(bus,reg);
	IICWaitAck(bus);	   	 										  		   
	IICSendByte(bus,data);     						   
	IICWaitAck(bus);  		    	   
  IICStop(bus);
	delay_us(1);
	return 0;
}

uint8_t IIC_Write_Multi_Byte(iic_bus_t *bus, uint8_t daddr,uint8_t reg,uint8_t length,uint8_t buff[])
{			
	unsigned char i;	
  IICStart(bus);  
	
	IICSendByte(bus,daddr<<1);	    
	if(IICWaitAck(bus))
	{
		IICStop(bus);
		return 1;
	}
	IICSendByte(bus,reg);
	IICWaitAck(bus);	
	for(i=0;i<length;i++)
	{
		IICSendByte(bus,buff[i]);     						   
		IICWaitAck(bus); 
	}		    	   
  IICStop(bus);
	delay_us(1);
	return 0;
} 

unsigned char IIC_Read_One_Byte(iic_bus_t *bus, uint8_t daddr,uint8_t reg)
{
	unsigned char dat;
	IICStart(bus);
	IICSendByte(bus,daddr<<1);
	IICWaitAck(bus);
	IICSendByte(bus,reg);
	IICWaitAck(bus);
	
	IICStart(bus);
	IICSendByte(bus,(daddr<<1)+1);
	IICWaitAck(bus);
	dat = IICReceiveByte(bus);
	IICSendNotAck(bus);
	IICStop(bus);
	return dat;
}


uint8_t IIC_Read_Multi_Byte(iic_bus_t *bus, uint8_t daddr, uint8_t reg, uint8_t length, uint8_t buff[])
{
	unsigned char i;
	IICStart(bus);
	IICSendByte(bus,daddr<<1);
	if(IICWaitAck(bus))
	{
		IICStop(bus);		 
		return 1;		
	}
	IICSendByte(bus,reg);
	IICWaitAck(bus);
	
	IICStart(bus);
	IICSendByte(bus,(daddr<<1)+1);
	IICWaitAck(bus);
	for(i=0;i<length;i++)
	{
		buff[i] = IICReceiveByte(bus);
		if(i<length-1)
		{IICSendAck(bus);}
	}
	IICSendNotAck(bus);
	IICStop(bus);
	return 0;
}
