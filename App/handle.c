#include "Handle.h"

//大环路的检测，还有是否初始时左右边界的赋值，赋值后是否可以，需要验证，留个记号


#define Display_OFF_ON   1    //补线显示开关，1为开 0为关

uint8 colour[2] = {255, 0},black=1,white=0; //0 和 1 分别对应的颜色
//注：山外的摄像头 0 表示 白色，1表示 黑色
////////////////////0       2       4       6       8

        //圆环入口连接正常
uchar Width_Max[61]={10,11,12,13,14,15,16,17,18,19,
                     20,21,22,23,24,25,26,27,28,29,
                     30,31,32,33,34,35,36,37,38,39,
                     40,41,42,43,44,45,46,47,48,49,
                     50,51,52,53,54,55,56,57,58,59,
                     60,61,62,63,64,65,66,67,68,69,70};

//加权平均参数

uchar Weight[60]={  6  ,6  ,6  ,6  ,6  ,6 ,6 ,6 ,6 ,6 , //最近十行//不压线，用于弯道666666666666666666
                    5  ,5  ,15  ,15 ,15 ,15 ,15 ,15 ,10 ,10 , //40-49行  49距车15cm
                    10  ,10  ,10  ,10  ,5  ,5  ,5  ,5  ,5  ,5  ,//30-39行  39距车15+9=24  30距车24+12=36
                    5 ,5 ,5 ,5 ,5  ,5  ,5  ,5  ,5  ,5  , //20-29行  20距车15+46=61
                    1  ,1  ,1  ,1  ,1  ,1 ,1 ,1 ,1  ,1  , //0-19行， 10距车61+50=111      1111111111111111111111
                    1  ,1  ,1  ,1  ,1  ,1  ,1  ,1  ,1  ,1 }; //0-9行，基本用不到     11111111111111111

uint8 i;
uchar Left_Line[62], Right_Line[62], Mid_Line[62];	// 原始左右边界数据
uchar Left_Add_Line[62], Right_Add_Line[62];		// 左右边界补线数据
uchar Left_Add_Flag[62], Right_Add_Flag[62];		// 左右边界补线标志位
uchar Width_Real[62];	// 实际赛道宽度
uchar Width_Add[62];	// 补线赛道宽度
uchar Width_Min;		// 最小赛道宽度

uchar Ek_compar;    // 两行中间值与两行的中行的Mid_Line的差值
uchar Foresight;	// 前瞻，用于速度控制
uchar Out_Side = 0;	// 丢线控制
uchar Line_Count;	// 记录成功识别到的赛道行数

uchar Left_Add_Start, Right_Add_Start;	// 左右补线起始行坐标
uchar Left_Add_Stop, Right_Add_Stop;	// 左右补线结束行坐标
float Left_Ka = 0, Right_Ka = 0;
float Left_Kb = 1, Right_Kb = 79;	// 最小二乘法参数

uchar Left_Hazard_Flag, Right_Hazard_Flag, Hazard_Flag;	// 左右障碍物标志
uint8 Hazard_Time;
uchar Left_Max, Right_Min;

uchar Starting_Line_Flag = 0;	// 起跑线标志位
uchar Starting_Line_Times = 0;	// 起跑线标志位
uchar Starting_Line_Time = 0;	// 起跑线标志位
char Last_Point = 40;

/************** 环路相关变量 *************/
uchar Annulus1_Flag=0;
uchar Annulus2_Flag=0;
uchar Annulus_Mid=0;
uchar Annulus_Left;
uchar Annulus_Right;
uchar Annulus_Count[4] = {0, 0, 0, 0};	         // 0右环路，1左环路
uchar Annulus_Times = 0;	// 环路次数
uchar Annulus2_Delay = 0;
uint16 Annulus_DelayMs = 0;	// 检测到环路后延时一段时间后自动清除环路标志位
uint8 first_signal=0;
uint8 second_signal=0;
uint8 third_signal=0;
uint8 fourth_signal=0;
uint8 fifth_signal=0;
uint8 sixth_signal=0;
uchar Annulus_Mode = 0;
uint8 seventh_signal=0;
extern int32 S3010_Duty;
/************** 环路相关变量 *************/


void Annulus2_Control(void)
{
	Annulus_Right = Annulus_Count[Annulus_Times++];
        Annulus_Left = !Annulus_Right;

        Annulus_Mode = 1;	// 开始进入环路
        Annulus2_Flag = 1;	// 环岛标志位 置1
        //first_signal=1;

        //led(LED_MAX,LED_ON);// 打开LED指示灯		全部初始化成功

        if (Annulus_Times >= 4)
        {
                Annulus_Times = 0;
        }
}
void Annulus1_Control(void)
{
	Annulus_Right = Annulus_Count[Annulus_Times++];
        Annulus_Left = !Annulus_Right;

        Annulus_Mode = 1;	// 开始进入环路
        Annulus1_Flag = 1;	// 环岛标志位 置1
        //first_signal=1;

        //led(LED_MAX,LED_ON);// 打开LED指示灯		全部初始化成功

        if (Annulus_Times >= 4)
        {
                Annulus_Times = 0;
        }
}

/*
*	环路图像处理算法
*
*	说明：环路专用，不含其他元素
*/
void Annulus2_Handle(uchar *data)
{
	uchar i;	// 控制行
	uchar res;

	Line_Count = 0;	// 赛道行数复位

	Left_Add_Start = 0;		// 复位补线起始行坐标
	Right_Add_Start = 0;
	Left_Add_Stop = 0;
	Right_Add_Stop = 0;

	/***************************** 第一行特殊处理 *****************************/
	res = First_Line_Handle(data);
	if (res == 0)
	{
		Out_Side++;	//丢线
		return;
	}
	if (Left_Add_Start || Right_Add_Start)
	{
		Width_Min = 60;
	}
	/*************************** 第一行特殊处理结束 ***************************/

	for (i = 59; i >= 15;)   //15
	{
		i -= 2;

		if (!data[i*80 + Mid_Line[i+2]])//前2行中点在本行为黑点，赛道结束
		{
			i += 1;
			break;
		}
		else
		{
			Traversal_Mid_Line(i, data, Mid_Line[i+2], 1, 79, Left_Line, Right_Line, Left_Add_Line, Right_Add_Line);
			/************************** 补线检测开始 **************************/
//			if (Width_Real[i] >= Width_Min)	// 赛道宽度变宽,进入左环岛
//			{
                        
                               if(i>=55 && Width_Real[i] >= Width_Min && Right_Add_Flag[i]==1&&Left_Add_Flag[i]==0&&fourth_signal==0)
                              {       
                                      
                                     first_signal=1;
                                     if (first_signal==1)	//第一个标志位  补直线
                                     {
                                       Left_Add_Line[i] = Left_Line[i];                           
                                       Right_Add_Line[i]=range_protect((Left_Add_Line[i] + Width_Max[i]-7),0,79);
                                     }
                                
                                     
                             }
                             if( (i>=43&& i<=55)&& first_signal==1&& Right_Add_Flag[i]==0&&(Width_Real[i]-Width_Real[i-2]>0) && (Width_Real[i-4]-Width_Real[i-6]<0)&&second_signal==0&&fourth_signal==0)//(Width_Real[i] >= Width_MinWidth_Real[i]-Width_Real[i-2]>0) && (Width_Real[i-4]-Width_Real[i-6]<0)
                             {
                                           //检测到中间的圆，为第二个标志位进环做铺垫
                                second_signal=1;
                                
                             }
                                  
                             if( Width_Real[i-2] >= Width_Min && second_signal==1&&fourth_signal==0)//Width_Real[i-2] >= Width_Min && second_signal==1&&fourth_signal==0
                             {
                                 third_signal=1;
                                 first_signal=0;
                                 if(second_signal==1)				// 第二个标志位  进环
                                 {
                                   
                                   
                                   S3010[0][0] = 400;
                                   Right_Add_Line[i] = Right_Line[i];
                                   Left_Add_Line[i]=range_protect((Right_Line[i] - Width_Max[i]+3 ),0,79);
                                   if(third_signal==1&&Right_Add_Flag[i]==0&&Left_Add_Flag[i]==0&&Left_Add_Flag[i-2]==0&&Width_Real[i] <= Width_Min&&Width_Real[i-2] <= Width_Min&&Right_Add_Flag[i-2]==0)
                                   {
                                     S3010[0][0] = 258;
                                     sixth_signal=1;//进环优化
                                   }
                                }
                                 
                             }
                             
                            

                             if(third_signal==1&&sixth_signal==1&&i>=40&&Width_Real[i]>Width_Min&&(Left_Line[i]-Left_Line[i-2]>=0)&&(Left_Line[i-2]-Left_Line[i-4]>=0)&&fourth_signal==0&&Left_Add_Flag[i]==0&&Left_Add_Flag[i-8]==1)//Width_Real[i-2]<= 63&& Width_Real[i-4]<= 63&&Right_Add_Flag[i]==1
                             {
                                S3010[0][0] = 258;
                               
                                 gpio_init(PTE24,GPO,1);
                                 if(Right_Add_Flag[i]==0&&Right_Add_Flag[i-2]==0&&Right_Add_Flag[i+2]==0)
                                 {
                                     
                                   tpm_pwm_init(S3010_TPM, S3010_CH,S3010_HZ,30000); 
                                   DELAY_MS(200);
                                 }
                                 else
                                 {
                                   tpm_pwm_init(S3010_TPM, S3010_CH,S3010_HZ,33800);                       //第三个标志位，出环
                                   DELAY_MS(300);
                                 }
                                
                                
                                fourth_signal=1;
                                
                                                                                                                            //第三个标志位，出环
                             }
                             if(fourth_signal==1&&i>=55&&Left_Add_Flag[i]==0 && Right_Add_Flag[i]==1&&Width_Real[i]>=Width_Min)//fourth_signal==1&&i>=56&&Width_Real[i]>=Width_Min&&Right_Add_Flag[i]==0&&Left_Add_Flag==0
                             {
                                gpio_init(PTE24,GPO,0);
                                fifth_signal=1;                                                                                //第五个标志位，出环岛
                                Left_Add_Line[i] = Left_Line[i];                           
                                Right_Add_Line[i]=range_protect((Left_Add_Line[i] + (Width_Max[i]-20)),0,79);
                                S3010[0][0] = 258;
                               
                             }

                             if(fifth_signal==1&&i>=56&&Left_Add_Flag[i]==0 && Right_Add_Flag[i]==0&&Width_Real[i]<=(Width_Min+8) )
                             {
                               first_signal=0;
                               second_signal=0;
                               third_signal=0;
                               fourth_signal=0;
                               fifth_signal=0;
                               sixth_signal=0;//第六个标志位，清标志位
                               Annulus2_Flag = 0;
                               gpio_init(PTE24,GPO,1);
                               S3010[0][0] = 258;
                               S3010[0][2] = 360;
                             }
                              

			/************************** 补线检测结束 **************************/
			Mid_Line[i] = (Right_Add_Line[i] + Left_Add_Line[i]) / 2;
			Width_Add[i] = Right_Add_Line[i] - Left_Add_Line[i];
			if (Width_Add[i] < Width_Min)
			{
				Width_Min = Width_Add[i];
			}
			if (Width_Add[i] <= 6)
			{
				i += 2;
				break;
			}
		}
	}
	Line_Count = i;

#if   Display_OFF_ON
	for (i = 61; i >= Line_Count+2;)
	{
		i -= 2;
		/**//************************* OLED显示边界 *************************/
		/**/data[i*80 + Left_Add_Line[i] + 2] = 0;	// 显示补线后的左边界，不用时屏蔽
		/**/data[i*80 + Right_Add_Line[i] - 2] = 0;	// 显示补线后的右边界，不用时屏蔽
		/**/data[i*80 + Mid_Line[i]] = 0;			// 显示中线，不用时屏蔽
//		/**/data[i*80 + Left_Line[i] + 2] = 0;		// 显示原始左边界，不用时屏蔽
//		/**/data[i*80 + Right_Line[i] - 2] = 0;		// 显示原始右边界，不用时屏蔽
		/**//************************* OLED显示边界 *************************/
	}
#endif

        //Annulus_Flag = 0;	// 清除环路状态标志位
	/************************** 环路出入状态判断结束 **************************/
}

void Annulus1_Handle(uchar *data)
{
	uchar i;	// 控制行
	uchar res;

	Line_Count = 0;	// 赛道行数复位

	Left_Add_Start = 0;		// 复位补线起始行坐标
	Right_Add_Start = 0;
	Left_Add_Stop = 0;
	Right_Add_Stop = 0;

	/***************************** 第一行特殊处理 *****************************/
	res = First_Line_Handle(data);
	if (res == 0)
	{
		Out_Side++;	//丢线
		return;
	}
	if (Left_Add_Start || Right_Add_Start)
	{
		Width_Min = 60;
	}
	/*************************** 第一行特殊处理结束 ***************************/

	for (i = 59; i >= 15;)   //15
	{
		i -= 2;

		if (!data[i*80 + Mid_Line[i+2]])//前2行中点在本行为黑点，赛道结束
		{
			i += 1;
			break;
		}
		else
		{
			Traversal_Mid_Line(i, data, Mid_Line[i+2], 1, 79, Left_Line, Right_Line, Left_Add_Line, Right_Add_Line);
			/************************** 补线检测开始 **************************/
//			if (Width_Real[i] >= Width_Min)	// 赛道宽度变宽,进入左环岛
//			{
                        
                               if(i>=55 && Width_Real[i] >= Width_Min &&Left_Add_Flag[i]==1&&Right_Add_Flag[i]==0&&fourth_signal==0)
                              {       
                                      
                                     first_signal=1;
                                     if (first_signal==1)	//第一个标志位  补直线
                                     {
                                       Right_Add_Line[i] = Right_Line[i];                           
                                       Left_Add_Line[i]=range_protect((Right_Add_Line[i] - Width_Max[i]),0,79);
                                     }
                                
                                     
                             }
                             if( (i>=43&& i<=55)&& first_signal==1&& Left_Add_Flag[i]==0&&(Width_Real[i]-Width_Real[i-2]>0) && (Width_Real[i-4]-Width_Real[i-6]<0)&&second_signal==0&&fourth_signal==0)//(Width_Real[i] >= Width_MinWidth_Real[i]-Width_Real[i-2]>0) && (Width_Real[i-4]-Width_Real[i-6]<0)
                             {
                                       //检测到中间的圆，为第二个标志位进环做铺垫
                                second_signal=1;
                                
                             }
                                  
                             if( Width_Real[i-2] >= Width_Min && second_signal==1&&fourth_signal==0)//Width_Real[i-2] >= Width_Min && second_signal==1&&fourth_signal==0
                             {
                                 third_signal=1;
                                 first_signal=0;
                                 if(second_signal==1)				// 第二个标志位  进环
                                 {
                                   Left_Add_Line[i] =  Left_Line[i];
                                   Right_Add_Line[i]=range_protect((Left_Add_Line[i] + (Width_Max[i]-3) ),0,79);
                                  }
                                 
                             }

                             if(third_signal==1&&i>=40&&Width_Real[i]>Width_Min&&(Right_Line[i]-Right_Line[i-2]<=0)&&(Right_Line[i-2]-Right_Line[i-4]<=0)&&fourth_signal==0&&Right_Add_Flag[i]==0&&Right_Add_Flag[i-8]==1)//Width_Real[i-2]<= 63&& Width_Real[i-4]<= 63&&Right_Add_Flag[i]==1
                             {
                                 if(Left_Add_Flag[i]==0&&Left_Add_Flag[i-2]==0&&Left_Add_Flag[i+2]==0)
                                 {
                                   tpm_pwm_init(S3010_TPM, S3010_CH,S3010_HZ,25000); 
                                   DELAY_MS(180);
                                   
                                 }
                                 else
                                 {
                                   tpm_pwm_init(S3010_TPM, S3010_CH,S3010_HZ,24000); 
                                   DELAY_MS(300);
                                 }
                                
                                
                                fourth_signal=1;
                                second_signal=0;
                                                                                                                            //第三个标志位，出环
                             }
                             if(fourth_signal==1&&i>=55&&Right_Add_Flag[i]==0&&Left_Add_Flag[i]==1)//&&i>=55&&Width_Real[i]>=Width_Min&&Right_Add_Flag[i]==0&&Left_Add_Flag[i]==1
                             {
                              
                                gpio_init(PTE24,GPO,0);
                                fifth_signal=1;                                                                                //第五个标志位，出环岛
                                Right_Add_Line[i] = Right_Line[i];                           
                                Left_Add_Line[i]=range_protect((Right_Add_Line[i] - (Width_Max[i]+30)),0,79);

                             }

                             if(fifth_signal==1&&i>=56&&Left_Add_Flag[i]==0 && Right_Add_Flag[i]==0&&Width_Real[i]<=(Width_Min+8) )
                             {
                               first_signal=0;
                               second_signal=0;
                               third_signal=0;
                               fourth_signal=0;
                               fifth_signal=0;                                                                          //第六个标志位，清标志位
                               Annulus1_Flag = 0;
                               gpio_init(PTE24,GPO,1);
                            
                             }
                              

			/************************** 补线检测结束 **************************/
			Mid_Line[i] = (Right_Add_Line[i] + Left_Add_Line[i]) / 2;
			Width_Add[i] = Right_Add_Line[i] - Left_Add_Line[i];
			if (Width_Add[i] < Width_Min)
			{
				Width_Min = Width_Add[i];
			}
			if (Width_Add[i] <= 6)
			{
				i += 2;
				break;
			}
		}
	}
	Line_Count = i;

#if   Display_OFF_ON
	for (i = 61; i >= Line_Count+2;)
	{
		i -= 2;
		/**//************************* OLED显示边界 *************************/
		/**/data[i*80 + Left_Add_Line[i] + 2] = 0;	// 显示补线后的左边界，不用时屏蔽
		/**/data[i*80 + Right_Add_Line[i] - 2] = 0;	// 显示补线后的右边界，不用时屏蔽
		/**/data[i*80 + Mid_Line[i]] = 0;			// 显示中线，不用时屏蔽
//		/**/data[i*80 + Left_Line[i] + 2] = 0;		// 显示原始左边界，不用时屏蔽
//		/**/data[i*80 + Right_Line[i] - 2] = 0;		// 显示原始右边界，不用时屏蔽
		/**//************************* OLED显示边界 *************************/
	}
#endif

        //Annulus_Flag = 0;	// 清除环路状态标志位
	/************************** 环路出入状态判断结束 **************************/
}




/************************************ 新算法 **********************************/

/*
*	图像算法参数初始化
*
*	说明：仅影响第一行特殊处理
*/
void Image_Para_Init(void)
{
	Mid_Line[61] = 40;
	Left_Line[61] = 1;
	Right_Line[61] = 79;
	Left_Add_Line[61] = 1;
	Right_Add_Line[61] = 79;
	Width_Real[61] = 78;
	Width_Add[61] = 78;
}


/*
*	图像处理算法
*
*	说明：处理普通图像，包括十字、障碍
*/
void Image_Handle(uchar *data)
{
	uchar i;	// 控制行
    uint8 j;

	uchar res;	// 用于结果状态判断
	float Result;	// 用于结果状态判断
        uchar Add_Start_Max;

	Line_Count = 0;	// 赛道行数复位
	Starting_Line_Times=0;
        Starting_Line_Time=0;
        Starting_Line_Flag=0;
	Left_Hazard_Flag = 0;	// 复位左右障碍物标志位
	Right_Hazard_Flag = 0;
	Hazard_Flag=0;

	Left_Add_Start = 0;		// 复位补线起始行坐标
	Right_Add_Start = 0;
	Left_Add_Stop = 0;
	Right_Add_Stop = 0;

	/***************************** 第一行特殊处理 *****************************/
	res = First_Line_Handle(data);
	if (res == 0)
	{
		Out_Side = 1;	// 丢线
		return;
	}
	Out_Side = 0;
	Line_Count = 59;
	/*************************** 第一行特殊处理结束 ***************************/

	for (i = 59; i >= 10;)	// 仅处理前50行图像，隔行后仅处理25行数据
	{
		i -= 2;	// 隔行处理，减小单片机负荷
		Traversal_Mid_Line(i, data, Mid_Line[i+2], 1, 79, Left_Line, Right_Line, Left_Add_Line, Right_Add_Line);
		
        /**************************** 起跑线检测开始 ****************************/
        if (Width_Real[i] <= 4 && i>40)
        {
             if (Last_Point >35 && Last_Point < 45)
              {
                 for(j=Left_Line[i+2];j<Right_Line[i+2];j++)
                  {
                      if (Starting_Line_Time==0)
                      {
                          if(!data[80*(i-2)+j])
                          {
                              Starting_Line_Times++;
                              Starting_Line_Time=1;
                          }
                      }
                      else
                      {
                          if(data[80*(i-2)+j])
                          {
                              Starting_Line_Time=0;
                          }
                      }
                  }
                  if(Starting_Line_Times>3 && Starting_Line_Times<15)
                  {
                     Starting_Line_Flag=1;
                  }
              }
        }
         /**************************** 简单的环岛检测，环境影响小时使用 ****************************/
        
        if(Right_Add_Flag[i]==1&&Left_Add_Flag[i]==0&&Left_Add_Flag[i-2]==0&&Left_Add_Flag[i-4]==0&&Right_Add_Flag[i-2]==1&&i>=50&&(Width_Real[i+2] > Width_Real[Right_Add_Start]) && ((Width_Real[i+2] - Width_Real[i+4]) <=10) && ((Right_Line[i] - Right_Line[i-2])>=0)&&(Width_Real[i]>Width_Min))
          
        {
          
           if((Left_Line[i]-Left_Line[i-1]>=0)&&(Left_Line[i]-Left_Line[i-2]>=0)&& !Left_Add_Flag[i+2] && Right_Add_Start &&  !Right_Add_Stop && !Left_Add_Start )
          {
           gpio_init(PTC20,GPO,1);
           Annulus2_Control();
          }
           
        }
     
          if(Left_Add_Flag[i]==1&&Right_Add_Flag[i]==0&&Right_Add_Flag[i-2]==0&&Right_Add_Flag[i-4]==0&&Left_Add_Flag[i-2]==1&&i>=50&&(Width_Real[i+2] > Width_Real[Left_Add_Start]) && ((Width_Real[i+2] - Width_Real[i+4]) <=10) && ((Left_Line[i] - Left_Line[i-2])>=0)&&(Width_Real[i]>Width_Min))
          
        {
          
           if(Annulus2_Flag ==0&&(Right_Add_Flag[i]-Right_Add_Flag[i-1]>=0)&&(Right_Add_Flag[i]-Right_Add_Flag[i-2]>=0)&&Left_Add_Start&&!Left_Add_Stop&&!Right_Add_Start  )
           { 
             if(Left_Add_Flag[i-4] && !Right_Add_Flag[i-4] && Left_Add_Start &&  !Left_Add_Stop && !Right_Add_Start)
               {
                 gpio_init(PTC4,GPO,1);
                 Annulus1_Control();
               }
           }
           
        }
        
        
       
          

                
                        /**************************** 简单的环岛检测，环境影响小时使用 ****************************/
        /**************************** 障碍检测 ****************************/
        if (i < 57 && i > 30 && Width_Real[i] < 50 && Annulus_Check && !Starting_Line_Flag)
	      {
            if (Right_Add_Line[i+2] - Right_Add_Line[i]<4 && Left_Add_Line[i] > Left_Add_Line[i+2] + 6 && !Left_Add_Start && !Right_Add_Flag[i])	// 本行与前一行左边界有较大突变且没有补线
		    {
                for (j=Left_Add_Line[i+2];j<Left_Add_Line[i]-1;j++) //j<可以优化
                {
                     if(data[80*(i-2) + j] && !data[80*(i-2) + j+4 ])
                     {
                         Hazard_Flag=j;
                     }
                     else if (data[80*(i-3) + j] && !data[80*(i-3) + j+4])
                    {
                        Hazard_Flag=j;
                    }
                }
                if(Hazard_Flag && Left_Add_Line[i] - Hazard_Flag > 4) //后面判断可以不要
                {
                    Hazard_Flag = (Left_Add_Line[i] + Hazard_Flag)/2;
                    for (j=i;j>(i-30);j--)  //i-20
                    {
                        if(data[80*j + Hazard_Flag] && data[80*(j-1) + Hazard_Flag] && data[80*(j-2) + Hazard_Flag])
                        {
                            //Left_Hazard_Flag = i;	// 障碍物标志位置位
                            //save_var(VAR2, Left_Hazard_Flag);
                            if(data[80*(i-2) + Left_Add_Line[i]+1] && data[80*(i-2) + Left_Add_Line[i]+3] && data[80*(i-2) + Left_Add_Line[i]+5])
                            {
                                Left_Hazard_Flag = i;	 // 障碍物标志位置位
                                // save_var(VAR3, Left_Hazard_Flag);
                            }
                        }
                    }
                }
            }
            else if (Left_Add_Line[i] - Left_Add_Line[i+2] <4 && Right_Add_Line[i] < Right_Add_Line[i+2] - 6 && !Right_Add_Start && !Left_Add_Flag[i])	// 本行与前一行右边界有较大突变且没有补线
		    {
		        for (j=Right_Add_Line[i+2];j>Right_Add_Line[i]+1;j--) //j<可以优化
              {
                  if(data[80*(i-2) + j] && !data[80*(i-2) + j-4])
                  {
                        Hazard_Flag=j;
                  }
                  else if (data[80*(i-3) + j] && !data[80*(i-3) + j-4])
                  {
                      Hazard_Flag=j;
                  }
              }
              if(Hazard_Flag && Hazard_Flag - Right_Add_Line[i]> 4) //后面判断可以不要
              {
                  Hazard_Flag = (Right_Add_Line[i] + Hazard_Flag)/2;
                  for (j=i;j>(i-30);j--)  //i-20
                  {
                      if(data[80*j + Hazard_Flag] && data[80*(j-1) + Hazard_Flag] && data[80*(j-2) + Hazard_Flag])
                      {
                        if(data[80*(i-2) + Right_Add_Line[i]-1] && data[80*(i-2) + Right_Add_Line[i]-3] && data[80*(i-2) + Right_Add_Line[i]-5])
                        {
                            Right_Hazard_Flag = i;    // 障碍物标志位置位
                            //save_var(VAR3, Right_Hazard_Flag);
                        }
                      }
                  }
              }
		    }
        }
        /**********************************************************************/

		/**************************** 补线检测开始 ****************************/
        if (Width_Real[i] >= Width_Min+2) 	// 赛道宽度变宽，可能是十字或环路
		{

            if (Left_Add_Flag[i+2])	//前一行补线了
			{
				if (Left_Line[i] < Left_Add_Line[i+2]-1)	//与前一行的左边界实线比较
				{
					Left_Add_Flag[i] = 1;
				}
			}
			else	//前一行没有补线
			{
				if (Left_Line[i] < Left_Line[i+2]-1)	//与前一行的左边界实线比较
				{
					Left_Add_Flag[i] = 1;
				}
			}

			if (Right_Add_Flag[i+2])	//前一行右边界补线了
			{
				if (Right_Line[i] > Right_Add_Line[i+2]+1)	//与前一行的右边界实线进行比较
				{
					Right_Add_Flag[i] = 1;
				}
			}
			else	//前一行右边界没有补线
			{
				if (Right_Line[i] > Right_Line[i+2]+1)		//与前一行的右边界实线进行比较
				{
					Right_Add_Flag[i] = 1;
				}
			}
		}
		/**************************** 补线检测结束 ****************************/



		/*************************** 第一轮补线开始 ***************************/
		if (Left_Add_Flag[i])	// 左侧需要补线
		{
			if (i >= 55)	// 前三行补线不算
			{
				Left_Add_Line[i] = Left_Add_Line[i+2];// 使用前一行左边界直接向上补线，但不记录补线开始行
				Left_Add_Start = 53;	// 强制认定53行为补线开始行
				Left_Ka = 0;
				Left_Kb = Left_Add_Line[i];
			}
			else
			{
				if (!Left_Add_Start)	// 之前没有补线
				{	// 补线开始行最早为53
					Left_Add_Start=i;
                    //使用两点法拟合直线
					Curve_Fitting(&Left_Ka, &Left_Kb, Left_Add_Start+4, Left_Add_Line);
				}
				Left_Add_Line[i] = Calculate_Add(i, Left_Ka, Left_Kb);	// 补线完成
			}
		}
		else
		{
			if (!Left_Add_Stop && Left_Add_Start)	//
			{
				Result = Calculate_Angle(Left_Line[i+4], Left_Line[i+2], Left_Line[i]);	// 突变点检测
				if (Result <= -3)	// -1：135
				{
					Left_Add_Stop = i;	// 记录左侧补线结束行
				}
			}
		}

		if (Right_Add_Flag[i])	// 右侧需要补线
		{
			if (i >= 55)	// 前三行补线不算
			{
				Right_Add_Line[i] = Right_Add_Line[i+2];// 使用前一行右边界直接向上补线，但不记录补线开始行
				Right_Add_Start = 53;	// 强制认定53行为补线开始行
				Right_Ka = 0;
				Right_Kb = Right_Add_Line[i];
			}
			else
			{
				if (!Right_Add_Start)	// 之前没有补线
				{   //记录右侧补线开始行
					Right_Add_Start = i;
                    // 使用两点法拟合直线
					Curve_Fitting(&Right_Ka, &Right_Kb, Right_Add_Start+4, Right_Add_Line);

				}
				Right_Add_Line[i] = Calculate_Add(i, Right_Ka, Right_Kb);	// 补线完成
			}
		}
		else
		{
			if (!Right_Add_Stop && Right_Add_Start)
			{
				Result = Calculate_Angle(Right_Line[i+4], Right_Line[i+2], Right_Line[i]);	// 突变点检测
				if (Result >= 3)	// -1：135
				{
					Right_Add_Stop = i;	// 记录右侧补线结束行
				}
			}
		}
		/*************************** 第一轮补线结束 ***************************/

		Width_Add[i] = Right_Add_Line[i] - Left_Add_Line[i];	// 重新计算赛道宽度
		if ((Left_Add_Flag[59] && Left_Add_Start && !Left_Add_Stop) && (Right_Add_Flag[59] && Right_Add_Start && !Right_Add_Stop))
		{
			Mid_Line[i] = Mid_Line[i+2];
		}
		else
		{
			Mid_Line[i] = (Right_Add_Line[i] + Left_Add_Line[i]) / 2;	// 计算中线
		}
		if (Width_Add[i] < Width_Min)
		{
			Width_Min = Width_Add[i];	// 更新最小赛道宽度
		}

		Line_Count = i;	// 记录成功识别到的赛道行数
                if(Left_Add_Stop&&Right_Add_Stop)    //补线结束后又要补线这不补
                {
                    if(Left_Add_Flag[i]||Right_Add_Flag[i])
                    {
                        Line_Count = i+2;  //break;
                        break;             //Line_Count = i+2;
                    }
                }
	}
	/*************************** 第二轮补线修复开始 ***************************/
	if (Left_Add_Start && Right_Add_Start)	// 左右都需要补线
	{
		if (Left_Add_Stop)	// 有始有终
		{
			Line_Repair(Left_Add_Start, Left_Add_Stop, data, Left_Line, Left_Add_Line, Left_Add_Flag);
		}
		if (Right_Add_Stop)	// 有始有终
		{
			Line_Repair(Right_Add_Start,Right_Add_Stop , data, Right_Line, Right_Add_Line, Right_Add_Flag);
		}
	}
	/*************************** 第二轮补线修复结束 ***************************/

	/****************************** 中线修复开始 ******************************/
	Mid_Line_Repair(Line_Count, data);
	/****************************** 中线修复结束 ******************************/
}

/*
*	中线平均
*
*	说明：不做特殊处理，直接取均值
*/
uchar Point_Average(void)
{
	uchar i, Half_Line;
	int32 Point, Sum = 0;
	static uchar Last_Point = 40;

	if (Out_Side || Line_Count >= 55)	// 出界或者摄像头图像异常
	{
		Point  = Last_Point;			// 使用上次目标点
	}
	else
	{
		Half_Line = 59-(59-Line_Count)/2;	// 计算有效行的中间行
		if (Half_Line%2 == 0)
		{
			Half_Line += 1;
		}
		if (Left_Max + 12 < Right_Min)
		{
			Point = (Left_Max + Right_Min) / 2;
		}
		else
		{
			if (Line_Count <= 21)
			{
				Line_Count = 21;
			}
			for (i = 61; i >= Line_Count; )
			{
				i -= 2;
				Sum += Mid_Line[i];
			}
			Point = Sum / ((61-Line_Count)/2);	// 对中线求平均
			if (Left_Add_Line[Half_Line] >= 20)				// 太偏左
			{
				Point += (Left_Add_Line[Half_Line]-20);	// 适当补偿
			}
			if (Right_Add_Line[Half_Line] <= 60)				// 太偏右
			{
				Point -= (60-Right_Add_Line[Half_Line]);	// 适当补偿
			}
		}

		//Point = Point*0.9 + Last_Point*0.1;	// 低通滤波
                Point = (int)(Point*0.9 + Last_Point*0.1);	// 低通滤波
		Point = range_protect(Point, 1, 79);		// 限幅，防止补偿溢出

		/********************* 障碍物特殊情况处理 ********************/
		if (Left_Hazard_Flag)			//左侧有障碍物且需要补线，即使误判也不会造成影响
		{
			Point = Mid_Line[Left_Hazard_Flag]+6;	//使用障碍物出现的那一行中点作为目标点
			if (Left_Hazard_Flag < 40)
			{
				Point += 3;
			}
		}
		else if (Right_Hazard_Flag)	//右测有障碍物且需要补线，即使误判也不会造成影响
		{
			Point = Mid_Line[Right_Hazard_Flag]-6;//使用障碍物出现的那一行中点作为目标点
			if (Right_Hazard_Flag < 40)
			{
				Point -= 5;
			}
		}
        /************************************************************/
		Last_Point = Point;	// 更新上次目标点
	}

	return Point;
}

/*
*	最小二乘法计算斜率
*
*	说明：返回斜率作为转向控制量,结果放大100倍
*/
/*
int32 Least_Squares(void)
{
	int32 Ave_x, Ave_y, Sum1, Sum2;
	uchar i;
	float f_Slope;
	int32 i_Slope;
	static int32 i_Last_Slope = 0;

	for (i = 61; i >= Line_Count; )
	{
		i -= 2;
	}
	i_Last_Slope = i_Slope;

	return i_Slope;
}
*/
/*
*	斜率计算
*
*	说明：使用斜率作为转向控制量，返回斜率的倒数
*/
float Slope_Weight(uchar *Mid)
{
	float Slope;

	Slope = 1.0 * (Mid[Line_Count] - 40) / (60-Line_Count);

	return Slope;
}

/*
*	赛道角度计算
*
*	说明：返回结果为 右：0：180°，1：135°，2：90°，>= 1可能为直角突变
*					 左：0：180°，-1：135°，-2：90°，<= -1可能为直角突变
*/
char Calculate_Angle(uchar Point_1, uchar Point_2, uchar Point_3)
{
	char K1, K2,Result;

	K1 = Point_2 - Point_1;
	K2 = Point_3 - Point_2;

	Result = (K2 - K1);

	return Result;
}

/*
*	两点法求直线
*
*	说明：拟合直线 y = Ka * x + Kb
*/
void Curve_Fitting(float *Ka, float *Kb, uchar Start, uchar *Line)
{
	*Ka = 1.0*(Line[Start+4] - Line[Start]) / 4;
	*Kb = 1.0*Line[Start] - (*Ka * Start);
}

/*
*	两点法求直线
*
*	说明：拟合直线 y = Ka * x + Kb   Mode == 1代表左边界，Mode == 2代表右边界
*/
void Curve2_Fitting(float *Ka, float *Kb, uchar Start,uchar End, uchar *Line, uchar Mode)
{
        if (Mode==1)
        {
             *Ka = 1.0*((Line[Start]+20) - Line[End]) / (Start-End);
             *Kb = 1.0*Line[End] - (*Ka * End);
        }
        else
        {
              *Ka = 1.0*((Line[Start]-20) - Line[End]) / (Start-End);
              *Kb = 1.0*Line[End] - (*Ka * End);
        }

}

/*
*	计算补线坐标
*
*	说明：使用两点法计算拟合出的补线坐标
*/
uchar Calculate_Add(uchar i, float Ka, float Kb)	// 计算补线坐标
{
	float res;
	uchar Result;

	res = i * Ka + Kb;
	Result = range_protect((int32)res, 1, 79);

	return Result;
}

/*
*	尽头搜索
*
*	说明：从某一点开始竖直向上搜索，返回最远行坐标
*/
uchar Limit_Scan(uchar i, uchar *data, uchar Point)
{
	for ( ; i >= 20; i--)
	{
		if (!data[80*i + Point])	// 搜索到黑点
		{
			break;
		}
	}

	return i;	// 返回最远行坐标
}

/*
*	第一行特殊处理
*
*	说明：先使用第60行中点作为第59行(第一行)搜线起始位置，成功搜索到左右边界后
 	将第59行中点赋值给第60行便于下一帧图像使用。如果第60行中点在本行为黑点，再
 	分别使用左遍历和右遍历的方法搜索边界，以赛道宽度较大的结果作为第59行边界，
 	若仍然搜索不到边界或数据异常认为出界，出界返回0
*/
uchar First_Line_Handle(uchar *data)
{
	uchar i;	// 控制行
	uchar Weight_Left, Weight_Right;	// 左右赛道宽度
	uchar Mid_Left, Mid_Right;
	Image_Para_Init();  //图像算法参数初始化
	i = 59;

	if (!data[i*80 + Mid_Line[61]])	// 第61行中点在第59行为黑点
	{
		Weight_Left = Traversal_Left(i, data, &Mid_Left, 1, 79);	// 从左侧搜索边界
		Weight_Right = Traversal_Right(i, data, &Mid_Right, 1, 79); // 从右侧搜索边界
		if (Weight_Left >= Weight_Right && Weight_Left)	// 左赛道宽度大于右赛道宽度且不为0
		{
			Traversal_Left_Line(i, data, Left_Line, Right_Line);	// 使用左遍历获取赛道边界
		}
		else if (Weight_Left < Weight_Right && Weight_Right)
		{
			Traversal_Right_Line(i, data, Left_Line, Right_Line);	// 使用右遍历获取赛道边界
		}
		else	// 说明没查到
		{
			return 0;
		}
	}
	else
	{
		Traversal_Mid_Line(i, data, Mid_Line[i+2], 1, 79, Left_Line, Right_Line, Left_Add_Line, Right_Add_Line);	// 从前一行中点向两边扫描
	}

	Left_Line[61] = Left_Line[59];
	Right_Line[61] = Right_Line[59];
	Left_Add_Line[61] = Left_Add_Line[59];
	Right_Add_Line[61] = Right_Add_Line[59];

	if (Left_Add_Flag[59] && Right_Add_Flag[59])
	{
		Mid_Line[59] = Mid_Line[61];
	}
	else
	{
		Mid_Line[59] = (Right_Line[59] + Left_Line[59]) / 2;
		Mid_Line[61] = Mid_Line[59];	// 更新第60行虚拟中点，便于下一帧图像使用
	}
	Width_Real[61] = Width_Real[59];
	Width_Add[61] = Width_Add[59];
	Width_Min = Width_Add[59];

	return 1;
}

/*
*	从左侧开始搜索边界，返回赛道宽度
*
*	说明：本函数仅仅作为探测赛道使用，仅返回赛道宽度，不保存边界数据
*/
uchar Traversal_Left(uchar i, uchar *data, uchar *Mid, uchar Left_Min, uchar Right_Max)
{
	uchar j, White_Flag = 0;
	uchar Left_Line = Left_Min, Right_Line = Right_Max;

	for (j = Left_Min; j <= Right_Max; j++)	// 边界坐标 1到79
	{
		if (!White_Flag)	// 先查找左边界
		{
			if (data[i*80 + j])	// 检测到白点
			{
				Left_Line = j;	// 记录当前j值为本行左边界
				White_Flag = 1;	// 左边界已找到，必有右边界

				continue;	// 结束本次循环，进入下次循环
			}
		}
		else
		{
			if (!data[i*80 + j])//检测黑点
			{
				Right_Line = j-1;//记录当前j值为本行右边界

				break;	// 左右边界都找到，结束循环
			}
		}
	}

	if (!White_Flag)	// 未找到左右边界
	{
		return 0;
	}
	else
	{
		*Mid = (Right_Line + Left_Line) / 2;

		return (Right_Line - Left_Line);
	}
}

/*
*	从右侧开始搜索边界，返回赛道宽度
*
*	说明：本函数仅仅作为探测赛道使用，仅返回赛道宽度，不保存边界数据
*/
uchar Traversal_Right(uchar i, uchar *data, uchar *Mid, uchar Left_Min, uchar Right_Max)
{
	uchar j, White_Flag = 0;
	uchar Left_Line = Left_Min, Right_Line = Right_Max;

	for (j = Right_Max; j >= Left_Min; j--)	// 边界坐标 1到79
	{
		if (!White_Flag)	// 先查找右边界
		{
			if (data[i*80 + j])	// 检测到白点
			{
				Right_Line = j;	// 记录当前j值为本行右边界
				White_Flag = 1;	// 右边界已找到，必有左边界

				continue;	// 结束本次循环，进入下次循环
			}
		}
		else
		{
			if (!data[i*80 + j])	//检测黑点
			{
				Left_Line = j+1;	//记录当前j值为本行左边界

				break;	// 左右边界都找到，结束循环
			}
		}
	}

	if (!White_Flag)	// 未找到左右边界
	{
		return 0;
	}
	else
	{
		*Mid = (Right_Line + Left_Line) / 2;

		return (Right_Line - Left_Line);
	}
}

/*
*	从左侧开始搜索边界，保存赛道边界，返回1成功 0失败
*
*	说明：本函数使用后将保存边界数据
*/
uchar Traversal_Left_Line(uchar i, uchar *data, uchar *Left_Line, uchar *Right_Line)
{
	uchar j, White_Flag = 0;

	Left_Line[i] = 1;
	Right_Line[i] = 79;

	for (j = 1; j < 80; j++)	// 边界坐标 1到79
	{
		if (!White_Flag)	// 先查找左边界
		{
			if (data[i*80 + j])	// 检测到白点
			{
				Left_Line[i] = j;	// 记录当前j值为本行左边界
				White_Flag = 1;		// 左边界已找到，必有右边界

				continue;	// 结束本次循环，进入下次循环
			}
		}
		else
		{
			if (!data[i*80 + j])	//检测黑点
			{
				Right_Line[i] = j-1;//记录当前j值为本行右边界

				break;	// 左右边界都找到，结束循环
			}
		}
	}
	if (White_Flag)
	{
		Left_Add_Line[i] = Left_Line[i];
		Right_Add_Line[i] = Right_Line[i];
		Width_Real[i] = Right_Line[i] - Left_Line[i];
		Width_Add[i] = Width_Real[i];
	}

	return White_Flag;	// 返回搜索结果
}

/*
*	从右侧开始搜索边界，保存赛道边界，返回1成功 0失败
*
*	说明：本函数使用后将保存边界数据
*/
uchar Traversal_Right_Line(uchar i, uchar *data, uchar *Left_Line, uchar *Right_Line)
{
	uchar j, White_Flag = 0;

	Left_Line[i] = 1;
	Right_Line[i] = 79;

	for (j = 79; j > 0; j--)	// 边界坐标 1到79
	{
		if (!White_Flag)	// 先查找右边界
		{
			if (data[i*80 + j])	// 检测到白点
			{
				Right_Line[i] = j;	// 记录当前j值为本行右边界
				White_Flag = 1;		// 右边界已找到，必有左边界

				continue;	// 结束本次循环，进入下次循环
			}
		}
		else
		{
			if (!data[i*80 + j])	//检测黑点
			{
				Left_Line[i] = j+1;//记录当前j值为本行左边界

				break;	// 左右边界都找到，结束循环
			}
		}
	}
	if (White_Flag)
	{
		Left_Add_Line[i] = Left_Line[i];
		Right_Add_Line[i] = Right_Line[i];
		Width_Real[i] = Right_Line[i] - Left_Line[i];
		Width_Add[i] = Width_Real[i];
	}

	return White_Flag;	// 返回搜索结果
}

/*
*	从中间向两边搜索边界
*
*	说明：本函数使用后将保存边界数据
*/
void Traversal_Mid_Line(uchar i, uchar *data, uchar Mid, uchar Left_Min, uchar Right_Max, uchar *Left_Line, uchar *Right_Line, uchar *Left_Add_Line, uchar *Right_Add_Line)
{
	uchar j;

	Left_Add_Flag[i] = 1;	// 初始化补线标志位
	Right_Add_Flag[i] = 1;

////	Left_Min = range_protect(Left_Min, 1, 79);	// 限幅，防止出错
//	if (Left_Add_Flag[i+2])
//	{
//		Left_Min = range_protect(Left_Add_Line[i+2]-10, 1, 79);
//	}
////	Right_Max = range_protect(Right_Max, 1, 79);
//	if (Right_Add_Flag[i+2])
//	{
//		Right_Max = range_protect(Right_Add_Line[i+2]+10, 1, 79);
//	}

	Right_Line[i] = Right_Max;
	Left_Line[i] = Left_Min;	// 给定边界初始值

	for (j = Mid; j >= Left_Min+1; j--)	// 以前一行中点为起点向左查找边界
	{
		if (!data[i*80 + j]&&!data[i*80 + j-1])	// 检测到黑点
		{
			Left_Add_Flag[i] = 0;	//左边界不需要补线，清除标志位
			Left_Line[i] = j;	    //记录当前j值为本行实际左边界
			Left_Add_Line[i] = j;	// 记录实际左边界为补线左边界

			break;
		}
	}
	for (j = Mid; j <= Right_Max-1; j++)	// 以前一行中点为起点向右查找右边界
	{
		if (!data[i*80 + j]&&!data[i*80 + j+1])	//检测到黑点
		{
			Right_Add_Flag[i] = 0;  //右边界不需要补线，清除标志位
			Right_Line[i] = j;	    //记录当前j值为本行右边界
			Right_Add_Line[i] = j;	// 记录实际右边界为补线左边界

			break;
		}
	}
	if (Left_Add_Flag[i])	// 左边界需要补线
	{
		if (i >= 55)	// 前6行
		{
			Left_Add_Line[i] = Left_Line[59];	// 使用底行数据
		}
		else                     ///
		{
			Left_Add_Line[i] = Left_Add_Line[i+2];	// 使用前2行左边界作为本行左边界
		}
	}
	if (Right_Add_Flag[i])	// 右边界需要补线
	{
		if (i >= 55)	// 前6行
		{
			Right_Add_Line[i] = Right_Line[59];	// 使用底行数据
		}
		else         //////
		{
			Right_Add_Line[i] = Right_Add_Line[i+2];	// 使用前2行右边界作为本行右边界
		}
	}
	Width_Real[i] = Right_Line[i] - Left_Line[i];			// 计算实际赛道宽度
	Width_Add[i] = Right_Add_Line[i] - Left_Add_Line[i];	// 计算补线赛道宽度

}

/*
*	补线修复
*
*	说明：有始有终才使用，直接使用两点斜率进行补线
*/
void Line_Repair(uchar Start, uchar Stop, uchar *data, uchar *Line, uchar *Line_Add, uchar *Add_Flag)
{
	float res;
	uchar i;	// 控制行
	float Ka, Kb;

	if (Start >= 55)
	{
		Start = 59;
	}
	else
	{
		Start += 4;            ////
	}

	if (Add_Flag[59] && Line_Count+4 <= Stop)	// 从上向下补线
	{
		Start = 59;
		Ka = 1.0*(Line_Add[Stop-4] - Line_Add[Stop]) / (Stop-4 - Stop);
		Kb = 1.0*Line_Add[Stop] - (Ka * Stop);
	}
	else
	{
		Ka = 1.0*(Line_Add[Start] - Line_Add[Stop]) / (Start - Stop);
		Kb = 1.0*Line_Add[Start] - (Ka * Start);
	}

	for (i = Stop; i <= Start; )
	{
		i += 2;
		res = i * Ka + Kb;
		Line_Add[i] = range_protect((int32)res, 1, 79);
	}
}

/*
*	中线修复
*
*	说明：普通弯道丢线使用平移赛道方式，中点到达边界结束
*/
void Mid_Line_Repair(uchar count, uchar *data)
{
	char res;
	uchar i,j;  //Left_Hazard,Right_Hazard;	// 控制行
	//float Ka, Kb;

    //Left_Hazard = 0;
    //Right_Hazard = 0;

	Left_Max = Left_Add_Line[59];
	Right_Min = Right_Add_Line[59];

	for (i = 61; i >= count+2; )
	{
		i -= 2;
		if (Left_Add_Flag[i] && Right_Add_Flag[i])	// 左右都需要补线
		{
			Mid_Line[i] = (Right_Add_Line[i] + Left_Add_Line[i]) / 2;	// 计算赛道中点
			Width_Add[i] = Right_Add_Line[i] - Left_Add_Line[i];		// 计算赛道宽度
		}
		else if (Left_Add_Flag[i])	// 仅左侧需要补线
		{
			if (Left_Add_Line[i] == 1 && !Left_Add_Stop)	// 补到极限了，不具备参考价值
			{
                      //平移赛道宽度一半
			res = Right_Add_Line[i] - Width_Add[i+2] / 2 + (Right_Add_Line[i+2] - Right_Add_Line[i]) / 2;
				Width_Add[i] = 2 * (Right_Add_Line[i] - res);	// 根据右边界和中线推算赛道宽度
				if (res <= 0)	// 中点已到达图像极限，不再具备参考价值
				{
					i += 2;	// 记录成功识别到的中点行数
					break;
				}
			}
			else
			{
				res = (Right_Add_Line[i] + Left_Add_Line[i]) / 2;		// 计算赛道中点
				Width_Add[i] = Right_Add_Line[i] - Left_Add_Line[i];	// 计算赛道宽度
			}
			Mid_Line[i] = res;	// 更新中点
		}
		else if (Right_Add_Flag[i])	// 仅右侧需要补线
		{
			if (Right_Add_Line[i] == 79 && !Right_Add_Stop)	// 补到极限了，不具备参考价值
			{
				res = Left_Add_Line[i] + Width_Add[i+2] / 2 - (Left_Add_Line[i]-Left_Add_Line[i+2]) / 2;	// 平移赛道宽度一半
				Width_Add[i] = 2 * (res - Left_Add_Line[i]);	// 根据中线和左边界推算赛道宽度
				if (res >= 80)	// 中点已到达图像极限，不再具备参考价值
				{
					i += 2;	// 记录成功识别到的中点行数
					break;
				}
			}
			else
			{
				res = (Right_Add_Line[i] + Left_Add_Line[i]) / 2;		// 计算赛道中点
				Width_Add[i] = Right_Add_Line[i] - Left_Add_Line[i];	// 计算赛道宽度
			}
			Mid_Line[i] = res;	// 更新中点
		}

        //障碍处理
        if(Left_Hazard_Flag)
        {
            Curve2_Fitting(&Left_Ka, &Left_Kb, 59,Left_Hazard_Flag, Left_Line, 1);
            for (j=59;j>=Left_Hazard_Flag;j--)
            {
                Left_Add_Line[j] = Calculate_Add(j, Left_Ka,Left_Kb);	// 补线完成
                Mid_Line[j] = (Right_Add_Line[j] + Left_Add_Line[j]) / 2;
            }
        }
        //BuzzerTest(Left_Hazard_Flag);
        if(Right_Hazard_Flag)
        {
            Curve2_Fitting(&Right_Ka, &Right_Kb, 59,Right_Hazard_Flag, Right_Line, 2);
            for (j=59;j>=Right_Hazard_Flag;j--)
            {
                Right_Add_Line[j] = Calculate_Add(j, Right_Ka, Right_Kb);	// 补线完成
                Mid_Line[j] = (Right_Add_Line[j] + Left_Add_Line[j]) / 2;
            }

        }
        //BuzzerTest(Right_Hazard_Flag);
		if (Left_Add_Line[i] > Left_Max)
		{
			Left_Max = Left_Add_Line[i];
		}
		if (Right_Add_Line[i] < Right_Min)
		{
			Right_Min = Right_Add_Line[i];
		}

#if   Display_OFF_ON
	/**//*************************** 上位机显示边界 ***************************/
	/**/data[i*80 + Left_Add_Line[i] + 2] = 0;	// 上位机显示补线后的左边界，不用时屏蔽
	/**/data[i*80 + Right_Add_Line[i] - 2] = 0;	// 上位机显示补线后的右边界，不用时屏蔽
	/**/data[i*80 + Mid_Line[i]] = 0;			// 上位机显示中线，不用时屏蔽
//	/**/data[i*80 + Left_Line[i] + 2] = 0;		// 上位机显示原始左边界，不用时屏蔽
//	/**/data[i*80 + Right_Line[i] - 2] = 0;		// 上位机显示原始右边界，不用时屏蔽
	/**//*************************** 上位机显示边界 ***************************/
#endif
	}
	Mid_Line[61] = Mid_Line[59];
	Line_Count = i;		// 记录成功识别到的中点行数

	if (Left_Add_Stop)
	{
		Curve_Fitting(&Left_Ka, &Left_Kb, Left_Add_Stop, Left_Add_Line);
	}
	if (Right_Add_Stop)
	{
		Curve_Fitting(&Right_Ka, &Right_Kb, Right_Add_Stop, Right_Add_Line);
	}
}

/****************** 新算法 ******************/

/*
*	加权平均
*
*	说明：权重是乱凑的，效果不好
*/
uchar Point_Weight(void)
{
	uchar i ,Point, Point_Mid;

	int32 Sum = 0, Weight_Count = 0;

	if (Out_Side || Line_Count >= 57)	//出界或者摄像头图像异常
	{
		Point  = Last_Point;
               // Speed=0;
                if(Annulus_Check)
                {
                   Stop_Flag = 1;
                }
	}
    if(Starting_Line_Flag)   //斑马线识别后置1
    {
        Point  = Last_Point;
        // Speed=0;
        if(Annulus_Check)    //起跑后延时50ms置1
        {
            Stop_Flag = 1;   //再次检测则停车
        }
    }
	else
	{
          	if (Line_Count <= 20)  //20
                {
                  Line_Count = 21; //21
                }
		for (i = 61; i >= Line_Count+2; )		//使用加权平均
		{
			i -= 2;
			Sum += Mid_Line[i] * Weight[59-i];
			Weight_Count += Weight[59-i];
		}
		Point = range_protect(Sum / Weight_Count, 2, 78);
		Point = range_protect(Point, 2, 78);
		Last_Point = Point;

							/***** 使用最远行数据和目标点作为前瞻 *****/
//		if (Line_Count >= 27)
//		{
//			Point_Mid = Mid_Line[31];
//		}
//		else
//		{
//			Point_Mid = Mid_Line[Line_Count+2];
//		}
                Point_Mid = Mid_Line[Line_Count+2];
                //Ek_compar=(Mid_Line[Line_Count]+Mid_Line[Line_Count+8])>>1;
                //Ek_compar=Mid_Line[Line_Count+4]-Ek_compar;
	}
//        if (Line_Count <= 30)
//        {
//            Foresight = 0.8 * Error_Transform(Point_Mid, 40)	//使用最远行偏差和加权偏差确定前瞻
//			  + 0.2 * Error_Transform(Point,  40);//+0.1*Ek_compar;
//        }
//	else
//        {
            //Foresight = 0.8 * Error_Transform(Point_Mid, 40)	//使用最远行偏差和加权偏差确定前瞻
			//  + 0.2 * Error_Transform(Point,  40);//+0.1*Ek_compar;
            Foresight = (int)(0.8 * Error_Transform(Point_Mid, 40)	//使用最远行偏差和加权偏差确定前瞻
			  + 0.2 * Error_Transform(Point,  40));//+0.1*Ek_compar;
//        }

	return Point;
}

char Error_Transform(uchar Data, uchar Set)   //求绝对值
{
	char Error;

	Error = Set - Data;
	if (Error <= 0)
	{
		Error = -Error;
	}

	return Error;
}
int32 abs_int(int32 i)//作用： 求绝对值  int32型
{
     int32 B;
      B=i;
     if(B<0) return (-B);
     else    return B;
}
/*************函数功能********************
          滤波采用中值滤波

          可优化
*************3月13号  验证通过 ***********/
uint8  MiddleFilter(uint8 a,uint8 b,uint8 c)
{
  uint8 temp;
  if(a>b)
  {
    temp=b;
    b=a;
    a=temp;
  }
  if(b>c)
  {
    temp=c;
    c=b;
    b=temp;
  }
  if(a>b)
  {
    temp=b;
    b=a;
    a=temp;
  }
  return b;
}





/*!
*  @brief      二值化图像解压（空间 换 时间 解压）
*  @param      dst             图像解压目的地址
*  @param      src             图像解压源地址
*  @param      srclen          二值化图像的占用空间大小
*  @since      v5.0            img_extract(img, imgbuff,CAMERA_SIZE);
*  Sample usage:
*/
//void img_extract(uint8 *dst, uint8 *src, uint32 srclen)
//{
//  uint8 tmpsrc;
//  while(srclen --)
//  {
//    tmpsrc = *src++;
//    *dst++ = colour[ (tmpsrc >> 7 ) & 0x01 ];
//    *dst++ = colour[ (tmpsrc >> 6 ) & 0x01 ];
//    *dst++ = colour[ (tmpsrc >> 5 ) & 0x01 ];
//    *dst++ = colour[ (tmpsrc >> 4 ) & 0x01 ];
//    *dst++ = colour[ (tmpsrc >> 3 ) & 0x01 ];
//    *dst++ = colour[ (tmpsrc >> 2 ) & 0x01 ];
//    *dst++ = colour[ (tmpsrc >> 1 ) & 0x01 ];
//    *dst++ = colour[ (tmpsrc >> 0 ) & 0x01 ];
//  }
//}

/*!
*  @brief      二值化图像压缩（空间 换 时间 压缩）
*  @param      dst             图像压缩目的地址
*  @param      src             图像压缩源地址
*  @param      srclen          二值化图像的占用空间大小
*  @since      v5.0            img_extract(img, imgbuff,CAMERA_SIZE);
*  Sample usage:
*/
void img_recontract(uint8 *dst, uint8 *src, uint32 srclen)
{
  uint8 tmpsrc;

  while(srclen --)
  {
    tmpsrc=0;
    if(*dst++ == colour[black])
    {
      tmpsrc = tmpsrc + 0x80;
    }

    if(*dst++ == colour[black])
    {
      tmpsrc = tmpsrc + 0x40;
    }

    if(*dst++ == colour[black])
    {
      tmpsrc = tmpsrc + 0x20;
    }

    if(*dst++ == colour[black])
    {
      tmpsrc = tmpsrc + 0x10;
    }

    if(*dst++ == colour[black])
    {
      tmpsrc = tmpsrc + 0x08;
    }

    if(*dst++ == colour[black])
    {
      tmpsrc = tmpsrc + 0x04;
    }

    if(*dst++ == colour[black])
    {
      tmpsrc = tmpsrc + 0x02;
    }

    if(*dst++ == colour[black])
    {
      tmpsrc = tmpsrc + 0x01;
    }

    *src++ = tmpsrc;
  }
}

/*!
*  @brief      取边界线
*  @param      dst             图像压缩目的地址
*  @param      src             图像压缩源地址
*  @param      srclen          二值化图像的占用空间大小
*  @since      v5.0            img_extract(img, imgbuff,CAMERA_SIZE);
*  Sample usage:
*/
void img_getline(uint8 *dst, uint8 *src, uint32 srclen)
{
  uint8 tmpsrc,buff_y,buff_x;

  while(srclen --)
  {
    tmpsrc=0;
    buff_x = *(dst + 80);
    buff_y = *dst++;
    *(dst-1)=colour[white];
    if(*dst != buff_y || (buff_y != buff_x && srclen > 9))
    {
      tmpsrc = tmpsrc + 0x80;
      *(dst-1)=colour[black];
    }

    buff_x = *(dst + 80);
    buff_y = *dst++;
    *(dst-1)=colour[white];
    if(*dst != buff_y || (buff_y != buff_x && srclen > 9))
    {
      tmpsrc = tmpsrc + 0x40;
      *(dst-1)=colour[black];
    }

    buff_x = *(dst + 80);
    buff_y = *dst++;
    *(dst-1)=colour[white];
    if(*dst != buff_y || (buff_y != buff_x && srclen > 9))
    {
      tmpsrc = tmpsrc + 0x20;
      *(dst-1)=colour[black];
    }

    buff_x = *(dst + 80);
    buff_y = *dst++;
    *(dst-1)=colour[white];
    if(*dst != buff_y || (buff_y != buff_x && srclen > 9))
    {
      tmpsrc = tmpsrc + 0x10;
      *(dst-1)=colour[black];
    }

    buff_x = *(dst + 80);
    buff_y = *dst++;
    *(dst-1)=colour[white];
    if(*dst != buff_y || (buff_y != buff_x && srclen > 9))
    {
      tmpsrc = tmpsrc + 0x08;
      *(dst-1)=colour[black];
    }

    buff_x = *(dst + 80);
    buff_y = *dst++;
    *(dst-1)=colour[white];
    if(*dst != buff_y || (buff_y != buff_x && srclen > 9))
    {
      tmpsrc = tmpsrc + 0x04;
      *(dst-1)=colour[black];
    }

    buff_x = *(dst + 80);
    buff_y = *dst++;
    *(dst-1)=colour[white];
    if(*dst != buff_y || (buff_y != buff_x && srclen > 9))
    {
      tmpsrc = tmpsrc + 0x02;
      *(dst-1)=colour[black];
    }

    buff_x = *(dst + 80);
    buff_y = *dst++;
    *(dst-1)=colour[white];
    if((*dst != buff_y && srclen % 10 !=0 ) || (buff_y != buff_x && srclen > 9))
    {
      tmpsrc = tmpsrc + 0x01;
      *(dst-1)=colour[black];
    }

    *src++ = tmpsrc;
  }
}

void image_processing(uchar *data)
{
    if (Annulus1_Flag==0&&Annulus2_Flag==1)
    {
      Annulus2_Handle(data);
    }
    else if(Annulus1_Flag==1&&Annulus2_Flag==0)
    {
      Annulus1_Handle(data);
    }
    else
      Image_Handle(data);
      

}

