#include "Control.h"
#include "math.h"

/****************************    其他控制变量    ****************************/ 
char Set = 0,Speed_Set=0;
uint8   BangBang_Flag = 0;        //在增量式PID里
uint8 	Run_Flag  = 0;            //起跑标志
extern uchar Foresight;
extern int32 S3010_Duty;

int32 S3010_Duty =29000;   //新添加的，注意以后修改

/****************************    坡道控制量    ****************************/ 
uint8  Ramp_Flag=  0;       //坡道标志位          
uint16 Ramp_Time=  0;
uint16 Ramp_Delay= 80;      //坡道延时时间

/****************************        结束        ****************************/ 

/****************************    起跑识别变量    ****************************/ 
uint8   Going_Flag = 0;
uint8   Stop_Flag = 0;
uint8   Stoping_Flag = 0;
uint8   Annulus_Check=0;

/***************************************************************************/ 


/****************************    速度控制变量    ****************************/ 
int16   Starting_Time=300;              //起跑延时时间2S
int16    Starting1_Time;                 //起跑偏移延时
//int32  Start_Pian_Delaytime=100;        //起跑偏移延时
int8    Stop_Time=0;                    //停车延时
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



extern uchar Annulus_Flag;  //圆环标志位


/**************************    发车线检测  ***********************************/
void Starting_Check(void)
{ 
    if (Starting_Time==0 && !Going_Flag && !Stop_Flag)
    {
        Going_Flag=1;
        Starting1_Time = 370;    //起跑开始关闭环路检测 
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
            Stop_Time=20;  //40 //检测到起跑线以后延时停车               
        }
        if (Stop_Time==15)
        {
            Going_Flag=0;
            //Starting_Time=-10;
        }
    }  

} 


/******************************** 速度控制 ***********************************/
void Adjust_Speed(void)
{
  High_Speed = 80;
       
        /*****************************  速度1档  ******************************/
//        if(0)  //gai
//        {
//            if( 0/*loop_times==0*/ )
//            {
//                High_Speed = 130;  //稳定130      
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
//                //  High_Speed = 170;  //匀速170
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
/*********************************** 电机控制 *********************************/
void MOTOR_Control(void)
{
    //uint16 num; //var;
    
    /****************************      起跑检测      **************************/ 
    //Starting_Check();  
    
    /****************************      坡道检测     ***************************/
    //num= adc_once(ADC0_SE12, ADC_8bit); 
//    if( (num>=70) && (Foresight <= 8) )
//    {   
//        Ramp_Flag=1;     //坡道标志位  置0在中断里，延时置0
//        gpio_set   (PTB23,1);
//    }
//    else 
//    {   gpio_set   (PTB23,0);  }
     
    
   /****************************      速度调节      ***************************/ 
   Adjust_Speed();         

   
   //使用串级增量式PID进行调节
    MOTOR_Duty  += PID_Realize(&MOTOR_PID, MOTOR_Speed, High_Speed);

    if (MOTOR_Duty>0)
    {
      MOTOR_Duty = range_protect(MOTOR_Duty, 0, 10000);	//限幅保护500
      //电机正转
      tpm_pwm_duty(MOTOR_TPM, MOTOR1_PWM,0);	//占空比最大990！！！
      tpm_pwm_duty(MOTOR_TPM, MOTOR2_PWM,MOTOR_Duty);	   		//占空比最大990！！！

    }
    else
    {
      MOTOR_Duty = range_protect(MOTOR_Duty, -500, 0);
      //电机反转
      tpm_pwm_duty(MOTOR_TPM, MOTOR1_PWM,-MOTOR_Duty);	   			//占空比最大990！！！
      tpm_pwm_duty(MOTOR_TPM, MOTOR2_PWM,0);	//占空比最大990！！！
    }
   
       
   
}

#if wareon_off
int32 var[4];
#endif

void speed_measure(void)
{    
    //编码器
    MOTOR_Speed = tpm_pulse_get(TPM0); 
    tpm_pulse_clean(TPM0); 	

 
    /********************************  速度过滤  ******************************/  
    MOTOR_Speed   = (MOTOR_Speed>2000?2000:MOTOR_Speed); 
    /**************************************************************************/  
    
    /**************************** 虚拟上位机显示速度 **************************/  
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


   


/******************************  限幅保护  ***********************************/
int32 range_protect(int32 duty, int32 min, int32 max)//限幅保护
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
 *  @brief      山外多功能调试助手上位机，虚拟示波器显示函数
 *  @param      wareaddr    波形数组起始地址
 *  @param      waresize    波形数组占用空间的大小
 *  @since      v5.0
 */
void vcan_sendware(void *wareaddr, uint32_t waresize)
{
  
#define CMD_WARE     3
    uint8_t cmdf[2] = {CMD_WARE, ~CMD_WARE};    //串口调试 使用的前命令
    uint8_t cmdr[2] = {~CMD_WARE, CMD_WARE};    //串口调试 使用的后命令

    uart_putbuff(VCAN_PORT, cmdf, sizeof(cmdf));    //先发送前命令
    uart_putbuff(VCAN_PORT, (uint8_t *)wareaddr, waresize);    //发送数据
    uart_putbuff(VCAN_PORT, cmdr, sizeof(cmdr));    //发送后命令

}
