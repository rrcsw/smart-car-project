

#include "common.h"
#include "include.h"
#include "math.h"

uint8 img[CAMERA_H][CAMERA_W];                           //由于鹰眼摄像头是一字节8个像素，因而需要解压为 1
uint8 imgbuff[CAMERA_SIZE];                             //定义存储接收图像的数组

unsigned char StartingLineFlag;

/*****************函数声明******************/
void vcan_sendimg(uint8 *imgaddr, uint32 imgsize);
void img_extract(uint8 *dst, uint8 *src, uint32 srclen);
extern void all_init();   
extern void Binarization();
extern void LCD_Binarization();
extern void move();
extern void steer_control();



void main()
{

  
     all_init();                                       //初始化

/**********     
      while(1)
      {

        ADL_V = adc_once(ADL, ADC_8bit);    //ADC 采集左右两个电感感应电压值
        ADR_V = adc_once(ADR, ADC_8bit);

        steer_error = ADL_V - ADR_V;       //偏差等于左右电感之差
        steer_duty  = STEER_MID + steer_P * steer_error;  //计算舵机输出的 PWM
        ftm_pwm_init(STEER_FTM ,STEER_CH, STEER_HZ, STEER_MID);  //舵机输出控制
**********/ 

while(1)
    {  
      StartingLineFlag=0;
       Binarization();                  //摄像头二值化
       LCD_Binarization();              //LCD显示二值化
       //move();
       
              
           
        /********
          for(int i=59;i>=0;i--)//开始获取中线
    {
      int right_black=79;
      int left_black=0;
      for(int j=40;j>=0;j--)
        if(img[80*i+j]==0x00){ left_black=j;break;}
      for(int j=41;j<80;j++)
        if(img[i*80+j]==0x00){right_black=j;break;}
      mid_line[i]=(right_black+left_black)/2;             //找到左右边界后获取中点
      img[i*80+mid_line[i]]=0x0000;                       //中线置为黑色
    }
    //vcan_sendimg(imgbuff, sizeof(imgbuff));          //串口发送中线，注意接收模式
   
           /************  电机输出  *************/
        /*******
          ftm_pwm_duty( MOTOR_FTM, MOTOR_CH_1, 25);
        ftm_pwm_duty( MOTOR_FTM, MOTOR_CH_2, 0); 
        **********/
    
        /**舵机控制**/
       steer_control();
  
/**************
        site.x=0;site.y=65;
        LCD_Img_gray_Z(site, size, img[0] ,imgsize);             //LCD黑白图像显示
*******/
//#endif
      //  pit_close  (PIT1);                    //关闭PIT（可选择是否关闭）
    
    }
}












