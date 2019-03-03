#include "common.h"
#include "include.h"
#include "math.h"


extern int32 time1,time2;
extern uint8 img[CAMERA_H][CAMERA_W];
extern uint8 imgbuff[CAMERA_SIZE]; 
extern uint8 right_black[60];
extern uint8 left_black[60];
extern uint8 mid_line[61];

 /*LCDҺ����������*/
    Site_t site = {0, 0};                           //LCD��ʾͼ�����Ͻ�λ��
    Size_t imgsize  = {CAMERA_W, CAMERA_H};             //LCDͼ���С
    Size_t size={80,60};                                //LCD��ʾ����ͼ���С


void LCD_Binarization()
{
        site.x=80;site.y=0;
        LCD_num_BC(site,time1,8,BLUE,RED);                 //us//��ȡͼ��ʱ��
        site.x=80;site.y=65;
        LCD_num_BC(site,time2,8,BLUE,RED);                 //us//��ֵ��ʱ��
        
        site.x=0;site.y=0;
        LCD_Img_gray_Z(site, size, imgbuff,imgsize);            //LCD�Ҷ�ͼ����ʾ
        site.x=0;site.y=65;
        LCD_Img_gray_Z(site, size, img[0] ,imgsize);             //LCD�ڰ�ͼ����ʾ
}

void LCD_Display()
{
    
       {Site_t lcdsite;uint8 i;
         for(i=0;i<60;i++) //����ɨ����
         {
           lcdsite.y=i;//����������
           lcdsite.x= mid_line[i]; LCD_point(lcdsite,RED); //��ɫ����
           lcdsite.x= left_black[i]; LCD_point(lcdsite,BLUE); //��ɫ��߽�
           lcdsite.x=right_black[i]; LCD_point(lcdsite,YELLOW); //��ɫ�ұ߽�
           lcdsite.x=39; LCD_point(lcdsite,GREEN); //��ɫ��������ж���׼
         }
         for(i=0;i<80;i++)//����ɨ����
         {
           lcdsite.x=i;//���ƺ�����
           lcdsite.y=30;
           LCD_point(lcdsite,GREEN); //���ʶ����
         }
       }
}

void LCD_Init()
{
    /*LCDҺ����������*/
    Site_t site = {0, 0};                           //LCD��ʾͼ�����Ͻ�λ��
    Size_t imgsize  = {CAMERA_W, CAMERA_H};             //LCDͼ���С
    Size_t size={80,60};                                //LCD��ʾ����ͼ���С
    LCD_init();   //��ʾ����ʼ��  
    LCD_str  (site,"Cam init ing",FCOLOUR,BCOLOUR);   //��ʾ����ʾ  �����Ͻ����꣬�ַ�����������ɫ��������ɫ��

    size.H = CAMERA_H;    //��      //����ʾȫ����ʾ��
    size.W = CAMERA_W;    //��
  
    LCD_str  (site,"Cam init OK!",FCOLOUR,BCOLOUR);   //��ʾ����
    LCD_clear(RED);     //���� ��������ɫ��
}