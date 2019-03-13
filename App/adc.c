#include "common.h"        
#include "include.h"     

/*********** ADC 采集口定义  **************/
#define ADL     ADC0_SE16    //  对应 PTE1 引脚 ,具体ad采集口由你们的电路决定
#define ADR     ADC1_SE16    //  对应 PTE25 引脚


int steer_error_0;          // 保存偏差
//int steer_duty;           // 保存舵机 PWM 输出
int steer_P = 8;          // 舵机 PD 控制的 P 参数 
int ADL_V;                // 保存左边电感的采集值
int ADR_V;                // 保存右边电感的采集值

void adc(void)
{
ADL_V = adc_once(ADL, ADC_8bit);    //ADC 采集左右两个电感感应电压值
ADR_V = adc_once(ADR, ADC_8bit);    
        
steer_error_0 = 1000+1000*(ADL_V - ADR_V)/(ADL_V + ADR_V);       //偏差等于左右电感之差
//steer_duty  = S3010_MID + steer_P * steer_error;  //计算舵机输出的 PWM
}