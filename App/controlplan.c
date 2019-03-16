#include "common.h"
#include "include.h"
#include "camera.h"

int plan=1;    //1　摄像头控制方案
int Sub;					  //2　电磁控制方案
					  //3  路障躲避方案

void start_adc()
{     
    
    if (BlackEndML<=50&&BlackEndM<=50&&BlackEndMR<=50&BlackEndR<=50&&BlackEndL<=50)
    {
      Sub=ABS(BlackEndR-BlackEndL);
      if(Sub<=10)
      plan=2;
      else if (BlackEndLL<=8&&BlackEndM<=8&&BlackEndRR<=8&BlackEndR<=8&&BlackEndL<=8)
      plan=2;
      else
        plan=1;
      
       
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