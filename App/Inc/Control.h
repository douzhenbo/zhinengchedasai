#ifndef __CONTROL_H__
#define __CONTROL_H__

#include "common.h"
#include "include.h"

extern int32 Left_MOTOR_Duty;       //电机左占空比
extern int32 Right_MOTOR_Duty;      //电机右占比比
extern int32   MOTOR_Speed ;

extern int32 Left_MOTOR_Speed;
extern int32 Right_MOTOR_Speed;
extern uint8 Run_Flag;
extern uint8 Game_End;
extern uint8 Stop_Flag;
extern uint8 BangBang_Flag;
extern uint8 Point;
extern int16 High_Speed;
extern char Set,Speed_Set;
extern uint16 Ramp_Delay;
extern uint8 Ramp_Flag;
extern int32  Ramp_Speed;
extern int32    Start_Pian_Delaytime;
extern int32 Control[3];
/************ 定义电机PWM端口	************/	
#define MOTOR_TPM   TPM2
#define MOTOR_HZ    500	  

#define MOTOR1_PWM  TPM_CH0     //PTB18
#define MOTOR2_PWM  TPM_CH1     //PTB19


/*************  定义舵机端口 **************/		
#define S3010_TPM TPM1
#define S3010_CH  TPM_CH0
#define S3010_HZ  300
#define S3010_MID   29000   //23000	差4000
#define S3010_LEFT  23500    //4750
#define S3010_RIGHT 34500   //3850  27500
extern uint8   Point_Mid; //目标点横坐标



/*************** 函数声明 ****************/

void Adjust_Speed(void);
void speed_measure(void);		//电机速度测量
void MOTOR_Control(void);	//增量式PID调控电机速度
void Starting_Check(void);
int32 range_protect(int32 duty, int32 min, int32 max); //限幅保护
extern void vcan_sendware(void *wareaddr, uint32_t waresize);

/*********山外虚拟示波器*********/

#define wareon_off  0


#endif /* __CONTROL_H__ */


