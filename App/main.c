

#include "common.h"
#include "include.h"
#include "math.h"

uint8 img[CAMERA_H][CAMERA_W];                           //����ӥ������ͷ��һ�ֽ�8�����أ������Ҫ��ѹΪ 1
uint8 imgbuff[CAMERA_SIZE];                             //����洢����ͼ�������

unsigned char StartingLineFlag;

/*****************��������******************/
void vcan_sendimg(uint8 *imgaddr, uint32 imgsize);
void img_extract(uint8 *dst, uint8 *src, uint32 srclen);
extern void all_init();   
extern void Binarization();
extern void LCD_Binarization();
extern void move();
extern void steer_control();



void main()
{

  
     all_init();                                       //��ʼ��

/**********     
      while(1)
      {

        ADL_V = adc_once(ADL, ADC_8bit);    //ADC �ɼ�����������и�Ӧ��ѹֵ
        ADR_V = adc_once(ADR, ADC_8bit);

        steer_error = ADL_V - ADR_V;       //ƫ��������ҵ��֮��
        steer_duty  = STEER_MID + steer_P * steer_error;  //����������� PWM
        ftm_pwm_init(STEER_FTM ,STEER_CH, STEER_HZ, STEER_MID);  //����������
**********/ 

while(1)
    {  
      StartingLineFlag=0;
       Binarization();                  //����ͷ��ֵ��
       LCD_Binarization();              //LCD��ʾ��ֵ��
       //move();
       
              
           
        /********
          for(int i=59;i>=0;i--)//��ʼ��ȡ����
    {
      int right_black=79;
      int left_black=0;
      for(int j=40;j>=0;j--)
        if(img[80*i+j]==0x00){ left_black=j;break;}
      for(int j=41;j<80;j++)
        if(img[i*80+j]==0x00){right_black=j;break;}
      mid_line[i]=(right_black+left_black)/2;             //�ҵ����ұ߽���ȡ�е�
      img[i*80+mid_line[i]]=0x0000;                       //������Ϊ��ɫ
    }
    //vcan_sendimg(imgbuff, sizeof(imgbuff));          //���ڷ������ߣ�ע�����ģʽ
   
           /************  ������  *************/
        /*******
          ftm_pwm_duty( MOTOR_FTM, MOTOR_CH_1, 25);
        ftm_pwm_duty( MOTOR_FTM, MOTOR_CH_2, 0); 
        **********/
    
        /**�������**/
       steer_control();
  
/**************
        site.x=0;site.y=65;
        LCD_Img_gray_Z(site, size, img[0] ,imgsize);             //LCD�ڰ�ͼ����ʾ
*******/
//#endif
      //  pit_close  (PIT1);                    //�ر�PIT����ѡ���Ƿ�رգ�
    
    }
}












