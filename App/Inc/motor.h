#ifndef __MOTOR_H__
#define __MOTOR_H__

#define  MotorControlPeriod   8     //电机控制值更新周期
#define  SteetControlPeriod   20    //舵机控制周期

#define  OpenLoop 1 //宏定义置1，速度开环控制
#define  CloseLoop 0 //宏定义置1，速度闭环控制

//

extern  int32 GetRightMotorPules;
extern  int32 GetLeftMotorPules;
extern  float OutData[4];
extern  float AvaliableLines;
extern  int   BlackEndM ; 
extern  int   MiddleLine[RowMax+1];//中线
extern  int   BlackEndMR;
extern  int   BlackEndML;
extern float  SteerPwmAdd;
extern int ChangeFlag;
extern int SpeedDropRow;
extern  int32 SpeedSet;
extern int BlackEndMaxMax;
extern int plan;
extern void MotorInit(void);
void CalculateMotorSpeedError(float LeftMotorTarget,float RightMotorTarget);

void MotorControlOpenLoop(void);
extern void MotorControl(void);

void GetTargetSpeed(void);


#endif