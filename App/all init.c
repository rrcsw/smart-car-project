#include "common.h"
#include "include.h"


/**********�������˿�*********/
#define MOTOR_FTM   FTM0
#define MOTOR3_PWM  FTM_CH5
#define MOTOR4_PWM  FTM_CH6
#define MOTOR_HZ    20000//(20*1000)

/***************�������˿�************************/
#define STEER_FTM   FTM1                                   //PTA12
#define STEER_CH    FTM_CH0                                //������� 10000
#define STEER_HZ    50                                  //���Ƶ�� 100 Hz 100
//define STEER_MID   3940                               //������ֵ��ʵ����ֵ�������ǳ�װ��֮��ϸ�� 1520

/*******��������ͷ********/
extern uint8 imgbuff[CAMERA_SIZE];                         
/********���庯��********/
void PORTA_IRQHandler();
void DMA0_IRQHandler();

/*********** ADC �ɼ��ڶ���  **************/
#define ADL     ADC0_SE16    //  ��Ӧ PTE1 ���� ,����ad�ɼ��������ǵĵ�·����
#define ADR     ADC1_SE16    //  ��Ӧ PTE25 ����
void all_init()
{
      camera_init(imgbuff);                                     //��ʼ������ͷ
      adc_init(ADL);      //  ADC�ɼ����ų�ʼ��
      adc_init(ADR);      //
       /*****�����жϷ�����**********/
      set_vector_handler(PORTA_VECTORn , PORTA_IRQHandler);   //���� PORTA ���жϷ�����Ϊ PORTA_IRQHandler
      set_vector_handler(DMA0_VECTORn , DMA0_IRQHandler);     //���� DMA0 ���жϷ�����Ϊ PORTA_IRQHandler
      
      LCD_Init();                                                //Һ����ʼ��
      /********�����ʼ��***********/
      ftm_pwm_init(STEER_FTM ,STEER_CH, STEER_HZ, SteerMidle);    //��� PWM ģ���ʼ��
      /********�����ʼ��**********/
      ftm_pwm_init(MOTOR_FTM, MOTOR3_PWM,MOTOR_HZ,600);           //��ʼ�� ���3 PWM
      ftm_pwm_init(MOTOR_FTM, MOTOR4_PWM,MOTOR_HZ,600);           //��ʼ�� ���4 PWM
}  

