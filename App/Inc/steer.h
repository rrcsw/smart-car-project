#ifndef __STEER_H__
#define __SYEER_H__

#define S3010_FTM   FTM1       //FTM1ģ��
#define S3010_CH    FTM_CH0   //ͨ����PTA8
#define S3010_HZ    50 
//�����Ƶ��


#if 0

#define SteerMax 660     //������ֵ
#define SteerMin  515    //�����ֵ
#define SteerMidle 440   //�����Сֵ

#endif

#if 1

#define SteerMax 780    //������ֵ
#define SteerMin  620    //�����Сֵ
#define SteerMidle 710 //�����ֵ

#endif



extern float  LastError;
extern float  Error;


void SteerInit(void);//�����ʼ��
void CalculateError(void);
extern void SteerControl(void);


#endif