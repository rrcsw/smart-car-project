#include "common.h"
#include "include.h"


/**********舵机PID*******/
int steer_error;          // 保存偏差
int steer_duty;           // 保存舵机 PWM 输出
int steer_P = 8;          // 舵机 PD 控制的 P 参数 
uint8 mid_line[61];                                //中线数组
/***************定义舵机端口************************/
#define STEER_FTM   FTM1                                   //PTA12
#define STEER_CH    FTM_CH0                                //舵机精度 10000
#define STEER_HZ    (300)                                  //舵机频率 100 Hz 100
#define STEER_MID   3940                               //理论中值，实际中值得在你们车装好之后细调 1520
#define angle_line (30)                                   //把这行的数据用于打角


void steer_control()
{
   steer_duty  = STEER_MID ;+ steer_P*(mid_line[angle_line]-40);  //计算舵机输出的 PWM
   ftm_pwm_duty(STEER_FTM ,STEER_CH, steer_duty);  //舵机输出控制
}