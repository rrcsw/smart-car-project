#include "common.h"
#include "include.h"


/**********定义电机端口*********/
#define MOTOR_FTM   FTM0
#define MOTOR3_PWM  FTM_CH5
#define MOTOR4_PWM  FTM_CH6
#define MOTOR_HZ    20000//(20*1000)

/***************定义舵机端口************************/
#define STEER_FTM   FTM1                                   //PTA12
#define STEER_CH    FTM_CH0                                //舵机精度 10000
#define STEER_HZ    50                                  //舵机频率 100 Hz 100
//define STEER_MID   3940                               //理论中值，实际中值得在你们车装好之后细调 1520

/*******定义摄像头********/
extern uint8 imgbuff[CAMERA_SIZE];                         
/********定义函数********/
void PORTA_IRQHandler();
void DMA0_IRQHandler();

void all_init()
{
      camera_init(imgbuff);                                     //初始化摄像头
       /*****配置中断服务函数**********/
      set_vector_handler(PORTA_VECTORn , PORTA_IRQHandler);   //设置 PORTA 的中断服务函数为 PORTA_IRQHandler
      set_vector_handler(DMA0_VECTORn , DMA0_IRQHandler);     //设置 DMA0 的中断服务函数为 PORTA_IRQHandler
      
      LCD_Init();                                                //液晶初始化
      /********舵机初始化***********/
      ftm_pwm_init(STEER_FTM ,STEER_CH, STEER_HZ, SteerMidle);    //舵机 PWM 模块初始化
      /********电机初始化**********/
      ftm_pwm_init(MOTOR_FTM, MOTOR3_PWM,MOTOR_HZ,600);           //初始化 电机3 PWM
      ftm_pwm_init(MOTOR_FTM, MOTOR4_PWM,MOTOR_HZ,600);           //初始化 电机4 PWM
}  

