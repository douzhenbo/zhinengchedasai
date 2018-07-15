#ifndef __HANDLE_H__
#define __HANDLE_H__
#include "common.h"
#include "include.h"

//外部变量声明
#define uchar unsigned char
extern uchar Foresight;
extern uchar Out_Side;
extern uchar Line_Count;
extern uchar Starting_Line_Flag;
extern uchar Left_Max, Right_Min;
extern uchar Left_Line[62];
extern uchar Right_Line[62];
extern uchar Mid_Line[62];
extern uchar Left_Add_Line[62];
extern uchar Right_Add_Line[62];
extern int16 Speed;
extern uint8 Annulus_Check;
extern uint8 Stop_Flag;
extern uint8 Obstacle_Check;
extern uint8 Point_Mid;
extern uchar Annulus2_Delay;
extern uchar Annulus1_Flag;
extern uchar Annulus2_Flag;

//函数声明
//void Annulus_Control(void);
void Annulus1_Control(void);
void Annulus2_Control(void);
void Annulus1_Handle(uchar *data);
void Annulus2_Handle(uchar *data);
void Image_Para_Init(void);
void Image_Handle(uchar *data);
uchar Point_Average(void);
int32 Least_Squares(void);
float Slope_Weight(uchar *Mid);
char Calculate_Angle(uchar Point_1, uchar Point_2, uchar Point_3);
void Curve_Fitting(float *Ka, float *Kb, uchar Start, uchar *Line);
void Curve2_Fitting(float *Ka, float *Kb, uchar Start,uchar End, uchar *Line, uchar Mode);
uchar Calculate_Add(uchar i, float Ka, float Kb);
uchar Limit_Scan(uchar i, uchar *data, uchar Point);
uchar First_Line_Handle(uchar *data);
uchar Traversal_Left(uchar i, uchar *data, uchar *Mid, uchar Left_Min, uchar Right_Max);
uchar Traversal_Right(uchar i, uchar *data, uchar *Mid, uchar Left_Min, uchar Right_Max);
uchar Traversal_Left_Line(uchar i, uchar *data, uchar *Left_Line, uchar *Right_Line);
uchar Traversal_Right_Line(uchar i, uchar *data, uchar *Left_Line, uchar *Right_Line);
void Traversal_Mid_Line(uchar i, uchar *data, uchar Mid, uchar Left_Min, uchar Right_Max, uchar *Left_Line, uchar *Right_Line, uchar *Left_Add_Line, uchar *Right_Add_Line);
void Line_Repair(uchar Start, uchar Stop, uchar *data, uchar *Line, uchar *Line_Add, uchar *Add_Flag);
void Mid_Line_Repair(uchar count, uchar *data);
uchar Point_Weight(void);
char Error_Transform(uchar Data, uchar Set);
int32 abs_int(int32 i);
uint8  MiddleFilter(uint8 a,uint8 b,uint8 c);
//extern void img_extract(uint8 *dst, uint8 *src, uint32 srclen);
void img_recontract(uint8 *dst, uint8 *src, uint32 srclen);
void img_getline(uint8 *dst, uint8 *src, uint32 srclen);
void image_processing(uchar *data);
uint16 island(uchar *data);




#endif
