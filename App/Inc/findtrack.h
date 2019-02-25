#ifndef __FINDTRACK_H__
#define __FINDTRACK_H__

/*** �궨�� ****/ 

#define Black_Point 0
#define White_Point 255
#define RowMax	    60	  //����
#define ColumnMax	80	  //����


/*** �ⲿ������������ ****/  


extern uint8  img[CAMERA_H][CAMERA_W];//����洢����ͼ�������,CAMERA_H(59-0),CAMERA_W(0-79)

extern uint8 imgbuff[CAMERA_SIZE];
extern int   AllLose;
extern unsigned char StartingLineFlag;
extern int BlackEndL;//20
extern int BlackEndM;//40
extern int BlackEndR;//60
extern int BlackEndMR;//50
extern int BlackEndML;//30
extern int BlackEndLL;
extern int BlackEndRR;//70
extern int DropRow;

extern  int   RightEdge[RowMax+1];
extern  int   LeftEdge[RowMax+1];
extern  int   MiddleLine[RowMax+1];
extern int   LastLine;
extern int   Width[RowMax+1];
extern int   LeftLose;
extern int   RightLose ;
extern int   AllLose ;

/*** �������� ****/    

extern void SearchCenterBlackline(void);
extern void GetBlackEndParam();
extern void DrawBlackline();
extern void SetInitVal();

#endif