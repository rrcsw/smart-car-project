

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
extern void SearchCenterline();
extern void steer_control();

//fuck you K60
//

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
      Binarization();                  //摄像头二值化      
      LCD_Binarization();            //LCD显示二值化
      SearchCenterline();              //寻找中线
      GetEndParam();             //获取黑线截止行
    
      if(!StartingLineFlag)
              {
               CrossRecognition(&Cross);//十字识别
               CrossConduct();//十字补线
               LoopRecognition(&Loop);//圆环识别
               //LoopIntoRepair();//圆环补线
               //FindLoopOptimalExit();//寻找圆环出口
               //TrackType();//主要赛道类型
               Stop();
              } 
      
   vcan_sendimg(img[0], sizeof(img[0]));          //串口发送中线，注意接收模式        
//# if ObstacleOpen  //如果不需要避障碍，将这个宏定义置0即可
             
//              RecognitionObstacle();
//#endif             
             
               //SteerControl();
//#if OpenLoop              
               
             MotorControlOpenLoop(); 
//#endif
              
//#if CloseLoop      
               //MotorControl();
//#endif
               
             
             
              LCD_Display();//液晶显示
	      DELAY_MS(2000);
                 
               
         }

}
      







