#ifndef __CONTROL_H__
#define __CONTROL_H__

#include "common.h"
#include "include.h"

extern int32 Left_MOTOR_Duty;       //�����ռ�ձ�
extern int32 Right_MOTOR_Duty;      //�����ռ�ȱ�
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
/************ ������PWM�˿�	************/	
#define MOTOR_TPM   TPM2
#define MOTOR_HZ    500	  

#define MOTOR1_PWM  TPM_CH0     //PTB18
#define MOTOR2_PWM  TPM_CH1     //PTB19


/*************  �������˿� **************/		
#define S3010_TPM TPM1
#define S3010_CH  TPM_CH0
#define S3010_HZ  300
#define S3010_MID   29000   //23000	��4000
#define S3010_LEFT  23500    //4750
#define S3010_RIGHT 34500   //3850  27500
extern uint8   Point_Mid; //Ŀ��������



/*************** �������� ****************/

void Adjust_Speed(void);
void speed_measure(void);		//����ٶȲ���
void MOTOR_Control(void);	//����ʽPID���ص���ٶ�
void Starting_Check(void);
int32 range_protect(int32 duty, int32 min, int32 max); //�޷�����
extern void vcan_sendware(void *wareaddr, uint32_t waresize);

/*********ɽ������ʾ����*********/

#define wareon_off  0


#endif /* __CONTROL_H__ */


