#include "PID.h"


PID S3010_PID, MOTOR_PID;	    //�������͵����PID�����ṹ��


//P���32����  D��� 
                                   
int32 S3010[6][4] = {{ 257, 0, 360, 80},{365, 0, 411, 80},{ 360, 0, 350, 60 }, 
                     { 25, 0, 80, 20 },{ 23, 0, 60, 20 },{ 24, 0, 70, 20 }, };


 
float   MOTOR[3]  = {3.40, 0.672, 0.10};	//float   Left_MOTOR[3]  = {3.30, 0.65, 0.00};	


//λ��ʽPID������ʼ��
void PlacePID_Init(PID *sptr)
{
	sptr->SumError = 0;
	sptr->LastError = 0;	//Error[-1]
	sptr->PrevError = 0;	//Error[-2]
}

//����ʽPID������ʼ��
void IncPID_Init(PID *sptr, float *MOTOR_PID)
{
	sptr->SumError = 0;
	sptr->LastError = 0;	//Error[-1]
	sptr->PrevError = 0;	//Error[-2]	
	sptr->LastSpeed = 0;
    sptr->KP = *MOTOR_PID;
    sptr->KI = *(MOTOR_PID+1);
    sptr->KD = *(MOTOR_PID+2);
        
}

//λ��ʽPID�������
int32 PlacePID_Control(PID *sprt, int32 NowPiont, int32 SetPoint)
{
	//����Ϊ�Ĵ���������ֻ���������ͺ��ַ��ͱ�������������ٶ�
	int32 iError,	//��ǰ���
		  Actual;	//���ó���ʵ�����ֵ
	float	Kp;		//��̬P

	
	iError = SetPoint - NowPiont;	//���㵱ǰ���
	Kp = 1.0 * (iError*iError) / S3010[Set][T] + S3010[Set][P];	//Pֵ���ֵ�ɶ��κ�����ϵ  
    
	//Actual =  Kp * iError + S3010[Set][D] * (iError - sprt->LastError) ;//ֻ��PD
	Actual = (int)( Kp * iError + S3010[Set][D] * (iError - sprt->LastError) );//ֻ��PD
	
	sprt->LastError = iError;		//�����ϴ����
	
	return S3010_MID-Actual;
}

//����ʽ����PID����
int32 PID_Cascade(PID *sptr, int32 ActualSpeed, int32 SetSpeed)
{
	register int32 iError,	    //��ǰ���
					Increase;	//���ó���ʵ������
	iError = SetSpeed - ActualSpeed;//���㵱ǰ���
	
    //Increase = -sptr->KP * (ActualSpeed - sptr->LastSpeed)	//���ٶ�
	//		   + sptr->KI * iError
	//		   + sptr->KD * (iError - 2 * sptr->LastError + sptr->PrevError) ;
	Increase =(int)(-sptr->KP * (ActualSpeed - sptr->LastSpeed)	//���ٶ�
			   + sptr->KI * iError
			   + sptr->KD * (iError - 2 * sptr->LastError + sptr->PrevError));
	
	sptr->PrevError = sptr->LastError;	//����ǰ�����
	sptr->LastError = iError;		  	//�����ϴ����
	sptr->LastSpeed = ActualSpeed;		//�����ϴ��ٶ�
	
//	if (iError < -100)
//	{
//		Increase -= 400;
//	}
//	else if (iError < -80)
//	{
//		Increase -= 300;
//	}
//	else if (iError < -50)
//	{
//		Increase -= 250;
//	}
//	else if (iError < -30)
//	{
//		Increase -= 200;
//	}	
	return Increase;
        
}	

//����ʽPID�������    
int32 PID_Realize(PID *sptr, int32 ActualSpeed, int32 SetSpeed)
{
	//��ǰ������Ϊ�Ĵ���������ֻ���������ͺ��ַ��ͱ�������������ٶ�
//	register int32 iError,	    //��ǰ���
//					Increase;	//���ó���ʵ������
	
  	int32 iError,	    //��ǰ���
					Increase;	//���ó���ʵ������
  
	iError = SetSpeed - ActualSpeed;//���㵱ǰ���
    
	//Increase =sptr->KP * (iError - sptr->LastError)
	//		 + sptr->KI * iError
	//		 + sptr->KD * (iError - 2 * sptr->LastError + sptr->PrevError);
	Increase =(int)(sptr->KP * (iError - sptr->LastError)
			 + sptr->KI * iError
			 + sptr->KD * (iError - 2 * sptr->LastError + sptr->PrevError));
	sptr->PrevError = sptr->LastError;	//����ǰ�����
	sptr->LastError = iError;		  	//�����ϴ����
	
	if (iError < -50)
	{
		BangBang_Flag = 1;
	}
	else if (iError < -50 && BangBang_Flag)
	{
		BangBang_Flag = 0;
		Increase -= 300;
	}
//    if (iError > 100)
//	{
//		Increase +=  800;
//	}
//    else if (iError > 50)
//    {
//        Increase +=  400;
//    }
////	else if (iError < -40)
////	{
////		BangBang_Flag = 1;
////		Increase -= 200;
////	}
//
//        
//        if (iError > 100)
//	{
//		Increase +=  600;
//	}
////	else if ((iError > 40) && (BangBang_Flag == 1))
////	{
////		BangBang_Flag = 0;
////		Increase +=  400;
////	}
//	else if (iError > 50)
//	{
//		Increase += 300;
//                BangBang_Flag = 1;
//	}
	
	return Increase;
}



