#include "common.h"
#include "include.h"
#include "camera.h"

int controlplan = 1;  //1������ͷ���Ʒ���
					  //2����ſ��Ʒ���
					  //3  ·�϶�ܷ���

void start_adc()
{     

    if (BlackEndRR<=3&&BlackEndR<=3&&BlackEndLL<=3)
    {controlplan=2;}
     else
     {controlplan=1;}
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