#include "common.h"
#include "include.h"
#include "camera.h"
#include "MK60_pit.h"
int plan=1;    //1　摄像头控制方案
					  //2　电磁控制方案
					  //3  路障躲避方案
					  //4  断路控制方案
int getadc_t1;
int getadc_t2;
int adc_times=0;
void start_adc()
{     
    if (BlackEndRR<=60&&BlackEndR<=60&&BlackEndLL<=60)
  //if (BlackEndRR<=60&&BlackEndR<=60&&BlackEndLL<=60)
    {
      if(adc_times>=1)
      {
      getadc_t2 = pit_time_get_ms (PIT0);
      if(getadc_t2-getadc_t1<=10000)
      {
      plan=4;
      }
      }
	else
	{plan=1;}
    }
    else
    {plan=1;}
    if (BlackEndML<=60&&BlackEndM<=60&&BlackEndMR<=60&&BlackEndML>=57&&BlackEndM>=57&&BlackEndMR>=57)
    {
      adc_times++;
      getadc_t1 = pit_time_get_ms (PIT0); //获取计时时间
	plan = 4;
    }
      
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