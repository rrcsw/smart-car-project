#include "common.h"        
#include "include.h"     

/*********** ADC �ɼ��ڶ���  **************/
#define ADL     ADC0_SE16    //  ��Ӧ PTE1 ���� ,����ad�ɼ��������ǵĵ�·����
#define ADR     ADC1_SE16    //  ��Ӧ PTE25 ����


int steer_error_0;          // ����ƫ��
//int steer_duty;           // ������ PWM ���
int steer_P = 8;          // ��� PD ���Ƶ� P ���� 
int ADL_V;                // ������ߵ�еĲɼ�ֵ
int ADR_V;                // �����ұߵ�еĲɼ�ֵ

void adc(void)
{
ADL_V = adc_once(ADL, ADC_8bit);    //ADC �ɼ�����������и�Ӧ��ѹֵ
ADR_V = adc_once(ADR, ADC_8bit);    
        
steer_error_0 = 1000+1000*(ADL_V - ADR_V)/(ADL_V + ADR_V);       //ƫ��������ҵ��֮��
//steer_duty  = S3010_MID + steer_P * steer_error;  //����������� PWM
}