#include "common.h"
#include "include.h"


/**********���PID*******/
int steer_error;          // ����ƫ��
int steer_duty;           // ������ PWM ���
int steer_P = 8;          // ��� PD ���Ƶ� P ���� 
uint8 mid_line[61];                                //��������
/***************�������˿�************************/
#define STEER_FTM   FTM1                                   //PTA12
#define STEER_CH    FTM_CH0                                //������� 10000
#define STEER_HZ    (300)                                  //���Ƶ�� 100 Hz 100
#define STEER_MID   3940                               //������ֵ��ʵ����ֵ�������ǳ�װ��֮��ϸ�� 1520
#define angle_line (30)                                   //�����е��������ڴ��


void steer_control()
{
   steer_duty  = STEER_MID ;+ steer_P*(mid_line[angle_line]-40);  //����������� PWM
   ftm_pwm_duty(STEER_FTM ,STEER_CH, steer_duty);  //����������
}