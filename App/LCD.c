#include "common.h"
#include "include.h"
#include "math.h"


extern int32 time1,time2;
extern uint8 img[CAMERA_H][CAMERA_W];
extern uint8 imgbuff[CAMERA_SIZE]; 
extern uint8 right_black[60];
extern uint8 left_black[60];
extern uint8 mid_line[61];

 /*LCD液晶变量定义*/
    Site_t site = {0, 0};                           //LCD显示图像左上角位置
    Size_t imgsize  = {CAMERA_W, CAMERA_H};             //LCD图像大小
    Size_t size={80,60};                                //LCD显示区域图像大小


void LCD_Binarization()
{
        site.x=80;site.y=0;
        LCD_num_BC(site,time1,8,BLUE,RED);                 //us//获取图像时间
        site.x=80;site.y=65;
        LCD_num_BC(site,time2,8,BLUE,RED);                 //us//二值化时间
        
        site.x=0;site.y=0;
        LCD_Img_gray_Z(site, size, imgbuff,imgsize);            //LCD灰度图像显示
        site.x=0;site.y=65;
        LCD_Img_gray_Z(site, size, img[0] ,imgsize);             //LCD黑白图像显示
}

void LCD_Display()
{
    
       {Site_t lcdsite;uint8 i;
         for(i=0;i<60;i++) //纵向扫描打点
         {
           lcdsite.y=i;//控制纵坐标
           lcdsite.x= mid_line[i]; LCD_point(lcdsite,RED); //红色中线
           lcdsite.x= left_black[i]; LCD_point(lcdsite,BLUE); //蓝色左边界
           lcdsite.x=right_black[i]; LCD_point(lcdsite,YELLOW); //黄色右边界
           lcdsite.x=39; LCD_point(lcdsite,GREEN); //绿色中线误差判定标准
         }
         for(i=0;i<80;i++)//横向扫描打点
         {
           lcdsite.x=i;//控制横坐标
           lcdsite.y=30;
           LCD_point(lcdsite,GREEN); //打角识别行
         }
       }
}

void LCD_Init()
{
    /*LCD液晶变量定义*/
    Site_t site = {0, 0};                           //LCD显示图像左上角位置
    Size_t imgsize  = {CAMERA_W, CAMERA_H};             //LCD图像大小
    Size_t size={80,60};                                //LCD显示区域图像大小
    LCD_init();   //显示屏初始化  
    LCD_str  (site,"Cam init ing",FCOLOUR,BCOLOUR);   //显示屏显示  （左上角坐标，字符串，字体颜色，背景颜色）

    size.H = CAMERA_H;    //高      //（表示全屏显示）
    size.W = CAMERA_W;    //宽
  
    LCD_str  (site,"Cam init OK!",FCOLOUR,BCOLOUR);   //显示屏显
    LCD_clear(RED);     //清屏 （背景红色）
}