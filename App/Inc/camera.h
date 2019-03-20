#ifndef __CAMERA_H__
#define __CAMERA_H__
//
/*** 宏定义 ****/ 

#define Black_Point 0
#define White_Point 255
#define RowMax	    90	  //行数
#define ColumnMax	120	  //列数


/*** 外部函数变量申明 ****/  


extern uint8  img[CAMERA_H][CAMERA_W];//定义存储接收图像的数组,CAMERA_H(59-0),CAMERA_W(0-79)

extern uint8 imgbuff[CAMERA_SIZE];
extern int   AllLose;
extern unsigned char StartingLineFlag;
extern int BlackEndL;//20
extern int BlackEndM;//50
extern int BlackEndR;//60
extern int BlackEndMR;//50
extern int BlackEndML;//30
extern int BlackEndLL;
extern int BlackEndRR;//70
extern int DropRow;

extern  int   RightEdge[RowMax+1];
extern  int   LeftEdge[RowMax+1];
extern  uint8   mid_line[RowMax+1];
extern int   LastLine;
extern int   Width[RowMax+1];
extern int   LeftLose;
extern int   RightLose ;
extern int   AllLose;

/*** 函数声明 ****/    

extern void SearchCenterBlackline(void);
extern void GetEndParam();
extern void DrawBlackline();
extern void SetInitVal();
extern void NormalSearchingMidLine();
extern void LoseEdgeClose();
extern void SearchMidLineNext();

#endif