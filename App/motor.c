
#include "common.h"
#include "include.h"
#include "motor.h"

int16  NumPulseFTM,NumPulselptmr;//脉冲计数寄存器的值
int32  RightMotorPulseAccumulate;
int32  LeftMotorPulseAccumulate;
int32  GetRightMotorPules,GetLeftMotorPules; //获得的一次脉冲总数（用于PID电机的数据处理）
int SpeedDropRow=0;
int LastSpeedDropRow=0;
int32 LSpeedSet=0;//60;//2.5M/S
int32 RSpeedSet=0;
int32 SpeedSet=0;

float SpeedErrorL=0;
float SpeedErrorR=0;

float SpeedLastErrorL=0;
float SpeedLastErrorR=0;

float SpeedPerErrorL=0;
float SpeedPerErrorR=0;

int Error1=0,Error1Last=0;
int SlowSpeed=0;
int Slowing=0;


#if 1

float  SpeedP=75.0;//50.0;40
float  SpeedI= 0.0006;//16.0;50,0.0006
float  SpeedD=9.0;//1.3,10.0

#endif


float  Differential_P=0.0;//0.0250,0.1050

int MotorPwmL=0;
int MotorPwmR=0;

int MotorPwmRight=0;
int MotorPwmLeft=0;

float LeftMotorPwmAdd=0;
float RightMotorPwmAdd=0;
float MotorError=0.0;

/****电机初始化******/
/*****
void MotorInit(void)
{
  gpio_init (PTB18, GPO, 0);//0正转，1反转
  ftm_pwm_init(FTM0, FTM_CH0, 1000,0);//PTC1，右电机
   
  gpio_init (PTB9, GPO, 0);
  ftm_pwm_init(FTM0, FTM_CH1, 1000,0);//PTC2,左电机  
}
******/


#if OpenLoop//开环控制方式

int32  OpendLoopPwmB; //开环左边电机的PWM值
int32  OpendLoopPwmF; //开环右边电机的PWM值


//开环控制分为两个档位，通过3号拨码开关选择
//1.占空比不会变的开环匀速  2.开环变速，根据偏差拟和

void MotorControlOpenLoop(void)
{
//  int ErrorError=0;
  MotorError=-ABS(Error);

  if(plan==1)
  { OpendLoopPwmB=100;
     OpendLoopPwmF=240;
   
    

      //差速
     // OpendLoopPwmB=(int)(OpendLoopPwmB-(OpendLoopPwmB*Error*000.1));
      
      //if(OpendLoopPwmB>=400)  OpendLoopPwmB=400;
     // if(OpendLoopPwmB<=200)    OpendLoopPwmB=200;
      
      OpendLoopPwmF=(int)(OpendLoopPwmF+(OpendLoopPwmF*Error*000.1));
            
      if(OpendLoopPwmF>=250)  OpendLoopPwmF=250;
      if(OpendLoopPwmF<=185)    OpendLoopPwmF=185;
      
      ftm_pwm_duty(FTM0,FTM_CH5,OpendLoopPwmB);
      ftm_pwm_duty(FTM0,FTM_CH6,OpendLoopPwmF); //PTC2,左电机 
  }
  if(plan==2)
  {
    OpendLoopPwmB=100;
    OpendLoopPwmF=175;
    ftm_pwm_duty(FTM0,FTM_CH5,OpendLoopPwmB);
    ftm_pwm_duty(FTM0,FTM_CH6,OpendLoopPwmF); 
  }
}
#endif 

void Stop()
{
  if (BlackEndMaxMax>=80)
  {
    ftm_pwm_duty(FTM0,FTM_CH5,100);
    ftm_pwm_duty(FTM0,FTM_CH6,100);
  }
  else
  {
      ftm_pwm_duty(FTM0,FTM_CH5,OpendLoopPwmF);
      ftm_pwm_duty(FTM0,FTM_CH6,OpendLoopPwmB);
  }
}

//



/********

#if CloseLoop //闭环控制


void GetTargetSpeed(void)
{
  
  if(DialSwitch_2)//二号拨码开关不拨上去,动态速度
  {
    
    LastSpeedDropRow=SpeedDropRow;
    
     SpeedDropRow=(int)(BlackEndM);
     
     if(SpeedDropRow>=55) 
        SpeedDropRow=55;

     else if(SpeedDropRow<35) 
         SpeedDropRow=35;//急弯减速很好

     //SpeedSet=(int)(0.05*(SpeedDropRow*SpeedDropRow)-(ABS(Error*Error)*0.02));//0.045,0.020//0.048,0.022
     SpeedSet=(int)(1.4*SpeedDropRow+10*(SpeedDropRow-LastSpeedDropRow)-(0.3*ABS(Error)+1.0*(Error-LastError)));
     
     if(SpeedSet<=45)  SpeedSet=45;
     if(SpeedSet>=65)  SpeedSet=65; 
    
    if(ABS(Error)>=10) //偏差大于某个值才进行差速
    {
      Differential_P=(float)(30.0/((60-(LastLine+4))*(60-(LastLine+4))));//30
      LSpeedSet=(int32)(SpeedSet-(Differential_P*Error*SpeedSet)) ;//左轮差速
       if(LSpeedSet<=5)    LSpeedSet=5;
       if(LSpeedSet>=110)  LSpeedSet=110; 
       RSpeedSet=(int32)(SpeedSet+(Differential_P*Error*SpeedSet) );//右轮差速
       if(RSpeedSet<=5)    RSpeedSet=5;
       if(RSpeedSet>=110)  RSpeedSet=110; 
    } 
    else
    {
        LSpeedSet=SpeedSet ;
        RSpeedSet=SpeedSet ;
    }
    
    }
  
  else if(!DialSwitch_2)//二号拨码开关往上波
  {
       SpeedSet=50;
       SpeedP=40.0;//50.0;40
       SpeedI= 0.0006;//16.0;50,0.0006
       SpeedD=10.0;//1.3,10.0

    if(ABS(Error)>=10)
    {
      //Differential_P=(float)(30.0/((60-(AvaliableLines+10))*(60-(AvaliableLines+10))));
       SpeedSet=50;
      
       Differential_P=0.0180;//调差速，调太大会跳轮
       LSpeedSet=(int32)(SpeedSet-(Differential_P*Error*SpeedSet)) ;//左轮差速
       if(LSpeedSet<=20)    LSpeedSet=20;
       if(LSpeedSet>=80)  LSpeedSet=80; 
       RSpeedSet=(int32)(SpeedSet+(Differential_P*Error*SpeedSet) );//右轮差速
       if(RSpeedSet<=20)    RSpeedSet=20;
       if(RSpeedSet>=80)  RSpeedSet=80; 
    } 
    else
    {
        LSpeedSet=SpeedSet ;
        RSpeedSet=SpeedSet ;
    }
    
  
  }
      
}

*********/
//计算速度偏差
   
void CalculateMotorSpeedError(float LeftMotorTarget,float RightMotorTarget)
{ 
    SpeedPerErrorL=SpeedLastErrorL;
    SpeedLastErrorL=SpeedErrorL;
    SpeedErrorL=LeftMotorTarget-GetLeftMotorPules; 
    SpeedPerErrorR=SpeedLastErrorR;
    SpeedLastErrorR=SpeedErrorR ;
    SpeedErrorR=RightMotorTarget-GetRightMotorPules;
}

//增量式PID控制算法

void MotorControl(void)
{
       GetTargetSpeed();
       CalculateMotorSpeedError(LSpeedSet,RSpeedSet) ;//设定目标速度计算偏差
       MotorPwmR+=(SpeedP+SpeedI+SpeedD)*SpeedErrorR-(SpeedP+2*SpeedD)*SpeedLastErrorR+SpeedD*SpeedPerErrorR;
       MotorPwmRight=(int)(MotorPwmR);
       MotorPwmL+=(SpeedP+SpeedI+SpeedD)*SpeedErrorL-(SpeedP+2*SpeedD)*SpeedLastErrorL+SpeedD*SpeedPerErrorL;
       MotorPwmLeft=(int)(MotorPwmL);
       if(MotorPwmLeft<=0)               MotorPwmLeft=0;
       if(MotorPwmLeft>=220)             MotorPwmLeft=220;
       if(MotorPwmRight<=0)              MotorPwmRight=0;
       if(MotorPwmRight>=220)            MotorPwmRight=220; 
       ftm_pwm_duty(FTM0,FTM_CH0,MotorPwmRight);
       ftm_pwm_duty(FTM0,FTM_CH1,MotorPwmLeft); //PTC2,左电机       
}

    








void SpeedGet(void)

{
    NumPulselptmr=lptmr_pulse_get(); 
    lptmr_pulse_clean();//计数寄存器清零
    LeftMotorPulseAccumulate+=NumPulselptmr;//把每一次PIT定时时间的脉冲数累加到脉冲总数变量pulse_period_L  

    
    NumPulseFTM=ftm_quad_get(FTM2);//取寄存器的值
    ftm_quad_clean(FTM2);//计数寄存器清零
    RightMotorPulseAccumulate+=NumPulseFTM;//单相脉冲    （正交解码测脉冲）右轮 
}


void PIT0_IRQHandler()
{  
  
             static uint16  TimerCnt8ms=0;
           
              disable_irq(PIT0_IRQn);            //关PIT中断
            
              PIT_Flag_Clear(PIT0);                           //清中断标志位
              
              TimerCnt8ms++;
                 
              SpeedGet();                        //每次进来累加一次速度
                
       if(TimerCnt8ms>=MotorControlPeriod )//一个8ms周期取一次速度，当作车的速度
         
       { 
               
              GetRightMotorPules=-RightMotorPulseAccumulate;  //保持好这次测得的总数，10ms一共测得的脉冲
              GetLeftMotorPules=LeftMotorPulseAccumulate;
           
              TimerCnt8ms=0;//清零
                
              RightMotorPulseAccumulate=0;//累加值清零，
              LeftMotorPulseAccumulate=0;
        }
         
    enable_irq(PIT0_IRQn);//使能中断	
}





