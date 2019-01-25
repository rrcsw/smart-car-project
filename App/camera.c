

#include "common.h"
#include "include.h"
#include "math.h"


//��������
void PORTA_IRQHandler();
void DMA0_IRQHandler();

/*******��������ͷ*******/

uint8 image_threshold;                                  //ͼ���ֵ����ֵ      

int32 time1=0,time2=0;                            //��ʱ��
extern uint8 imgbuff[CAMERA_SIZE];   
extern uint8 mid_line[61];                                //��������
uint8 right_black[60];
uint8 left_black[60];


/*********define for SearchCenterBlackline**********/

int   MiddleLine[RowMax+1];
int   RightEdge[RowMax+1];
int   LeftEdge[RowMax+1];
int   Width[RowMax+1];//={2,3,3,3,4,4,5,5,6,6,
                       //8,8,10,10,12,13,14,14,16,17,
                       //18,18,20,20,22,22,24,24,26,26,
                       //28,30,31,32,32,34,36,36,38,39,
                       //41,41,43,43,45,45,47,47,49,50,
                       //50,51,52,54,55,56,57,58,59,60,61};  // Width[i]  = 20+i*3/4;     //��̬·��

int   MidPri         = 40;
int   LastLine       = 0;
float AvaliableLines = 0;
int   LeftLose       = 0;
int   RightLose      = 0; 
int   AllLose        = 0;
int   LeftLoseStart  = 0;//��¼��߶��ߵĿ�ʼ��
int   RightLoseStart = 0;//��¼�ұ߱߶��ߵĿ�ʼ��
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
    uint8  n;    //���ź�
    uint32 flag;

    while(!PORTA_ISFR);
    flag = PORTA_ISFR;
    PORTA_ISFR  = ~0;                                   //���жϱ�־λ
#if defined (MKL26Z4)              //KL26 KL46
   n = 6;                                             //���ж�
    if(flag & (1 << n))                                 //PTA6�����ж�
    {
        camera_vsync();
    }
#else                              //K60 K66
   n = 29;                                             //���ж�
    if(flag & (1 << n))                                 //PTA29�����ж�
    {
        camera_vsync();
    }
#if ( CAMERA_USE_HREF == 1 )                            //ʹ�����ж�
    n = 28;
    if(flag & (1 << n))                                 //PTA28�����ж�
    {
        camera_href();
    }
#endif

#endif
   


}

/*!
 *  @brief      DMA0�жϷ�����
 *  @since      v5.0
 */
void DMA0_IRQHandler()
{
    camera_dma();
}




void Binarization()
{
        pit_time_start  (PIT1);                                 //��ʼ��ʱ
        camera_get_img();                                      //����ͷ��ȡͼ��
        time1 = pit_time_get_us    (PIT1);                      //����ͷ��ȡͼ��ʱ��
        image_threshold = otsuThreshold(imgbuff);               //��򷨼�����ֵ
       //image_threshold =0x40;                                  //�̶���ֵ
        MT9V032_Binarization(img,imgbuff,image_threshold);      //��ֵ��
        time2 = pit_time_get_us(PIT1);                          //��ȡ��ֵ����ʱʱ��
}
/*********
void move()
{       
    for(int i=59;i>=0;i--)                                     //��ʼ��ȡ����
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
***********/


void SearchCenterBlackline(void)
{

  int16 i         = 0;
  int16 j         = 0;
  uint8 jj        = 0;
  uint8 WhiteNum  = 0; 
  
  LeftLose        = 0;//��������
  RightLose       = 0;
  AllLose         = 0;
  WhiteNum        = 0;

  MiddleLine[RowMax]  = ColumnMax/2;//��60��
  LeftEdge[RowMax]    = 0;
  RightEdge[RowMax]   = ColumnMax;
  Width[RowMax]       = 60;
  
  SetInitVal();
  
  //ǰʮ�У�������������ұ�ɨ��ķ���
  
  for(i=RowMax-1;i>=50;i--)//������ǰʮ�У�ȫ��ɨ��
  {
    if(i ==RowMax-1)//���о���ͼ��������Ϊɨ�����
    {
       j = MidPri;//40  
    } 
    else
    {
        j = MiddleLine[i+1];//���������һ���е��λ����Ϊ����ɨ�����
    }   
    if(j <= 2)
    {
        j = 2;
    } 
    while(j >= 2)//j>=2��Ч��Χ�ڽ�����Ѱ 
    {
        if(img[i][j]==White_Point&& img[i][j-1]==Black_Point&&img[i][j-2]==Black_Point)//���������ҵ��װ׺����� 
        {
             LeftEdge[i] =j;//�ҵ���ֵ �Ҳ�������ԭֵ0      
             break;//��������Ѱ��
        }
             j--;//���������ƶ�
     }
     if(i==RowMax-1) //�����ұ߽�
     {
        j = MidPri;//������У���ͼ�����Ŀ�ʼ��Ѱ
     }          
     else
     {
        j = MiddleLine[i+1];//�������һ������λ�ÿ�ʼ��Ѱ
     }
     if(j >=ColumnMax-2)//j >=ColumnMax-2��Ч��Χ����Ѱ����
     {
        j = ColumnMax-2;
     }
     while(j <= ColumnMax-2)
     {
         
        if(img[i][j]==White_Point && img[i][j+1]==Black_Point && img[i][j+2]==Black_Point)//���������ҵ��װ׺������
        {
               RightEdge[i] = j;//�ҵ���ֵ   �Ҳ�������ԭֵ
               break;//��������Ѱ��
        }
               j++;//���������ƶ�
     }
     if(LeftEdge[i]!=0 && RightEdge[i]!=ColumnMax)//�����жϣ�û�ж���
     {
          MiddleLine[i] = (LeftEdge[i] + RightEdge[i])/2;  
     }
     else if(LeftEdge[i]!=0 && RightEdge[i]==ColumnMax)//��������,û�ж�����
     {
          RightLose++;//��¼ֻ�����߶�������
          
          //������е�

        if((RightEdge[i]-LeftEdge[i]) >=(RightEdge[i+1]-LeftEdge[i+1]+5))//ͻ��
        {
                MiddleLine[i] = MiddleLine[i+1]+2;
        }
        else 
        {
                MiddleLine[i] = LeftEdge[i] + Width[i]/2+5;//�����Ļ����ð��
        }
     }
     else if(LeftEdge[i]==0 && RightEdge[i]!=ColumnMax)//��������
     {
           
        if((RightEdge[i]-LeftEdge[i]) >= (RightEdge[i+1]-LeftEdge[i+1]+1))//ͻ��      
        {
                MiddleLine[i] = MiddleLine[i+1]-1; 
        } 
        else 
        {
                MiddleLine[i] = RightEdge[i] - Width[i]/2-5;//�߿�
        }
     }
     else if(LeftEdge[i]==0 && RightEdge[i]==ColumnMax)//���߶����˵Ļ�  
     {
         AllLose++;
             
        if(i ==RowMax-1)//��������о���ͼ��������Ϊ�е�
        {
                MiddleLine[i] = MidPri;
        }       
        else 
        {
                MiddleLine[i] = MiddleLine[i+1];//����������о�����һ�е�������Ϊ�����е�
        }             
      }
    
    
    
     if(MiddleLine[RowMax-1] >=70)
     {
         MidPri = 70;
     }          
     else if(MiddleLine[RowMax-1] <=10)
     {
         MidPri = 10;
     }         
     else
     {
          MidPri = MiddleLine[RowMax-1];//��¼��֡ͼ���59�е�����ֵ����Ϊ��һ��ͼ���59��ɨ����ʼ��
     }
    
  }   
  for(i=49; i>0; i--)//����ʣ����
  {   
    if(LeftEdge[i+1]!=0 && RightEdge[i+1]!=ColumnMax) //��һ�����߶��ҵ� ���ñ���ɨ��     
    {
         j = ((LeftEdge[i+1]+10) >= ColumnMax-2)? ColumnMax-2:(LeftEdge[i+1]+10);//������߽�    
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
         j = ((RightEdge[i+1]-10) <= 1)? 1:(RightEdge[i+1]-10); //�����ұ߽�   
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
    else if(LeftEdge[i+1]!=0 && RightEdge[i+1]==ColumnMax)//��һ��ֻ�ҵ���߽�   
    {      
         j  = ((LeftEdge[i+1]+10) >=ColumnMax-2)? ColumnMax-2:(LeftEdge[i+1]+10);//��߽��ñ���ɨ��   
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
         j = MiddleLine[i+1];//��һ�ж����ұ߽���ȫ��ɨ�� 
      if(j >= 78)
      {
           j = 78;
      }
      while(j <= ColumnMax-2)      
      {    
          if(img[i][j]==White_Point && img[i][j+1]==Black_Point&&img[i][j+2]==Black_Point)
          {
               RightEdge[i] = j;
               break;
          }
         j++;
      }
           
    }
    else if(LeftEdge[i+1]==0 && RightEdge[i+1]!=ColumnMax) //��һ��ֻ�ҵ��ұ߽�      
    {
         j = ((RightEdge[i+1]-10) <= 1)?1:(RightEdge[i+1]-10);//��Ե׷�����ұ߽� 
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
           j = MiddleLine[i+1]; //ȫ��ɨ������߽�
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
    else if(LeftEdge[i+1]==0 && RightEdge[i+1]==ColumnMax)  //��һ��û�ҵ��߽磬������ʮ�ֻ��߻���  
    {
        
          j = MiddleLine[i+1];   //��ȫ������߽�
        while(j >= 1)  
        {
             if(img[i][j]==White_Point && img[i][j-1]==Black_Point&& img[i][j-2]==Black_Point)     
             {
                  LeftEdge[i] = j;
                  break;
             }
            j--;
        }
         j = MiddleLine[i+1];   //ȫ�����ұ߽�   
       while(j <=ColumnMax-2)       
       {   
            if(img[i][j]==White_Point&&img[i][j+1]==Black_Point)
            {
                  RightEdge[i] = j;  
                  break;
            }
           j++;
              
       }         
    }
    if( (RightEdge[i]-LeftEdge[i]) >= (RightEdge[i+1]-LeftEdge[i+1]+3) )//��������� 
    {      
          MiddleLine[i] = MiddleLine[i+1];//����һ��
    }
    else
    {
            if(LeftEdge[i]!=0 && RightEdge[i]!=ColumnMax)   
            {
                     MiddleLine[i] = (LeftEdge[i] + RightEdge[i])/2+5;
                     //��б��ʮ�ֽ��о���
                     
                 if( MiddleLine[i]-MiddleLine[i+1]>8&&((ABS(LeftEdge[i]-LeftEdge[i+1]>3)||ABS(RightEdge[i]-RightEdge[i+1]>3)) )&& i>=30)//��������ͻ��
                 {
                           uint8 ii = i;
                    
                      while(1)
                      {
                               MiddleLine[ii+1] = MiddleLine[ii]-1; 
                               ii++; 
                               
                           if( ii>=50 || (MiddleLine[ii]-MiddleLine[ii+1]<=1) )
                           {
                                 break;
                           }          
                       }
                 }
                 if( (MiddleLine[i+1]-MiddleLine[i]>8)&&((ABS(LeftEdge[i]-LeftEdge[i+1]>3)||ABS(RightEdge[i]-RightEdge[i+1]>3)))&& i>=30)
                 {
                           uint8 ii = i;
                          
                      while(1)
                      {
                               MiddleLine[ii+1] = MiddleLine[ii]+1;
                               ii++;
                               
                           if( ii>=50 || (MiddleLine[ii+1]-MiddleLine[ii]<=1) )
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
                               MiddleLine[i] = MiddleLine[i+1]+LeftEdge[i]-LeftEdge[i+1]+5;
                 }        
                 else
                 {
                                MiddleLine[i]  = LeftEdge[i] + Width[i]/2+5;
                 }
            }
            
            else if(LeftEdge[i]==0 && RightEdge[i]!=ColumnMax)//find right
            {
                          LeftLose++;
                          
                      
                 if(RightEdge[i+1] !=ColumnMax)
                 {
                  
                                 MiddleLine[i] = MiddleLine[i+1]+RightEdge[i]-RightEdge[i+1]-5;
                 }
                 else
                 {
                 
                                 MiddleLine[i] = RightEdge[i] - Width[i]/2-5;
                 }
            }
           else if(LeftEdge[i]==0 && RightEdge[i]==ColumnMax)//���߶���    
           {      
                         WhiteNum++;
                         AllLose++;
                
                 if(WhiteNum == 1)
                 {   
                                  WhiteStart = i;
                 }         
                        MiddleLine[i] = MiddleLine[i+1];
           }
            
       }
       
    if(i == 0)             
    {            
            AvaliableLines = 60;
            LastLine  = 0;  
            break;
    }
           uint16 m = MiddleLine[i];
    if(m < 5)
    { 
            m = 5;
    }
    if(m > 75)
    {
            m = 75;
    }
    if( (LeftEdge[i]!=0 && LeftEdge[i]>=65) || (RightEdge[i]!=ColumnMax && RightEdge[i]<15) || (i>=1)&&(img[i-1][m]== Black_Point)) //���һ��              
    {
            LastLine = i;//���һ�У���̬ǰհ
            AvaliableLines = 60 - i;//��Ч����
            break;
    }
    
   
  }
}