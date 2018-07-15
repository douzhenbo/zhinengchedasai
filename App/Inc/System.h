#ifndef __SYSTEM_H__
#define __SYSTEM_H__
#include "common.h"
#include "include.h"

#define MaxLengh  6
/***********���������Ա�־λ**************************/

#define  BuzzerRing           gpio_set   (PTB23, 1) //��������
#define  BuzzerQuiet          gpio_set   (PTB23, 0) //����������
extern void Buzzer_Init();
extern void BuzzerTest(unsigned char flag);

/*****************************************************/


//extern uint8 *imgbuff;
extern uint8 imgbuff[CAMERA_SIZE]; 
extern uint8 flag_run;
extern int32 S_D5_Duty;

extern uint8 threshold_value, var_speed;
extern uint8 Image_Send_Flag;	// ͼ���Ϳ��ƣ�1Ϊ���ͣ�0Ϊ������
extern uint8 Send_OK,CNST_Value;
extern uint8 img_display;

void System_Init();		 //����ģ���ʼ��
void data_send();		 //���߷��ͺͽ�������

void Boma_Key_Init(void);
void Parameter_Init(void);
uint8 wait_key_down();
void KEY_Adjust(void);

#endif