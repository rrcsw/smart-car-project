#include "common.h"
#include "include.h"
#include "camera.h"

int plan=1;    //1　摄像头控制方案
					  //2　电磁控制方案
					  //3  路障躲避方案

void start_adc()
{     
    
    if (BlackEndRR<=45&&BlackEndM<=45&&BlackEndLL<=45&BlackEndL<=45&&BlackEndR<=45)
    {
      plan=2;
       ftm_pwm_duty(FTM0,FTM_CH5,100);
        ftm_pwm_duty(FTM0,FTM_CH6,100); 
    }
    else
     {plan=1;}
    /***
	int i, j;
	for (i = 0;i < RowMax - 3;i++)
	{
		for (j = 0;j < ColumnMax - 1;j++)
		{
			if (img[i][j] == Black_Point && img[i][j + 1] == White_Point && img[i][j + 2] == White_Point)
				break;
		}
	}
	if (i > 20 && j > 20 && j < 59)
	controlplan = 2;
****/
  
}
/***
void end_adc()
{
	int j;
	for (j = 0;j < ColumnMax - 1;j++)
	{
		if (img[0][j] == White_Point && img[0][j + 1] == White_Point && img[0][j + 2] == White_Point)
			controlplan = 1;
		break;
	}

}
*****/