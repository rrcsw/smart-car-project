#include "common.h"
#include "include.h"
#include "math.h"
#include "steer.h"

extern int32 time1,time2;
extern uint8 img[CAMERA_H][CAMERA_W];
extern uint8 imgbuff[CAMERA_SIZE]; 
extern uint8 right_black[120];
extern uint8 left_black[120];
uint8  mid_line[91];
extern int RightLose;
extern int AllLose;
extern float  WeightSum;
extern float  CenterMeanValue;
extern float  CenterSum;
extern uint8 image_threshold;
extern int controlplan;







 /*LCDҺ����������*/
    Site_t site = {0, 0};                           //LCD��ʾͼ�����Ͻ�λ��
    Size_t imgsize  = {CAMERA_W, CAMERA_H};             //LCDͼ���С
    Size_t size={120,90};                                //LCD��ʾ����ͼ���С


void LCD_Binarization()
{
        //site.x=80;site.y=0;
        //LCD_num_BC(site,time1,8,BLUE,RED);                 //us//��ȡͼ��ʱ��
       // site.x=80;site.y=65;
        //LCD_num_BC(site,time2,8,BLUE,RED);                 //us//��ֵ��ʱ��
        
       // site.x=0;site.y=0;
      // LCD_Img_gray_Z(site, size, imgbuff,imgsize);            //LCD�Ҷ�ͼ����ʾ
        site.x=0;site.y=0;
        LCD_Img_gray_Z(site, size, img[0] ,imgsize);             //LCD�ڰ�ͼ����ʾ
        /*
        site.x=0;site.y=100;
        LCD_num_BC(site,Error,8,BLUE,RED);           //ƫ��
        
        site.x=20;site.y=100;
        LCD_num_BC(site,image_threshold,8,BLUE,RED);        //��ֵ         //������  ��ֵ690
        */
        //site.x=50;site.y=100;
        //LCD_num_BC(site,RightLose,8,BLUE,RED);         //�ұ߽綪ʧ����
        
        //site.x=70;site.y=100;
        //LCD_num_BC(site,AllLose,8,BLUE,RED);            //����ȫ������
        /*
        site.x=50;site.y=100;
        LCD_num_BC(site,CenterMeanValue,8,BLUE,RED);            //���߼�Ȩֵ
        
        site.x=70;site.y=100;
        LCD_num_BC(site,SteerPwmAdd,8,BLUE,RED);  //Pwm�ı���
        
       site.x=100;site.y=100;
        LCD_num_BC(site,KP,8,BLUE,RED);//kp
        */
        site.x=110;site.y=100;
        LCD_num_BC(site,controlplan,8,BLUE,RED);//kd
        
        
        
        
}

void LCD_Display()
{
    
       {Site_t lcdsite;uint8 i;
         for(i=0;i<90;i++) //����ɨ����
         {
           lcdsite.y=i;//����������
           lcdsite.x= mid_line[i]; LCD_point(lcdsite,RED); //��ɫ����
           lcdsite.x= LeftEdge[i]; LCD_point(lcdsite,BLUE); //��ɫ��߽�
           lcdsite.x= RightEdge[i]; LCD_point(lcdsite,YELLOW); //��ɫ�ұ߽�
           lcdsite.x=59; LCD_point(lcdsite,GREEN); //��ɫ��������ж���׼
         }
         for(i=0;i<59;i++)//����ɨ����
         {
           lcdsite.x=i;//���ƺ�����
           lcdsite.y=44;
           LCD_point(lcdsite,GREEN); //���ʶ����
         }
       }
}

void LCD_Init()
{
    /*LCDҺ����������*/
    Site_t site = {0, 0};                           //LCD��ʾͼ�����Ͻ�λ��
    Size_t imgsize  = {CAMERA_W, CAMERA_H};             //LCDͼ���С
    Size_t size={120,80};                                //LCD��ʾ����ͼ���С
    LCD_init();   //��ʾ����ʼ��  
    LCD_str  (site,"Cam init ing",FCOLOUR,BCOLOUR);   //��ʾ����ʾ  �����Ͻ����꣬�ַ�����������ɫ��������ɫ��

    size.H = CAMERA_H;    //��      //����ʾȫ����ʾ��
    size.W = CAMERA_W;    //��
  
    LCD_str  (site,"Cam init OK!",FCOLOUR,BCOLOUR);   //��ʾ����
    LCD_clear(RED);     //���� ��������ɫ��
}