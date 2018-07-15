#include "System.h"


uint8 Send_OK = 0;
uint8 CNST_Value=65;  //阈
uint8 key_state,Key_Flag=0;
uint8 Adjust_cursor;
uint8 img_display=0;

extern uint8 img[CAMERA_W*CAMERA_H];
extern void PORTA_IRQHandler();
extern void DMA0_IRQHandler();
extern void PIT_IRQHandler();

void System_Init()
{
        DisableInterrupts;
        
        
    camera_init(imgbuff);                                 //摄像头初始化，把图像采集到 imgbuff 地址
    //配置中断服务函数
    set_vector_handler(PORTA_VECTORn ,PORTA_IRQHandler);  //设置 PORTA 的中断服务函数为 PORTA_IRQHandler
    set_vector_handler(DMA0_VECTORn ,DMA0_IRQHandler);    //设置 DMA0 的中断服务函数为 DMA0_IRQHandler

    pit_init_ms(PIT0,10);                                 //pit定时中断(用于按键定时扫描)
    set_vector_handler(PIT_VECTORn ,PIT_IRQHandler);      //设置 PIT0 的中断服务函数为 PIT0_IRQHandler
    enable_irq(PIT_IRQn);				  //使能中断



    /********************** PID参数初始化 ***********************/
    PlacePID_Init(&S3010_PID);                  //舵机PID参数初始化   参数还未调节
    IncPID_Init(&MOTOR_PID,MOTOR);	//电机  PID参数初始化

    /************************** LED 初始化  ***************************/
    //led_init(LED_MAX);
    /************************** 拨码开关 初始化  ***************************/
    Boma_Key_Init();
    /************************** LCD 初始化  ***************************/
    LCD_Init();
    LCD_Clear(LCD_WHITE);

    /************************ 红外测距（坡道） *******************************/
    //adc_init (ADC0_SE12); //PTB2
    /***************************** 蜂鸣器  ***********************************/
    //Buzzer_Init();     //蜂鸣器
     
    /************************ 各种参数 初始化  *******************************/
    Parameter_Init();
    while(!Key_Flag)   //第四个拨码开关
    {
        KEY_Adjust();
        if(Key_Flag)break;
    }
    SCCB_WriteByte (OV7725_CNST, CNST_Value);  //改变图像阈值  白天50  晚上60
     if(img_display){  LCD_Clear(LCD_WHITE);  }   //清屏
     else{  LCD_Clear(LCD_WHITE);      }                  

    /************************ 各种参数 初始化 结束 ***************************/

    /*********************** 舵机 初始化 ***********************/
    tpm_pwm_init(S3010_TPM, S3010_CH,S3010_HZ,S3010_MID);   //初始化 舵机 PWM
    /************************ 电机 初始化  ***********************/
    tpm_pwm_init(MOTOR_TPM, MOTOR1_PWM,MOTOR_HZ,0);   //初始化 电机右2 PWM 
    tpm_pwm_init(MOTOR_TPM, MOTOR2_PWM,MOTOR_HZ,0);   //初始化 电机右2 PWM 
    
    tpm_pulse_init(TPM0,TPM_CLKIN0,TPM_PS_1);        //编码器
    //tpm_pulse_init(TPM0,TPM_CLKIN1,TPM_PS_1); 


    EnableInterrupts;//使能总中断                   //使用定时器0触发，10ms执行一次
    gpio_init(PTC4, GPO, 0); 
    
}



/********************************  按键  *************************************/

void Boma_Key_Init(void)
{
          gpio_init(PTB1, GPI, 0);            // 1
     port_init_NoALT(PTB1, PULLDOWN);         //保持复用不变，仅仅改变配置选项

          gpio_init(PTB2, GPI, 0);            // 2
     port_init_NoALT(PTB2, PULLDOWN);         //保持复用不变，仅仅改变配置选项

          gpio_init(PTB3, GPI, 0);            // 3
     port_init_NoALT(PTB3, PULLDOWN);         //保持复用不变，仅仅改变配置选项

          gpio_init(PTB7, GPI, 0);            // 4
     port_init_NoALT(PTB7, PULLDOWN);         //保持复用不变，仅仅改变配置选项

          gpio_init(PTB8, GPI, 0);            // 5
     port_init_NoALT(PTB8, PULLDOWN);         //保持复用不变，仅仅改变配置选项

          gpio_init(PTB9, GPI, 0);            // 6
     port_init_NoALT(PTB9, PULLDOWN);         //保持复用不变，仅仅改变配置选项

          gpio_init(PTB10, GPI, 0);             // 7
     port_init_NoALT(PTB10, PULLDOWN);         //保持复用不变，仅仅改变配置选项

          gpio_init(PTB11, GPI, 0);             // 8
     port_init_NoALT(PTB11, PULLDOWN);         //保持复用不变，仅仅改变配置选项

     //按键
    key_init(KEY_U);
    key_init(KEY_D);
    key_init(KEY_L);
    key_init(KEY_R);
        
}


void Parameter_Init(void)
{
    if(gpio_get(PTB2) == KEY_UP)          //图像显示
    {
        img_display=1;
    }
    else
    {
        img_display=0;
          
    }


/******************************  速度模式   **********************************/
    //7号拨下 8号拨下  00
    if(gpio_get(PTB9) == KEY_UP&&gpio_get(PTB10) == KEY_UP)     //速度1
    {
        Speed_Set = 1;
        Set = 0;
    }
    //7号拨上 8号拨下  10
    else if(gpio_get(PTB9) == KEY_UP&&gpio_get(PTB10) == KEY_DOWN) //速度2
    {
        Speed_Set = 2;
        Set = 1;
    }
    //7号拨下 8号拨上  01
    else if(gpio_get(PTB9) == KEY_DOWN&&gpio_get(PTB10) == KEY_DOWN) //速度3
    {
        Speed_Set = 3;
        Set = 2;
    }
    //7号拨上 8号拨上  11
    else if(gpio_get(PTB9) == KEY_DOWN&&gpio_get(PTB10) == KEY_UP)    //速度4
    {
        Speed_Set = 4;
        Set = 3;
    }      


   
}

uint8 wait_key_down()
{
    uint8 ret_key=0xff;
    uint16 key_time=0;
    while(1)
    {
      if(key_check(KEY_U)==KEY_DOWN)
          ret_key=KEY_U;
      else if(key_check(KEY_D)==KEY_DOWN)
          ret_key=KEY_D;
      else if(key_check(KEY_L)==KEY_DOWN)
          ret_key=KEY_L;
      else if(key_check(KEY_R)==KEY_DOWN)
          ret_key=KEY_R;
      if(ret_key!=0xff)
      {
          key_state=KEY_DOWN;
          while(key_get(ret_key)==KEY_DOWN)
          {
              DELAY_MS(1);
              key_time++;
          }
          if(key_time>300)
          {
              key_state=KEY_HOLD;
          }
          break;
      }
    }
    return ret_key;
}


void KEY_Adjust(void)
{
  uint8 key=0;

  Show_Str(5, 5, LCD_BLACK,LCD_WHITE, "OV7725_CNST:",16,1);
  Show_Str(5, 21, LCD_BLACK,LCD_WHITE,"S3010_KP1:  ",16,1);
  Show_Str(5, 37, LCD_BLACK,LCD_WHITE,"S3010_K D:  ",16,1);
  Show_Str(5, 53, LCD_BLACK,LCD_WHITE,"S3010_KPx:  ",16,1);
  
  if(Adjust_cursor==1)
  { LCD_Num_BC(105,5,CNST_Value,16,3,LCD_RED,LCD_WHITE);
    LCD_Num_BC(105,21,S3010[Set][0],16,3,LCD_BLACK,LCD_WHITE);
    LCD_Num_BC(105,37,S3010[Set][2],16,3,LCD_BLACK,LCD_WHITE);
    LCD_Num_BC(105,53,S3010[Set][3],16,3,LCD_BLACK,LCD_WHITE);
    
  }
  else if(Adjust_cursor==2)
  { LCD_Num_BC(105,5 ,CNST_Value,16,3,LCD_BLACK,LCD_WHITE);
    LCD_Num_BC(105,21,S3010[Set][0],16,3,LCD_RED,LCD_WHITE);
    LCD_Num_BC(105,37,S3010[Set][2],16,3,LCD_BLACK,LCD_WHITE);
    LCD_Num_BC(105,53,S3010[Set][3],16,3,LCD_BLACK,LCD_WHITE);
  }
  else if(Adjust_cursor==3)
  { LCD_Num_BC(105,5,CNST_Value,16,3,LCD_BLACK,LCD_WHITE);
    LCD_Num_BC(105,21,S3010[Set][0],16,3,LCD_BLACK,LCD_WHITE);
    LCD_Num_BC(105,37,S3010[Set][2],16,3,LCD_RED,LCD_WHITE);
    LCD_Num_BC(105,53,S3010[Set][3],16,3,LCD_BLACK,LCD_WHITE);
  }
  else if(Adjust_cursor==4)
  { LCD_Num_BC(105,5,CNST_Value,16,3,LCD_BLACK,LCD_WHITE);
    LCD_Num_BC(105,21,S3010[Set][0],16,3,LCD_BLACK,LCD_WHITE);
    LCD_Num_BC(105,37,S3010[Set][2],16,3,LCD_BLACK,LCD_WHITE);
    LCD_Num_BC(105,53,S3010[Set][3],16,3,LCD_RED,LCD_WHITE);
  }
  else if(Adjust_cursor==5)
  { LCD_Num_BC(105,5,CNST_Value,16,3,LCD_BLACK,LCD_WHITE);
    LCD_Num_BC(105,21,S3010[Set][0],16,3,LCD_BLACK,LCD_WHITE);
    LCD_Num_BC(105,37,S3010[Set][2],16,3,LCD_BLACK,LCD_WHITE);
    LCD_Num_BC(105,53,S3010[Set][3],16,3,LCD_BLACK,LCD_WHITE);
    Show_Str(30, 70, LCD_RED,LCD_WHITE,"SET OK !!",16,1);
  }
  else
  { LCD_Num_BC(105,5,CNST_Value,16,3,LCD_BLACK,LCD_WHITE);
    LCD_Num_BC(105,21,S3010[Set][0],16,3,LCD_BLACK,LCD_WHITE);
    LCD_Num_BC(105,37,S3010[Set][2],16,3,LCD_BLACK,LCD_WHITE);
    LCD_Num_BC(105,53,S3010[Set][3],16,3,LCD_BLACK,LCD_WHITE);
    LCD_Num_BC(105,69,Speed_Set,16,3,LCD_BLACK,LCD_WHITE);
  }

  key = wait_key_down();

  switch (key)
  {
    case KEY_U:
    {
        //Adjust_Flag=1;
        Adjust_cursor++;  //光标移动到下一个
        if(Adjust_cursor>MaxLengh)  Adjust_cursor=0;  //循环光标

    }
    break;
    case KEY_D:
    {
         switch(Adjust_cursor)
         {
            case 1:CNST_Value++;
                   break;

            case 2:S3010[Set][0]+=10;
                   break;

            case 3:S3010[Set][2]+=10;
                   break;

            case 4:S3010[Set][3]+=10;
                   break;

         }
    }
    break;
    case KEY_L:
    {
         switch(Adjust_cursor)
         {
            case 1:CNST_Value--;
                   break;

            case 2:S3010[Set][0]-=10;
                   break;

            case 3:S3010[Set][2]-=10;
                   break;

            case 4:S3010[Set][3]-=10;
                   break;

         }

    }
    break;
    case KEY_R:
    {
        Key_Flag=1;
    }
    break;

  }
  
}

/*****************    蜂鸣器测试标志位    *******************/
void Buzzer_Init()
{   
    gpio_init(PTB23,GPO,0);
}

//蜂鸣器测试标志位
void BuzzerTest(unsigned char flag)
{
  unsigned char Flag=0;
  Flag=0;
  Flag=flag;
  
  if(Flag)
  {
    BuzzerRing;
  }
  
  else
  {
    BuzzerQuiet;
  }
}
