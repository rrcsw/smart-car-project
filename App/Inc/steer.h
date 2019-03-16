#ifndef __STEER_H__
#define __SYEER_H__

#define S3010_FTM   FTM1       //FTM1模块
#define S3010_CH    FTM_CH0   //通道号PTA8
#define S3010_HZ    50 
//舵机的频率


#if 0

#define SteerMax 660     //舵机最大值
#define SteerMin  515    //舵机中值
#define SteerMidle 440   //舵机最小值

#endif

#if 1

#define SteerMax 850    //舵机最大值
#define SteerMin  510    //舵机最小值
#define SteerMidle 680 //舵机中值

#endif



extern float  LastError;
extern float  Error;


void SteerInit(void);//舵机初始化
void CalculateError(void);
extern void SteerControl(void);
extern void TurnBack();
extern void NormalControl();
extern void NormalControl_adc();


#endif
