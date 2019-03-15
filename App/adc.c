#include "common.h"        
#include "include.h"     

/*********** ADC 采集口定义  **************/
#define ADL     ADC1_SE16    //  对应 PTE1 引脚 ,具体ad采集口由你们的电路决定
#define ADR     ADC0_SE16    //  对应 PTE25 引脚


int steer_error_0;          // 保存偏差
//int steer_duty;           // 保存舵机 PWM 输出
int steer_P = 8;          // 舵机 PD 控制的 P 参数 
int ADL_V;                // 保存左边电感的采集值
int ADR_V;                // 保存右边电感的采集值
int steer_error_1;
int steer_error_2 = 0;
int steer_error_3;

void adc(void)
{
LastError = steer_error_0;
ADL_V = adc_once(ADL, ADC_12bit);    //ADC 采集左右两个电感感应电压值
ADR_V = adc_once(ADR, ADC_12bit);    
        
steer_error_0 = 100*(ADR_V - ADL_V)/(ADL_V + ADR_V);       //偏差等于左右电感之差
steer_error_1 = steer_error_0-LastError;
steer_error_2 = steer_error_1 + steer_error_2;
steer_error_3 = steer_error_2 + 1000;
//steer_duty  = S3010_MID + steer_P * steer_error;  //计算舵机输出的 PWM
}