#include "PID.h"


PID S3010_PID, MOTOR_PID;	    //定义舵机和电机的PID参数结构体


//P最大32不震荡  D最大 
                                   
int32 S3010[6][4] = {{ 257, 0, 360, 80},{365, 0, 411, 80},{ 360, 0, 350, 60 }, 
                     { 25, 0, 80, 20 },{ 23, 0, 60, 20 },{ 24, 0, 70, 20 }, };


 
float   MOTOR[3]  = {3.40, 0.672, 0.10};	//float   Left_MOTOR[3]  = {3.30, 0.65, 0.00};	


//位置式PID参数初始化
void PlacePID_Init(PID *sptr)
{
	sptr->SumError = 0;
	sptr->LastError = 0;	//Error[-1]
	sptr->PrevError = 0;	//Error[-2]
}

//增量式PID参数初始化
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

//位置式PID舵机控制
int32 PlacePID_Control(PID *sprt, int32 NowPiont, int32 SetPoint)
{
	//定义为寄存器变量，只能用于整型和字符型变量，提高运算速度
	int32 iError,	//当前误差
		  Actual;	//最后得出的实际输出值
	float	Kp;		//动态P

	
	iError = SetPoint - NowPiont;	//计算当前误差
	Kp = 1.0 * (iError*iError) / S3010[Set][T] + S3010[Set][P];	//P值与差值成二次函数关系  
    
	//Actual =  Kp * iError + S3010[Set][D] * (iError - sprt->LastError) ;//只用PD
	Actual = (int)( Kp * iError + S3010[Set][D] * (iError - sprt->LastError) );//只用PD
	
	sprt->LastError = iError;		//更新上次误差
	
	return S3010_MID-Actual;
}

//增量式串级PID控制
int32 PID_Cascade(PID *sptr, int32 ActualSpeed, int32 SetSpeed)
{
	register int32 iError,	    //当前误差
					Increase;	//最后得出的实际增量
	iError = SetSpeed - ActualSpeed;//计算当前误差
	
    //Increase = -sptr->KP * (ActualSpeed - sptr->LastSpeed)	//加速度
	//		   + sptr->KI * iError
	//		   + sptr->KD * (iError - 2 * sptr->LastError + sptr->PrevError) ;
	Increase =(int)(-sptr->KP * (ActualSpeed - sptr->LastSpeed)	//加速度
			   + sptr->KI * iError
			   + sptr->KD * (iError - 2 * sptr->LastError + sptr->PrevError));
	
	sptr->PrevError = sptr->LastError;	//更新前次误差
	sptr->LastError = iError;		  	//更新上次误差
	sptr->LastSpeed = ActualSpeed;		//更新上次速度
	
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

//增量式PID电机控制    
int32 PID_Realize(PID *sptr, int32 ActualSpeed, int32 SetSpeed)
{
	//当前误差，定义为寄存器变量，只能用于整型和字符型变量，提高运算速度
//	register int32 iError,	    //当前误差
//					Increase;	//最后得出的实际增量
	
  	int32 iError,	    //当前误差
					Increase;	//最后得出的实际增量
  
	iError = SetSpeed - ActualSpeed;//计算当前误差
    
	//Increase =sptr->KP * (iError - sptr->LastError)
	//		 + sptr->KI * iError
	//		 + sptr->KD * (iError - 2 * sptr->LastError + sptr->PrevError);
	Increase =(int)(sptr->KP * (iError - sptr->LastError)
			 + sptr->KI * iError
			 + sptr->KD * (iError - 2 * sptr->LastError + sptr->PrevError));
	sptr->PrevError = sptr->LastError;	//更新前次误差
	sptr->LastError = iError;		  	//更新上次误差
	
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



