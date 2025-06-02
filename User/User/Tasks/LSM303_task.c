#include "LSM303_task.h"

int EI_Flag = 0;

void LSM303_task(void)
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
