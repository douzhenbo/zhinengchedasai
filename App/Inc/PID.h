#ifndef __PID_H__
#define __PID_H__

#include "common.h"
#include "include.h"

#define P 0
#define I 1
#define D 2
#define T 3

typedef struct PID
{
	long SumError;	 //����ۼ�	
	int32 LastError; //Error[-1]
	int32 PrevError; //Error[-2]	
	int32 LastSpeed; //Speed[-1]
    float KP;        //�������� Proportional Const
    float KI;        //���ֳ��� Integral Const
    float KD;        //΢�ֳ��� Derivative Const
} PID;

extern float   MOTOR[3];	//���PID
extern char Set;
extern int32 S3010[6][4];
extern PID S3010_PID, MOTOR_PID;	//�������͵����PID�����ṹ��

//λ��ʽPID������ʼ��
void PlacePID_Init(PID *sptr);
//����ʽPID������ʼ��
void IncPID_Init(PID *sptr, float *MOTOR_PID);
//λ��ʽPID�������
int32 PlacePID_Control(PID *sprt, int32 NowPiont, int32 SetPoint);
//����ʽ����PID����
int32 PID_Cascade(PID *sprt, int32 NowPiont, int32 SetPoint);
//����ʽPID����
int32 PID_Realize(PID *sptr, int32 ActualSpeed, int32 SetSpeed);


#endif /* __PID_H__ */