

#include "common.h"
#include "include.h"
#include "math.h"


//函数声明
void PORTA_IRQHandler();
void DMA0_IRQHandler();

/*******定义摄像头*******/

uint8 image_threshold;                                  //图像二值化阈值      

int32 time1=0,time2=0;                            //计时器
extern uint8 imgbuff[CAMERA_SIZE];   
extern  uint8 mid_line[RowMax+1];                               //中线数组
//uint8 right_black[60];
//uint8 left_black[60];

int16 i         = 0;
  int16 j         = 0;
  uint8 jj        = 0;
  uint8 WhiteNum  = 0; 
/*********define for SearchCenterBlackline**********/

 
int   RightEdge[RowMax+1];
int   LeftEdge[RowMax+1];
int   Width[RowMax+1];//={2,3,3,3,4,4,5,5,6,6,
                       //8,8,10,10,12,13,14,14,16,17,
                       //18,18,20,20,22,22,24,24,26,26,
                       //28,30,31,32,32,34,36,36,38,39,
                       //41,41,43,43,45,45,47,47,49,50,
                       //50,51,52,54,55,56,57,58,59,60,61};  // Width[i]  = 20+i*3/4;     //动态路宽

int   MidPri         = 60;
int   LastLine       = 0;
float AvaliableLines = 0;
int   LeftLose       = 0;
int   RightLose      = 0; 
int   AllLose        = 0;
int   LeftLoseStart  = 0;//记录左边丢线的开始行
int   RightLoseStart = 0;//记录右边边丢线的开始行
int   WhiteStart     = 0;


/*********define for GetBlackEndParam**********/
int BlackEndMR      = 0;
int BlackEndML      = 0;
int BlackEndLL      = 0;
int BlackEndRR      = 0;
int BlackEndL       = 0;
int BlackEndM       = 0;
int BlackEndR       = 0;
int BlackEndMaxMax  = 0;
int BlackEndMax     = 0;
int DropRow         = 0;






void PORTA_IRQHandler()
{
    uint8  n;    //引脚号
    uint32 flag;

    while(!PORTA_ISFR);
    flag = PORTA_ISFR;
    PORTA_ISFR  = ~0;                                   //清中断标志位
#if defined (MKL26Z4)              //KL26 KL46
   n = 6;                                             //场中断
    if(flag & (1 << n))                                 //PTA6触发中断
    {
        camera_vsync();
    }
#else                              //K60 K66
   n = 29;                                             //场中断
    if(flag & (1 << n))                                 //PTA29触发中断
    {
        camera_vsync();
    }
#if ( CAMERA_USE_HREF == 1 )                            //使用行中断
    n = 28;
    if(flag & (1 << n))                                 //PTA28触发中断
    {
        camera_href();
    }
#endif

#endif
   


}

/*!
 *  @brief      DMA0中断服务函数
 *  @since      v5.0
 */
void DMA0_IRQHandler()
{
    camera_dma();
}




void Binarization()
{
        pit_time_start  (PIT1);                                 //开始计时
        camera_get_img();//摄像头获取图像
        time1 = pit_time_get_us    (PIT1); //摄像头获取图像时间
        image_threshold = otsuThreshold(imgbuff);  //大津法计算阈值
        //image_threshold =0x40;                                  //固定阈值
        MT9V032_Binarization(img,imgbuff,image_threshold);      //二值化
        time2 = pit_time_get_us(PIT1);                          //获取二值化计时时间
}
/***
void move()
{     
    for(int i=59;i>=0;i--)                                     //开始获取中线
        {
        right_black[i]=79;left_black[i]=0;
        for(int j=39;i<79;j++)
        {
        if(img[i][j]==WHITE&&img[i][j+1]==BLACK)
        {right_black[i]=j+1;break;}
        }
        for(int j=39;i> 0;j--)
        {
        if(img[i][j]==WHITE&&img[i][j-1]==BLACK)
        {right_black[i]=j-1;break;}
        }
        mid_line[i]=(right_black[i]+left_black[i])/2;
        }
        
}
********/
void SetInitVal()
{
  int i;
  
  for(i=0;i<RowMax;i++)//赋初值             
  { 
      RightEdge[i]  = ColumnMax;
      LeftEdge[i]   = 0;
      mid_line[i] = ColumnMax/2;
      Width[i]  = 30+i*4/5;//动态路宽30+i*1/2
     
      
  }
  
} 


void SearchCenterline()
{

  int16 i         = 0;
  int16 j         = 0;
  uint8 jj        = 0;
  uint8 WhiteNum  = 0; 
  
  LeftLose        = 0;//变量清零
  RightLose       = 0;
  AllLose         = 0;
  WhiteNum        = 0;

  mid_line[RowMax]  = ColumnMax/2;//第60行
  LeftEdge[RowMax]    = 0;
  RightEdge[RowMax]   = ColumnMax;
  Width[RowMax]       = 90;
  
  SetInitVal();
  
  
  
    //二十行，采用右左边往右边扫描的方法    
  NormalSearchingMidLine();
  SearchMidLineNext();  
     
   
  }


//

void GetEndParam()//获取黑线截止行
{
  
  unsigned char LEndFlag  = 0;//标志位
  unsigned char MEndFlag  = 0;
  unsigned char REndFlag  = 0;	
  unsigned char MREndFlag = 0;
  unsigned char MLEndFlag = 0;
  unsigned char LLEndFlag = 0;
  unsigned char RREndFlag = 0;

  int i=0;

  BlackEndMR   = 0;//清零 75
  BlackEndML   = 0;//45
  BlackEndLL   = 0;//15
  BlackEndRR   = 0;//105
  BlackEndL    = 0;//30
  BlackEndM    = 0;//60
  BlackEndR    = 0;//90
  
  for (i = RowMax-1; i >= 3 ; i--)
  {
	if(img[i][ColumnMax/2] == White_Point && !MEndFlag )//!MEndFlag=1 //60
        {
		BlackEndM++;//中黑线截至行
        }
	else if(i > 1 && img[i-1][ColumnMax/2] == Black_Point && img[i-2][ColumnMax/2] == Black_Point)//连续两行是黑色        
        {
		MEndFlag = 1;
        }
	if(img[i][ColumnMax/4] == White_Point && !LEndFlag )//30
        {
		BlackEndL++;//左黑线截至行
        }
	else if(i > 1 && img[i-1][ColumnMax/4] == Black_Point && img[i-2][ColumnMax/4] == Black_Point)
        {
		LEndFlag = 1;
        }
	if(img[i][ColumnMax*3/4] == White_Point && !REndFlag )//90
        {
		BlackEndR++;//右黑线截至行
	}
	else if(i > 1 && img[i-1][ColumnMax*3/4] == Black_Point && img[i-2][ColumnMax*3/4] == Black_Point)
        {
		REndFlag = 1;
        }
        if(img[i][45] == White_Point && !MLEndFlag )
        {
		BlackEndML++;
        }
	else if(i > 1 && img[i-1][45] == Black_Point && img[i-2][45] == Black_Point)
        {
		MLEndFlag = 1;
        }
	if(img[i][75] == White_Point && !MREndFlag )
        {
		BlackEndMR++;
        }
	else if(i > 1 && img[i-1][75] == Black_Point && img[i-2][75] == Black_Point)
        {
		MREndFlag = 1;
	}
        if(img[i][15] == White_Point && !LLEndFlag )
        {
		BlackEndLL++;
	}
	else if(i > 1 && img[i-1][15] == Black_Point && img[i-2][15] == Black_Point)
        {
		LLEndFlag = 1;
	}
        if(img[i][105] == White_Point && !RREndFlag )
        {
		BlackEndRR++;
	}
	else if(i > 1 && img[i-1][105] == Black_Point && img[i-2][105] == Black_Point)
        {
		RREndFlag = 1;
	}
   
        
    
        BlackEndMax =MAX(BlackEndL,BlackEndM);//取大值
        BlackEndMax =MAX(BlackEndMax,BlackEndR);
        BlackEndMaxMax =MAX(BlackEndMax,BlackEndMR);
        BlackEndMaxMax =MAX(BlackEndMax,BlackEndML);
        BlackEndMaxMax =MAX(BlackEndMax,BlackEndLL);
        BlackEndMaxMax =MAX(BlackEndMax,BlackEndRR);
        if(BlackEndMaxMax>=90)
        {
            BlackEndMaxMax=88;
        }
        DropRow=90-BlackEndMaxMax;//封顶的行数      
 }

}

void NormalSearchingMidLine()
{
  for(i=RowMax-1;i>=0;i--)//首先找前二十行，全行扫描
  {
    if(i ==RowMax-1)//首行就以图像中心作为扫描起点
    {
       j = MidPri;//60 
    } 
    else
    {
        j = mid_line[i+1];//否则就以上一行中点的位置作为本行扫描起点
    }   
    if(j <= 2)
    {
        j = 2;
    } 
    while(j >= 2)//j>=2有效范围内进行搜寻 
    {
        if(img[i][j]==White_Point && img[i][j-1]==Black_Point&&img[i][j-2]==Black_Point)//从右向左找到白白黑跳变 
        {
             LeftEdge[i] =j;//找到则赋值 找不到保持原值0      
             break;//跳出本行寻线
        }
             j--;//列数往左移动
     }
     if(i==RowMax-1) //再找右边界
     {
        j = MidPri;//如果首行，从图像中心开始搜寻
     }          
     else
     {
        j = mid_line[i+1];//否则从上一行中心位置开始搜寻
     }
     if(j >=ColumnMax-2)//j >=ColumnMax-2有效范围内搜寻右线
     {
        j = ColumnMax-2;
     }
     while(j <= ColumnMax-2)
     {
         
        if(img[i][j]==White_Point && img[i][j+1]==Black_Point && img[i][j+2]==Black_Point)//从左向右找到白白黑跳变点
        {
               RightEdge[i] = j;//找到则赋值   找不到保持原值
               break;//跳出本行寻线
        }
               j++;//列数往右移动
     }
     if(LeftEdge[i]!=0 && RightEdge[i]!=ColumnMax)//中线判断，没有丢线
     {
          mid_line[i] = (LeftEdge[i] + RightEdge[i])/2;  
     }
     else if(LeftEdge[i]!=0 && RightEdge[i]==ColumnMax)//丢了右线,没有丢左线
     {
          RightLose++;//记录只有右线丢的数量
          
          //如果当行的

        if((RightEdge[i]-LeftEdge[i]) >=(RightEdge[i+1]-LeftEdge[i+1]+8))//突变
        {
                mid_line[i] = mid_line[i+1]+4;
        }
        else 
        {
                mid_line[i] = LeftEdge[i] + Width[i]/2+7;//正常的话就用半宽补                                need fix
        }
     }
     else if(LeftEdge[i]==0 && RightEdge[i]!=ColumnMax)//丢了左线
     {
           LeftLose++;
        if((RightEdge[i]-LeftEdge[i]) >= (RightEdge[i+1]-LeftEdge[i+1]-8))//突变      
        {
                mid_line[i] = mid_line[i+1]-4; 
        } 
        else 
        {
                mid_line[i] = RightEdge[i] - Width[i]/2-7;//线宽                                         need fix
        }
     }
     else if(LeftEdge[i]==0 && RightEdge[i]==ColumnMax)//两边都丢了的话  
     {
         AllLose++;
             
        if(i ==RowMax-1)//如果是首行就以图像中心作为中点
        {
                mid_line[i] = MidPri;
        }       
        else 
        {
                mid_line[i] = mid_line[i+1];//如果不是首行就用上一行的中线作为本行中点
        }             
      }
    
    
    
     if(mid_line[RowMax-1] >=ColumnMax-10)
     {
         MidPri = ColumnMax-10;
     }          
     else if(mid_line[RowMax-1] <=10)
     {
         MidPri = 10;
     }         
     else
     {
          MidPri = mid_line[RowMax-1];//记录本帧图像第119行的中线值，作为下一幅图像的59行扫描起始点
     }
    
  }   
}


void SearchMidLineNext()
{
  
  for(i=RowMax-21; i>0; i--)//查找剩余行
  {   
    if(LeftEdge[i+1]!=0 && RightEdge[i+1]!=ColumnMax) //上一行两边都找到 启用边沿扫描     
    {
         j = ((LeftEdge[i+1]+10) >= ColumnMax-2)? ColumnMax-2:(LeftEdge[i+1]+10);//先找左边界    
         jj = ((LeftEdge[i+1]-5) <= 1)? 1:(LeftEdge[i+1]-5);        
      while(j >= jj)       
      {       
          if(img[i][j]==White_Point && img[i][j-1]==Black_Point&& img[i][j-2]==Black_Point)  
          {
               LeftEdge[i] = j;
               break;
          }   
         j--;   
      }
         j = ((RightEdge[i+1]-10) <= 1)? 1:(RightEdge[i+1]-10); //在找右边界   
         jj = ((RightEdge[i+1]+5) >= ColumnMax-2)? ColumnMax-2:(RightEdge[i+1]+5);    
      while(j <= jj)             
      {
          if(img[i][j]==White_Point&& img[i][j+1]==Black_Point&& img[i][j+2]==Black_Point) 
          {
               RightEdge[i] = j;
               break;    
          }
         j++;
      }
    }
    else if(LeftEdge[i+1]!=0 && RightEdge[i+1]==ColumnMax)//上一行只找到左边界   
    {      
         j  = ((LeftEdge[i+1]+10) >=ColumnMax-2)? ColumnMax-2:(LeftEdge[i+1]+10);//左边界用边沿扫描   
         jj = ((LeftEdge[i+1]-5) <= 1)? 1:(LeftEdge[i+1]-5);              
      while(j >= jj)   
      {     
          if(img[i][j]==White_Point && img[i][j-1]==Black_Point && img[i][j-2]==Black_Point)
          {
               LeftEdge[i] = j;
               break;
          }
         j--;   
      }     
         j = mid_line[i+1];//上一行丢了右边界用全行扫描 
      if(j >= ColumnMax-3)
      {
           j = ColumnMax-3;
      }
      while(j <= ColumnMax-3)      
      {    
          if(img[i][j]==White_Point && img[i][j+1]==Black_Point&&img[i][j+2]==Black_Point)
          {
               RightEdge[i] = j;
               break;
          }
         j++;
      }
           
    }
    else if(LeftEdge[i+1]==0 && RightEdge[i+1]!=ColumnMax) //上一行只找到右边界      
    {
         j = ((RightEdge[i+1]-10) <= 1)?1:(RightEdge[i+1]-10);//边缘追踪找右边界 
         jj = ((RightEdge[i+1]+5) >= ColumnMax-2)? ColumnMax-2:(RightEdge[i+1]+5);            
       while(j <= jj)  
       {    
            if(img[i][j]==White_Point&&img[i][j+1]==Black_Point&&img[i][j+2]==Black_Point)
            {
                 RightEdge[i] = j;
                 break;
            }
           j++;     
       }   
           j = mid_line[i+1]; //全行扫描找左边界
        if(j < 2)
        {
             j = 2;
        }  
       while(j >= 1)  
       {  
            if(img[i][j]==White_Point && img[i][j-1]==Black_Point&& img[i][j-2]==Black_Point)       
            {
                 LeftEdge[i] = j;
                 break;
            }
           j--;      
       }           
     } 
    else if(LeftEdge[i+1]==0 && RightEdge[i+1]==ColumnMax)  //上一行没找到边界，可能是十字或者环形  
    {
        
          j = mid_line[i+1];   //找全行找左边界
        while(j >= 1)  
        {
             if(img[i][j]==White_Point && img[i][j-1]==Black_Point&& img[i][j-2]==Black_Point)     
             {
                  LeftEdge[i] = j;
                  break;
             }
            j--;
        }
         j = mid_line[i+1];   //全行找右边界   
       while(j <=ColumnMax-3)       
       {   
            if(img[i][j]==White_Point&&img[i][j+1]==Black_Point&& img[i][j+2]==Black_Point)
            {
                  RightEdge[i] = j;  
                  break;
            }
           j++;
              
       }         
    }
    if( (RightEdge[i]-LeftEdge[i]) >= (RightEdge[i+1]-LeftEdge[i+1]+3) )//不满足畸变 
    {      
          mid_line[i] = mid_line[i+1];//用上一行
    }
    else
    {
            if(LeftEdge[i]!=0 && RightEdge[i]!=ColumnMax)   
            {
                     mid_line[i] = (LeftEdge[i] + RightEdge[i])/2+5;
                     //对斜出十字进行纠正
                     
                 if( mid_line[i]-mid_line[i+1]>8&&((ABS(LeftEdge[i]-LeftEdge[i+1]>3)||ABS(RightEdge[i]-RightEdge[i+1]>3)) )&& i>=30)//中线向右突变
                 {
                           uint8 ii = i;
                    
                      while(1)
                      {
                               mid_line[ii+1] = mid_line[ii]-1; 
                               ii++; 
                               
                           if( ii>=80 || (mid_line[ii]-mid_line[ii+1]<=1) )
                           {
                                 break;
                           }          
                       }
                 }
                 if( (mid_line[i+1]-mid_line[i]>8)&&((ABS(LeftEdge[i]-LeftEdge[i+1]>3)||ABS(RightEdge[i]-RightEdge[i+1]>3)))&& i>=30)
                 {
                           uint8 ii = i;
                          
                      while(1)
                      {
                               mid_line[ii+1] = mid_line[ii]+1;
                               ii++;
                               
                           if( ii>=80 || (mid_line[ii+1]-mid_line[ii]<=1) )
                           { 
                                 break;
                           }       
                       }
                  }
            }
            else if(LeftEdge[i]!=0 && RightEdge[i]==ColumnMax)//find left
            { 
                         RightLose++;
                         
                     
                 if(LeftEdge[i+1] != 0)
                 {
                               mid_line[i] = mid_line[i+1]+LeftEdge[i]-LeftEdge[i+1]+5;
                 }        
                 else
                 {
                                mid_line[i]  = LeftEdge[i] + Width[i]/2+5;
                 }
            }
            
            else if(LeftEdge[i]==0 && RightEdge[i]!=ColumnMax)//find right
            {
                          LeftLose++;
                          
                      
                 if(RightEdge[i+1] !=ColumnMax)
                 {
                  
                                 mid_line[i] = mid_line[i+1]+RightEdge[i]-RightEdge[i+1]-5;
                 }
                 else
                 {
                 
                                 mid_line[i] = RightEdge[i] - Width[i]/2-5;
                 }
            }
           else if(LeftEdge[i]==0 && RightEdge[i]==ColumnMax)//两边丢线    
           {      
                         WhiteNum++;
                         AllLose++;
                
                 if(WhiteNum == 1)
                 {   
                                  WhiteStart = i;
                 }         
                        mid_line[i] = mid_line[i+1];
           }
            
       }
       
    if(i == 0)             
    {            
            AvaliableLines = 90;
            LastLine  = 0;  
            break;
    }
           uint16 m = mid_line[i];
    if(m < 5)
    { 
            m = 5;
    }
    if(m > 115)
    {
            m = 115;
    }
    if( (LeftEdge[i]!=0 && LeftEdge[i]>=105) || (RightEdge[i]!=ColumnMax && RightEdge[i]<15) || (i>=1)&&(img[i-1][m]== Black_Point)) //最后一行              
    {
            LastLine = i;//最后一行，动态前瞻
            AvaliableLines = 90 - i;//有效行数
            break;
    }
    
   
  }
}