#include "common.h"
#include "include.h"


/**********���PID*******/
int steer_error;          // ����ƫ��
int steer_duty;           // ������ PWM ���
extern  uint8   mid_line[RowMax+1];                                //��������
/***************�������˿�************************/
#define STEER_FTM   FTM1                                   //PTA12
#define STEER_CH    FTM_CH0                                //������� 10000
#define STEER_HZ    50                                    //���Ƶ�� 100 Hz 100
//#define STEER_MID   3940                               //������ֵ��ʵ����ֵ�������ǳ�װ��֮��ϸ�� 1520
#define angle_line (30)                                   //�����е��������ڴ��

/***
void steer_control()
{
   steer_duty  = STEER_MID ;+ steer_P*(mid_line[angle_line]-40);  //����������� PWM
   ftm_pwm_duty(STEER_FTM ,STEER_CH, steer_duty);  //����������
}
**/

/*********define for SteerControl**********/

float  KP=12;//����������ϵ����Ӱ�����Ĵ�Ƿ�Χ//15
float  KD=8;//10//7.5//�������΢��ϵ��,Ӱ�����Ĵ�Ƿ�Ӧ
float  SteerPwmAdd=0.0;//���pwm����
float  Error;//ƫ��ֵ
float  LastError;//�ϴε�ƫ��
float  WeightSum=0;
float  CenterMeanValue=0;
float  CenterSum=0;
float  J=0.0300;//����p��ƫ��Ĺ�ϵ��Խ������Խǿ
float  JD=0.0400;//����p��ƫ��Ĺ�ϵ��Խ������Խǿ
float  BasicP=3.0; //������Pֵ
float  BasicD=6.5; //������Pֵ
uint32 SteerPwm=0,LastSteerSwm=0;//�����pwmֵ���ϴζ����pwmֵ

//��Ȩƽ����Ȩֵ��ѡȡ
 #if 0
float Weight[60]={ 
                    0,0,0,0,0,0,0,0,0,0,     //0-9�У������ò���
                    0,0,0,0,0,0,2,2,2,2,  //0-19�У������ò���
                   
                   3.8,3.8,1.8,1.8,2.8,3.3,2.3,3.3,3.3,3.3,//20-29��

                   //1.05,1.05,1.05,1.05,1.05,2,2,2,2,2,
                    
                   2,2,2,2,2,2.0,2.0,2.5,2.5,2.5,//30-39��
            
                   2.1,2.1,2.1,1,1,2,1,1,1,1,//40-49��
                  1,0,0,0,0,};//���ʮ��*///��ѹ�ߣ��������
#endif

#if 1

float Weight[60]={ 
                     0,0,0,0,0,0,0,0,0,0,              //0-9�У������ò���
                    
                     2,2,2,2,2,1.5,1,1.5,1,             //0-19�У������ò���
                  
                     2.5,1,2.2,1,2.5,1,2,2,1,2,            //20-29��
                    
                     1,3,2,2,1,2,1,2,2,1,              //30-39��

                     3,1,2,3,3,2,3,1,1,             //40-49��
                   
                     1,1,1,1,1,1,1,1,1,1,};          //���ʮ��




#endif

#if 1
void CalculateError(void)

{
  
            int i;
            
            CenterSum=0;
            
            CenterMeanValue=0;
            
            WeightSum=0; 
        
       /****б��ʮ���ж�****     
      if(Cross.LeftSideling) 
      {
        for(i=58;i>InflectionPointL.InflectionPointRow;i--)
        {
               CenterSum+=mid_line[i]*Weight[i];             
               WeightSum+=Weight[i];
        }
      }
      
      else if(Cross.RightSideling)
      {
       for(i=58;i>InflectionPointR.InflectionPointRow;i--)
        {
               CenterSum+=mid_line[i]*Weight[i];             
               WeightSum+=Weight[i];
        }
      
      }
    ****/
      /***else***/ if(StrightIntoCrossL)
      {
         for(i=88;i>InflectionPointL.InflectionPointRow;i--)
        {
               CenterSum+=mid_line[i]*Weight[i];             
               WeightSum+=Weight[i];
        }
      
      }
      
      else if(StrightIntoCrossR)
      {
        for(i=88;i>InflectionPointR.InflectionPointRow;i--)
        {
               CenterSum+=mid_line[i]*Weight[i];             
               WeightSum+=Weight[i];
        }
      
      }
      else if(StrightIntoCrossLR)
      {
        for(i=88;i>(InflectionPointR.InflectionPointRow+InflectionPointR.InflectionPointRow)/2;i--)
        {
               CenterSum+=mid_line[i]*Weight[i];             
               WeightSum+=Weight[i];
        }
      
      }
      

       else if(Cross.CrossFlag)
       {
         for(i=45;i>LastLine;i--)
         {
               CenterSum+=mid_line[i]*Weight[i];             
               WeightSum+=Weight[i];
         }
       
       }
      
      else
      {
            
        for(i=87;i>LastLine;i--)
          
        {      
               CenterSum+=mid_line[i]*Weight[i];
              
               WeightSum+=Weight[i];       
         }
      
      }
      
       if(WeightSum!=0)
         
       {
             CenterMeanValue=(CenterSum/WeightSum);//�����Ȩƽ�������ߵ�ֵ
           
        }
            
             LastError=Error;
      
             Error=(600-CenterMeanValue);// һ��ͼ��ƫ��ֵ 
             
             if(Error>=30.0)//ƫ���޷�
                
               Error=30.0;
             
             if(Error<=-30.0)
               
               Error=-30.0; 
             
            KP=BasicP+(Error* Error)*J;//��̬����pģ��
             
             if(KP>=13) KP=13;//pֵ�޷�
             
             KD=BasicD+(Error* Error)*JD;
             
              if(KD>=20) KD=20;//pֵ�޷�
                    
}
                     
#endif                
     



/****************************************************** 
 *��������SteerControl
 *
 *���ܣ��������
 * 
 * ��ڲ�������
 * 
 *���ز�������
 *
 * ���ߣ�XGL
 * 
 * ���ڣ�2016-3-01(�Ѳ���)
 *******************************************************/



void SteerControl(void)
{
    
        CalculateError(); 
        NormalControl();
        TurnBack();
          
           ftm_pwm_duty(FTM1,STEER_CH,SteerPwm);//���pwm����
           
           LastSteerSwm=SteerPwm;//��¼pwmֵ
                    
}

void NormalControl()
{
   SteerPwmAdd=-((KP*Error)+KD*(Error-LastError));//�����pd������
       
        if(SteerPwmAdd>=150)
          
           SteerPwmAdd=150;
        
        if(SteerPwmAdd<=-150)
          
           SteerPwmAdd=-150;
            
        SteerPwm=(uint32)(SteerPwmAdd+SteerMidle);
        
          
        if(SteerPwm>=SteerMax)//�޷�
         
              SteerPwm=SteerMax;
       
        if(SteerPwm<=SteerMin)
          
              SteerPwm=SteerMin;
}

void TurnBack()
{
  extern uint16 i;
  extern uint16 j;
  extern uint32 SteerPwm;
  int LoseLeft=0;
  int LoseRight=0;
  
  for(i=RowMax;i>=85;i--)//������ǰ���У�ȫ��ɨ��
  {
    if(img[90][j]==White_Point && img[90][j+1]==Black_Point&&img[90][j+2]==Black_Point)
    {
      if(img[85][0]==Black_Point&&img[85][120]==Black_Point)
      {  
      LoseLeft=120-j;
      SteerPwm=SteerMin;//�������
      }
      else
      {
        LoseLeft=120-j;
      }
    }
     else if(img[90][j]==White_Point && img[90][j-1]==Black_Point&&img[90][j-2]==Black_Point)
     {
       if(img[85][0]==Black_Point&&img[85][120]==Black_Point)
       {
         LoseRight=j;
         SteerPwm=SteerMax;//���Ҵ���
       }
       else
       {
         LoseRight=j;
       }
     }
     
  
      if(LoseLeft>=50)
      {
        SteerPwm=SteerMin;//�������
      }
      else if(LoseRight>=50)
        {
        SteerPwm=SteerMax;//���Ҵ���
      }
    
  }
  
}