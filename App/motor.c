
#include "common.h"
#include "include.h"
#include "motor.h"

int16  NumPulseFTM,NumPulselptmr;//��������Ĵ�����ֵ
int32  RightMotorPulseAccumulate;
int32  LeftMotorPulseAccumulate;
int32  GetRightMotorPules,GetLeftMotorPules; //��õ�һ����������������PID��������ݴ�����
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


/****�����ʼ��******/
/*****
void MotorInit(void)
{
  gpio_init (PTB18, GPO, 0);//0��ת��1��ת
  ftm_pwm_init(FTM0, FTM_CH0, 1000,0);//PTC1���ҵ��
   
  gpio_init (PTB9, GPO, 0);
  ftm_pwm_init(FTM0, FTM_CH1, 1000,0);//PTC2,����  
}
******/


#if OpenLoop//�������Ʒ�ʽ

int32  OpendLoopPwmB; //������ߵ����PWMֵ
int32  OpendLoopPwmF; //�����ұߵ����PWMֵ


//�������Ʒ�Ϊ������λ��ͨ��3�Ų��뿪��ѡ��
//1.ռ�ձȲ����Ŀ�������  2.�������٣�����ƫ�����

void MotorControlOpenLoop(void)
{
//  int ErrorError=0;
  
  
     OpendLoopPwmB=100;
     OpendLoopPwmF=175;
    /****
      //����
      OpendLoopPwmB=(int)(OpendLoopPwmB-(OpendLoopPwmB*Error*000.1));
      
      if(OpendLoopPwmB>=400)  OpendLoopPwmB=400;
      if(OpendLoopPwmB<=200)    OpendLoopPwmB=200;
      
      OpendLoopPwmF=(int)(OpendLoopPwmF+(OpendLoopPwmF*Error*000.1));
            
      if(OpendLoopPwmF>=400)  OpendLoopPwmF=400;
      if(OpendLoopPwmF<=200)    OpendLoopPwmF=200;
      ***/
      ftm_pwm_duty(FTM0,FTM_CH5,OpendLoopPwmF);
      ftm_pwm_duty(FTM0,FTM_CH6,OpendLoopPwmB); //PTC2,���� 

  }
#endif 

void Stop()
{
  if (BlackEndMaxMax>=70)
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




/********

#if CloseLoop //�ջ�����


void GetTargetSpeed(void)
{
  
  if(DialSwitch_2)//���Ų��뿪�ز�����ȥ,��̬�ٶ�
  {
    
    LastSpeedDropRow=SpeedDropRow;
    
     SpeedDropRow=(int)(BlackEndM);
     
     if(SpeedDropRow>=55) 
        SpeedDropRow=55;

     else if(SpeedDropRow<35) 
         SpeedDropRow=35;//������ٺܺ�

     //SpeedSet=(int)(0.05*(SpeedDropRow*SpeedDropRow)-(ABS(Error*Error)*0.02));//0.045,0.020//0.048,0.022
     SpeedSet=(int)(1.4*SpeedDropRow+10*(SpeedDropRow-LastSpeedDropRow)-(0.3*ABS(Error)+1.0*(Error-LastError)));
     
     if(SpeedSet<=45)  SpeedSet=45;
     if(SpeedSet>=65)  SpeedSet=65; 
    
    if(ABS(Error)>=10) //ƫ�����ĳ��ֵ�Ž��в���
    {
      Differential_P=(float)(30.0/((60-(LastLine+4))*(60-(LastLine+4))));//30
      LSpeedSet=(int32)(SpeedSet-(Differential_P*Error*SpeedSet)) ;//���ֲ���
       if(LSpeedSet<=5)    LSpeedSet=5;
       if(LSpeedSet>=110)  LSpeedSet=110; 
       RSpeedSet=(int32)(SpeedSet+(Differential_P*Error*SpeedSet) );//���ֲ���
       if(RSpeedSet<=5)    RSpeedSet=5;
       if(RSpeedSet>=110)  RSpeedSet=110; 
    } 
    else
    {
        LSpeedSet=SpeedSet ;
        RSpeedSet=SpeedSet ;
    }
    
    }
  
  else if(!DialSwitch_2)//���Ų��뿪�����ϲ�
  {
       SpeedSet=50;
       SpeedP=40.0;//50.0;40
       SpeedI= 0.0006;//16.0;50,0.0006
       SpeedD=10.0;//1.3,10.0

    if(ABS(Error)>=10)
    {
      //Differential_P=(float)(30.0/((60-(AvaliableLines+10))*(60-(AvaliableLines+10))));
       SpeedSet=50;
      
       Differential_P=0.0180;//�����٣���̫�������
       LSpeedSet=(int32)(SpeedSet-(Differential_P*Error*SpeedSet)) ;//���ֲ���
       if(LSpeedSet<=20)    LSpeedSet=20;
       if(LSpeedSet>=80)  LSpeedSet=80; 
       RSpeedSet=(int32)(SpeedSet+(Differential_P*Error*SpeedSet) );//���ֲ���
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
//�����ٶ�ƫ��
   
void CalculateMotorSpeedError(float LeftMotorTarget,float RightMotorTarget)
{ 
    SpeedPerErrorL=SpeedLastErrorL;
    SpeedLastErrorL=SpeedErrorL;
    SpeedErrorL=LeftMotorTarget-GetLeftMotorPules; 
    SpeedPerErrorR=SpeedLastErrorR;
    SpeedLastErrorR=SpeedErrorR ;
    SpeedErrorR=RightMotorTarget-GetRightMotorPules;
}

//����ʽPID�����㷨

void MotorControl(void)
{
       GetTargetSpeed();
       CalculateMotorSpeedError(LSpeedSet,RSpeedSet) ;//�趨Ŀ���ٶȼ���ƫ��
       MotorPwmR+=(SpeedP+SpeedI+SpeedD)*SpeedErrorR-(SpeedP+2*SpeedD)*SpeedLastErrorR+SpeedD*SpeedPerErrorR;
       MotorPwmRight=(int)(MotorPwmR);
       MotorPwmL+=(SpeedP+SpeedI+SpeedD)*SpeedErrorL-(SpeedP+2*SpeedD)*SpeedLastErrorL+SpeedD*SpeedPerErrorL;
       MotorPwmLeft=(int)(MotorPwmL);
       if(MotorPwmLeft<=0)               MotorPwmLeft=0;
       if(MotorPwmLeft>=220)             MotorPwmLeft=220;
       if(MotorPwmRight<=0)              MotorPwmRight=0;
       if(MotorPwmRight>=220)            MotorPwmRight=220; 
       ftm_pwm_duty(FTM0,FTM_CH0,MotorPwmRight);
       ftm_pwm_duty(FTM0,FTM_CH1,MotorPwmLeft); //PTC2,����       
}

    








void SpeedGet(void)

{
    NumPulselptmr=lptmr_pulse_get(); 
    lptmr_pulse_clean();//�����Ĵ�������
    LeftMotorPulseAccumulate+=NumPulselptmr;//��ÿһ��PIT��ʱʱ����������ۼӵ�������������pulse_period_L  

    
    NumPulseFTM=ftm_quad_get(FTM2);//ȡ�Ĵ�����ֵ
    ftm_quad_clean(FTM2);//�����Ĵ�������
    RightMotorPulseAccumulate+=NumPulseFTM;//��������    ��������������壩���� 
}


void PIT0_IRQHandler()
{  
  
             static uint16  TimerCnt8ms=0;
           
              disable_irq(PIT0_IRQn);            //��PIT�ж�
            
              PIT_Flag_Clear(PIT0);                           //���жϱ�־λ
              
              TimerCnt8ms++;
                 
              SpeedGet();                        //ÿ�ν����ۼ�һ���ٶ�
                
       if(TimerCnt8ms>=MotorControlPeriod )//һ��8ms����ȡһ���ٶȣ����������ٶ�
         
       { 
               
              GetRightMotorPules=-RightMotorPulseAccumulate;  //���ֺ���β�õ�������10msһ����õ�����
              GetLeftMotorPules=LeftMotorPulseAccumulate;
           
              TimerCnt8ms=0;//����
                
              RightMotorPulseAccumulate=0;//�ۼ�ֵ���㣬
              LeftMotorPulseAccumulate=0;
        }
         
    enable_irq(PIT0_IRQn);//ʹ���ж�	
}





