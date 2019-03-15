#include "common.h"        
#include "include.h"     

/*********** ADC �ɼ��ڶ���  **************/
#define ADL     ADC1_SE16    //  ��Ӧ PTE1 ���� ,����ad�ɼ��������ǵĵ�·����
#define ADR     ADC0_SE16    //  ��Ӧ PTE25 ����


int steer_error_0;          // ����ƫ��
//int steer_duty;           // ������ PWM ���
int steer_P = 8;          // ��� PD ���Ƶ� P ���� 
int ADL_V;                // ������ߵ�еĲɼ�ֵ
int ADR_V;                // �����ұߵ�еĲɼ�ֵ
int steer_error_1;
int steer_error_2 = 0;
int steer_error_3;

void adc(void)
{
LastError = steer_error_0;
ADL_V = adc_once(ADL, ADC_12bit);    //ADC �ɼ�����������и�Ӧ��ѹֵ
ADR_V = adc_once(ADR, ADC_12bit);    
        
steer_error_0 = 100*(ADR_V - ADL_V)/(ADL_V + ADR_V);       //ƫ��������ҵ��֮��
steer_error_1 = steer_error_0-LastError;
steer_error_2 = steer_error_1 + steer_error_2;
steer_error_3 = steer_error_2 + 1000;
//steer_duty  = S3010_MID + steer_P * steer_error;  //����������� PWM
}