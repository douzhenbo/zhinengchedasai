#include "Control.h"
#include "math.h"

/****************************    �������Ʊ���    ****************************/ 
char Set = 0,Speed_Set=0;
uint8   BangBang_Flag = 0;        //������ʽPID��
uint8 	Run_Flag  = 0;            //���ܱ�־
extern uchar Foresight;
extern int32 S3010_Duty;

int32 S3010_Duty =29000;   //����ӵģ�ע���Ժ��޸�

/****************************    �µ�������    ****************************/ 
uint8  Ramp_Flag=  0;       //�µ���־λ          
uint16 Ramp_Time=  0;
uint16 Ramp_Delay= 80;      //�µ���ʱʱ��

/****************************        ����        ****************************/ 

/****************************    ����ʶ�����    ****************************/ 
uint8   Going_Flag = 0;
uint8   Stop_Flag = 0;
uint8   Stoping_Flag = 0;
uint8   Annulus_Check=0;

/***************************************************************************/ 


/****************************    �ٶȿ��Ʊ���    ****************************/ 
int16   Starting_Time=300;              //������ʱʱ��2S
int16    Starting1_Time;                 //����ƫ����ʱ
//int32  Start_Pian_Delaytime=100;        //����ƫ����ʱ
int8    Stop_Time=0;                    //ͣ����ʱ
int16   Stop1_Time=0;
uint8   Start_Pian_Flag=0;          

int32   MOTOR_Duty  = 0;
int32   MOTOR_Speed = 0;        
      
float   Left_Speed = 0;         
float   Right_Speed = 0; 

int16 Speed;
int16 Max_Speed=0;
int16 Min_Speed=0;
int16 High_Speed=0;

int16 Mid_Speed,Left_High_Speed,Right_High_Speed;



extern uchar Annulus_Flag;  //Բ����־λ


/**************************    �����߼��  ***********************************/
void Starting_Check(void)
{ 
    if (Starting_Time==0 && !Going_Flag && !Stop_Flag)
    {
        Going_Flag=1;
        Starting1_Time = 370;    //���ܿ�ʼ�رջ�·��� 
    }
    if (Starting1_Time==0 && Going_Flag && !Annulus_Check)
    {
        Annulus_Check=1;
        Start_Pian_Flag=1;
    }
    if (Stop_Flag && Going_Flag)
    {
        if (Stop_Time==0)   
        {
            Stop_Time=20;  //40 //��⵽�������Ժ���ʱͣ��               
        }
        if (Stop_Time==15)
        {
            Going_Flag=0;
            //Starting_Time=-10;
        }
    }  

} 


/******************************** �ٶȿ��� ***********************************/
void Adjust_Speed(void)
{
  High_Speed = 80;
       
        /*****************************  �ٶ�1��  ******************************/
//        if(0)  //gai
//        {
//            if( 0/*loop_times==0*/ )
//            {
//                High_Speed = 130;  //�ȶ�130      
//            }
//            else if( 0)
//            {   High_Speed = 155;          }
//            else
//            {   High_Speed = 130;          }
//        }
//        else if(!Going_Flag)
//        {
//          High_Speed=0;
//        }
//        else
//        {       
//            if( Speed_Set ==1 )
//           {     
//                //  High_Speed = 170;  //����170
//                if (Foresight >= 18)
//                {
//                    High_Speed = 166;
//                }
//                else if (Foresight >= 16)
//                {
//                    High_Speed = 167;
//                }
//                else if (Foresight >= 14)
//                {
//                    High_Speed = 168;
//                }
//                else if (Foresight >= 12)
//                {
//                    High_Speed = 170;
//                }
//                else if (Foresight >= 10)
//                {
//                    High_Speed = 172;
//                }
//                else if (Foresight >= 8)
//                {
//                    High_Speed = 175;
//                }
//                else if (Foresight >= 5)
//                {
//                    High_Speed = 176;
//                }
//                else if(Foresight >= 2)
//                {
//                    High_Speed = 176;
//                }
//                else
//                {
//                    High_Speed = 180;
//                }
//            }
//            else if( Speed_Set ==2 )
//            {     
//                High_Speed = 175;     
////                if (Foresight >= 18)
////                {
////                    High_Speed = 170;
////                }
////                else if (Foresight >= 16)
////                {
////                    High_Speed = 172;
////                }
////                else if (Foresight >= 14)
////                {
////                    High_Speed = 174;
////                }
////                else if (Foresight >= 12)
////                {
////                    High_Speed = 176;
////                }
////                else if (Foresight >= 10)
////                {
////                    High_Speed = 178;
////                }
////                else if (Foresight >= 8)
////                {
////                    High_Speed = 180;
////                }
////                else if (Foresight >= 5)
////                {
////                    High_Speed = 182;
////                }
////                else if(Foresight >= 2)
////                {
////                    High_Speed = 184;
////                }
////                else
////                {
////                    High_Speed = 186;
////                }
//            
//            }
//            else if( Speed_Set ==3 )
//            {     High_Speed = 165;     }
//        }


        
        
}
/*********************************** ������� *********************************/
void MOTOR_Control(void)
{
    //uint16 num; //var;
    
    /****************************      ���ܼ��      **************************/ 
    //Starting_Check();  
    
    /****************************      �µ����     ***************************/
    //num= adc_once(ADC0_SE12, ADC_8bit); 
//    if( (num>=70) && (Foresight <= 8) )
//    {   
//        Ramp_Flag=1;     //�µ���־λ  ��0���ж����ʱ��0
//        gpio_set   (PTB23,1);
//    }
//    else 
//    {   gpio_set   (PTB23,0);  }
     
    
   /****************************      �ٶȵ���      ***************************/ 
   Adjust_Speed();         

   
   //ʹ�ô�������ʽPID���е���
    MOTOR_Duty  += PID_Realize(&MOTOR_PID, MOTOR_Speed, High_Speed);

    if (MOTOR_Duty>0)
    {
      MOTOR_Duty = range_protect(MOTOR_Duty, 0, 10000);	//�޷�����500
      //�����ת
      tpm_pwm_duty(MOTOR_TPM, MOTOR1_PWM,0);	//ռ�ձ����990������
      tpm_pwm_duty(MOTOR_TPM, MOTOR2_PWM,MOTOR_Duty);	   		//ռ�ձ����990������

    }
    else
    {
      MOTOR_Duty = range_protect(MOTOR_Duty, -500, 0);
      //�����ת
      tpm_pwm_duty(MOTOR_TPM, MOTOR1_PWM,-MOTOR_Duty);	   			//ռ�ձ����990������
      tpm_pwm_duty(MOTOR_TPM, MOTOR2_PWM,0);	//ռ�ձ����990������
    }
   
       
   
}

#if wareon_off
int32 var[4];
#endif

void speed_measure(void)
{    
    //������
    MOTOR_Speed = tpm_pulse_get(TPM0); 
    tpm_pulse_clean(TPM0); 	

 
    /********************************  �ٶȹ���  ******************************/  
    MOTOR_Speed   = (MOTOR_Speed>2000?2000:MOTOR_Speed); 
    /**************************************************************************/  
    
    /**************************** ������λ����ʾ�ٶ� **************************/  
//    #if wareon_off
//      var[0]=Left_MOTOR_Speed;
//      var[1]=Right_MOTOR_Speed;
//      var[2]=Left_High_Speed;
//      var[3]=Right_High_Speed;
//      vcan_sendware(var, sizeof(var));
//    #endif
    /**************************************************************************/
      MOTOR_Control();
}


   


/******************************  �޷�����  ***********************************/
int32 range_protect(int32 duty, int32 min, int32 max)//�޷�����
{
	if (duty >= max)
	{
		return max;
	}
	if (duty <= min)
	{
		return min;
	}
	else
	{
		return duty;
	}
}

/*!
 *  @brief      ɽ��๦�ܵ���������λ��������ʾ������ʾ����
 *  @param      wareaddr    ����������ʼ��ַ
 *  @param      waresize    ��������ռ�ÿռ�Ĵ�С
 *  @since      v5.0
 */
void vcan_sendware(void *wareaddr, uint32_t waresize)
{
  
#define CMD_WARE     3
    uint8_t cmdf[2] = {CMD_WARE, ~CMD_WARE};    //���ڵ��� ʹ�õ�ǰ����
    uint8_t cmdr[2] = {~CMD_WARE, CMD_WARE};    //���ڵ��� ʹ�õĺ�����

    uart_putbuff(VCAN_PORT, cmdf, sizeof(cmdf));    //�ȷ���ǰ����
    uart_putbuff(VCAN_PORT, (uint8_t *)wareaddr, waresize);    //��������
    uart_putbuff(VCAN_PORT, cmdr, sizeof(cmdr));    //���ͺ�����

}
