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







 /*LCD液晶变量定义*/
    Site_t site = {0, 0};                           //LCD显示图像左上角位置
    Size_t imgsize  = {CAMERA_W, CAMERA_H};             //LCD图像大小
    Size_t size={120,90};                                //LCD显示区域图像大小


void LCD_Binarization()
{
        //site.x=80;site.y=0;
        //LCD_num_BC(site,time1,8,BLUE,RED);                 //us//获取图像时间
       // site.x=80;site.y=65;
        //LCD_num_BC(site,time2,8,BLUE,RED);                 //us//二值化时间
        
       // site.x=0;site.y=0;
      // LCD_Img_gray_Z(site, size, imgbuff,imgsize);            //LCD灰度图像显示
        site.x=0;site.y=0;
        LCD_Img_gray_Z(site, size, img[0] ,imgsize);             //LCD黑白图像显示
        /*
        site.x=0;site.y=100;
        LCD_num_BC(site,Error,8,BLUE,RED);           //偏差
        
        site.x=20;site.y=100;
        LCD_num_BC(site,image_threshold,8,BLUE,RED);        //阈值         //舵机输出  中值690
        */
        //site.x=50;site.y=100;
        //LCD_num_BC(site,RightLose,8,BLUE,RED);         //右边界丢失数量
        
        //site.x=70;site.y=100;
        //LCD_num_BC(site,AllLose,8,BLUE,RED);            //两边全丢数量
        /*
        site.x=50;site.y=100;
        LCD_num_BC(site,CenterMeanValue,8,BLUE,RED);            //中线加权值
        
        site.x=70;site.y=100;
        LCD_num_BC(site,SteerPwmAdd,8,BLUE,RED);  //Pwm改变量
        
       site.x=100;site.y=100;
        LCD_num_BC(site,KP,8,BLUE,RED);//kp
        */
        site.x=110;site.y=100;
        LCD_num_BC(site,controlplan,8,BLUE,RED);//kd
        
        
        
        
}

void LCD_Display()
{
    
       {Site_t lcdsite;uint8 i;
         for(i=0;i<90;i++) //纵向扫描打点
         {
           lcdsite.y=i;//控制纵坐标
           lcdsite.x= mid_line[i]; LCD_point(lcdsite,RED); //红色中线
           lcdsite.x= LeftEdge[i]; LCD_point(lcdsite,BLUE); //蓝色左边界
           lcdsite.x= RightEdge[i]; LCD_point(lcdsite,YELLOW); //黄色右边界
           lcdsite.x=59; LCD_point(lcdsite,GREEN); //绿色中线误差判定标准
         }
         for(i=0;i<59;i++)//横向扫描打点
         {
           lcdsite.x=i;//控制横坐标
           lcdsite.y=44;
           LCD_point(lcdsite,GREEN); //打角识别行
         }
       }
}

void LCD_Init()
{
    /*LCD液晶变量定义*/
    Site_t site = {0, 0};                           //LCD显示图像左上角位置
    Size_t imgsize  = {CAMERA_W, CAMERA_H};             //LCD图像大小
    Size_t size={120,80};                                //LCD显示区域图像大小
    LCD_init();   //显示屏初始化  
    LCD_str  (site,"Cam init ing",FCOLOUR,BCOLOUR);   //显示屏显示  （左上角坐标，字符串，字体颜色，背景颜色）

    size.H = CAMERA_H;    //高      //（表示全屏显示）
    size.W = CAMERA_W;    //宽
  
    LCD_str  (site,"Cam init OK!",FCOLOUR,BCOLOUR);   //显示屏显
    LCD_clear(RED);     //清屏 （背景红色）
}