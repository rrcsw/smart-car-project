

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
extern void SearchCenterline();
extern void steer_control();

//fuck you K60
//

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
      Binarization();                  //����ͷ��ֵ��      
      LCD_Binarization();            //LCD��ʾ��ֵ��
      SearchCenterline();              //Ѱ������
      GetEndParam();             //��ȡ���߽�ֹ��
    
      if(!StartingLineFlag)
              {
               CrossRecognition(&Cross);//ʮ��ʶ��
               CrossConduct();//ʮ�ֲ���
               LoopRecognition(&Loop);//Բ��ʶ��
               //LoopIntoRepair();//Բ������
               //FindLoopOptimalExit();//Ѱ��Բ������
               TrackType();//��Ҫ��������
               Stop();
              } 
adc();
    //vcan_sendimg(imgbuff, sizeof(imgbuff));          //���ڷ������ߣ�ע�����ģʽ        
//# if ObstacleOpen  //�������Ҫ���ϰ���������궨����0����
             
  //             RecognitionObstacle();
//#endif             
             
               SteerControl();
//#if OpenLoop              
               
             MotorControlOpenLoop(); 
//#endif
              
//#if CloseLoop      
               //MotorControl();
//#endif
               
             
             
              LCD_Display();//Һ����ʾ
              start_adc();
	      end_adc();
                 
               
         }

}
      







