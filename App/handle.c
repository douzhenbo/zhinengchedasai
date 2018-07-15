#include "Handle.h"

//��·�ļ�⣬�����Ƿ��ʼʱ���ұ߽�ĸ�ֵ����ֵ���Ƿ���ԣ���Ҫ��֤�������Ǻ�


#define Display_OFF_ON   1    //������ʾ���أ�1Ϊ�� 0Ϊ��

uint8 colour[2] = {255, 0},black=1,white=0; //0 �� 1 �ֱ��Ӧ����ɫ
//ע��ɽ�������ͷ 0 ��ʾ ��ɫ��1��ʾ ��ɫ
////////////////////0       2       4       6       8

        //Բ�������������
uchar Width_Max[61]={10,11,12,13,14,15,16,17,18,19,
                     20,21,22,23,24,25,26,27,28,29,
                     30,31,32,33,34,35,36,37,38,39,
                     40,41,42,43,44,45,46,47,48,49,
                     50,51,52,53,54,55,56,57,58,59,
                     60,61,62,63,64,65,66,67,68,69,70};

//��Ȩƽ������

uchar Weight[60]={  6  ,6  ,6  ,6  ,6  ,6 ,6 ,6 ,6 ,6 , //���ʮ��//��ѹ�ߣ��������666666666666666666
                    5  ,5  ,15  ,15 ,15 ,15 ,15 ,15 ,10 ,10 , //40-49��  49�೵15cm
                    10  ,10  ,10  ,10  ,5  ,5  ,5  ,5  ,5  ,5  ,//30-39��  39�೵15+9=24  30�೵24+12=36
                    5 ,5 ,5 ,5 ,5  ,5  ,5  ,5  ,5  ,5  , //20-29��  20�೵15+46=61
                    1  ,1  ,1  ,1  ,1  ,1 ,1 ,1 ,1  ,1  , //0-19�У� 10�೵61+50=111      1111111111111111111111
                    1  ,1  ,1  ,1  ,1  ,1  ,1  ,1  ,1  ,1 }; //0-9�У������ò���     11111111111111111

uint8 i;
uchar Left_Line[62], Right_Line[62], Mid_Line[62];	// ԭʼ���ұ߽�����
uchar Left_Add_Line[62], Right_Add_Line[62];		// ���ұ߽粹������
uchar Left_Add_Flag[62], Right_Add_Flag[62];		// ���ұ߽粹�߱�־λ
uchar Width_Real[62];	// ʵ���������
uchar Width_Add[62];	// �����������
uchar Width_Min;		// ��С�������

uchar Ek_compar;    // �����м�ֵ�����е����е�Mid_Line�Ĳ�ֵ
uchar Foresight;	// ǰհ�������ٶȿ���
uchar Out_Side = 0;	// ���߿���
uchar Line_Count;	// ��¼�ɹ�ʶ�𵽵���������

uchar Left_Add_Start, Right_Add_Start;	// ���Ҳ�����ʼ������
uchar Left_Add_Stop, Right_Add_Stop;	// ���Ҳ��߽���������
float Left_Ka = 0, Right_Ka = 0;
float Left_Kb = 1, Right_Kb = 79;	// ��С���˷�����

uchar Left_Hazard_Flag, Right_Hazard_Flag, Hazard_Flag;	// �����ϰ����־
uint8 Hazard_Time;
uchar Left_Max, Right_Min;

uchar Starting_Line_Flag = 0;	// �����߱�־λ
uchar Starting_Line_Times = 0;	// �����߱�־λ
uchar Starting_Line_Time = 0;	// �����߱�־λ
char Last_Point = 40;

/************** ��·��ر��� *************/
uchar Annulus1_Flag=0;
uchar Annulus2_Flag=0;
uchar Annulus_Mid=0;
uchar Annulus_Left;
uchar Annulus_Right;
uchar Annulus_Count[4] = {0, 0, 0, 0};	         // 0�һ�·��1��·
uchar Annulus_Times = 0;	// ��·����
uchar Annulus2_Delay = 0;
uint16 Annulus_DelayMs = 0;	// ��⵽��·����ʱһ��ʱ����Զ������·��־λ
uint8 first_signal=0;
uint8 second_signal=0;
uint8 third_signal=0;
uint8 fourth_signal=0;
uint8 fifth_signal=0;
uint8 sixth_signal=0;
uchar Annulus_Mode = 0;
uint8 seventh_signal=0;
extern int32 S3010_Duty;
/************** ��·��ر��� *************/


void Annulus2_Control(void)
{
	Annulus_Right = Annulus_Count[Annulus_Times++];
        Annulus_Left = !Annulus_Right;

        Annulus_Mode = 1;	// ��ʼ���뻷·
        Annulus2_Flag = 1;	// ������־λ ��1
        //first_signal=1;

        //led(LED_MAX,LED_ON);// ��LEDָʾ��		ȫ����ʼ���ɹ�

        if (Annulus_Times >= 4)
        {
                Annulus_Times = 0;
        }
}
void Annulus1_Control(void)
{
	Annulus_Right = Annulus_Count[Annulus_Times++];
        Annulus_Left = !Annulus_Right;

        Annulus_Mode = 1;	// ��ʼ���뻷·
        Annulus1_Flag = 1;	// ������־λ ��1
        //first_signal=1;

        //led(LED_MAX,LED_ON);// ��LEDָʾ��		ȫ����ʼ���ɹ�

        if (Annulus_Times >= 4)
        {
                Annulus_Times = 0;
        }
}

/*
*	��·ͼ�����㷨
*
*	˵������·ר�ã���������Ԫ��
*/
void Annulus2_Handle(uchar *data)
{
	uchar i;	// ������
	uchar res;

	Line_Count = 0;	// ����������λ

	Left_Add_Start = 0;		// ��λ������ʼ������
	Right_Add_Start = 0;
	Left_Add_Stop = 0;
	Right_Add_Stop = 0;

	/***************************** ��һ�����⴦�� *****************************/
	res = First_Line_Handle(data);
	if (res == 0)
	{
		Out_Side++;	//����
		return;
	}
	if (Left_Add_Start || Right_Add_Start)
	{
		Width_Min = 60;
	}
	/*************************** ��һ�����⴦����� ***************************/

	for (i = 59; i >= 15;)   //15
	{
		i -= 2;

		if (!data[i*80 + Mid_Line[i+2]])//ǰ2���е��ڱ���Ϊ�ڵ㣬��������
		{
			i += 1;
			break;
		}
		else
		{
			Traversal_Mid_Line(i, data, Mid_Line[i+2], 1, 79, Left_Line, Right_Line, Left_Add_Line, Right_Add_Line);
			/************************** ���߼�⿪ʼ **************************/
//			if (Width_Real[i] >= Width_Min)	// ������ȱ��,�����󻷵�
//			{
                        
                               if(i>=55 && Width_Real[i] >= Width_Min && Right_Add_Flag[i]==1&&Left_Add_Flag[i]==0&&fourth_signal==0)
                              {       
                                      
                                     first_signal=1;
                                     if (first_signal==1)	//��һ����־λ  ��ֱ��
                                     {
                                       Left_Add_Line[i] = Left_Line[i];                           
                                       Right_Add_Line[i]=range_protect((Left_Add_Line[i] + Width_Max[i]-7),0,79);
                                     }
                                
                                     
                             }
                             if( (i>=43&& i<=55)&& first_signal==1&& Right_Add_Flag[i]==0&&(Width_Real[i]-Width_Real[i-2]>0) && (Width_Real[i-4]-Width_Real[i-6]<0)&&second_signal==0&&fourth_signal==0)//(Width_Real[i] >= Width_MinWidth_Real[i]-Width_Real[i-2]>0) && (Width_Real[i-4]-Width_Real[i-6]<0)
                             {
                                           //��⵽�м��Բ��Ϊ�ڶ�����־λ�������̵�
                                second_signal=1;
                                
                             }
                                  
                             if( Width_Real[i-2] >= Width_Min && second_signal==1&&fourth_signal==0)//Width_Real[i-2] >= Width_Min && second_signal==1&&fourth_signal==0
                             {
                                 third_signal=1;
                                 first_signal=0;
                                 if(second_signal==1)				// �ڶ�����־λ  ����
                                 {
                                   
                                   
                                   S3010[0][0] = 400;
                                   Right_Add_Line[i] = Right_Line[i];
                                   Left_Add_Line[i]=range_protect((Right_Line[i] - Width_Max[i]+3 ),0,79);
                                   if(third_signal==1&&Right_Add_Flag[i]==0&&Left_Add_Flag[i]==0&&Left_Add_Flag[i-2]==0&&Width_Real[i] <= Width_Min&&Width_Real[i-2] <= Width_Min&&Right_Add_Flag[i-2]==0)
                                   {
                                     S3010[0][0] = 258;
                                     sixth_signal=1;//�����Ż�
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
                                   tpm_pwm_init(S3010_TPM, S3010_CH,S3010_HZ,33800);                       //��������־λ������
                                   DELAY_MS(300);
                                 }
                                
                                
                                fourth_signal=1;
                                
                                                                                                                            //��������־λ������
                             }
                             if(fourth_signal==1&&i>=55&&Left_Add_Flag[i]==0 && Right_Add_Flag[i]==1&&Width_Real[i]>=Width_Min)//fourth_signal==1&&i>=56&&Width_Real[i]>=Width_Min&&Right_Add_Flag[i]==0&&Left_Add_Flag==0
                             {
                                gpio_init(PTE24,GPO,0);
                                fifth_signal=1;                                                                                //�������־λ��������
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
                               sixth_signal=0;//��������־λ�����־λ
                               Annulus2_Flag = 0;
                               gpio_init(PTE24,GPO,1);
                               S3010[0][0] = 258;
                               S3010[0][2] = 360;
                             }
                              

			/************************** ���߼����� **************************/
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
		/**//************************* OLED��ʾ�߽� *************************/
		/**/data[i*80 + Left_Add_Line[i] + 2] = 0;	// ��ʾ���ߺ����߽磬����ʱ����
		/**/data[i*80 + Right_Add_Line[i] - 2] = 0;	// ��ʾ���ߺ���ұ߽磬����ʱ����
		/**/data[i*80 + Mid_Line[i]] = 0;			// ��ʾ���ߣ�����ʱ����
//		/**/data[i*80 + Left_Line[i] + 2] = 0;		// ��ʾԭʼ��߽磬����ʱ����
//		/**/data[i*80 + Right_Line[i] - 2] = 0;		// ��ʾԭʼ�ұ߽磬����ʱ����
		/**//************************* OLED��ʾ�߽� *************************/
	}
#endif

        //Annulus_Flag = 0;	// �����·״̬��־λ
	/************************** ��·����״̬�жϽ��� **************************/
}

void Annulus1_Handle(uchar *data)
{
	uchar i;	// ������
	uchar res;

	Line_Count = 0;	// ����������λ

	Left_Add_Start = 0;		// ��λ������ʼ������
	Right_Add_Start = 0;
	Left_Add_Stop = 0;
	Right_Add_Stop = 0;

	/***************************** ��һ�����⴦�� *****************************/
	res = First_Line_Handle(data);
	if (res == 0)
	{
		Out_Side++;	//����
		return;
	}
	if (Left_Add_Start || Right_Add_Start)
	{
		Width_Min = 60;
	}
	/*************************** ��һ�����⴦����� ***************************/

	for (i = 59; i >= 15;)   //15
	{
		i -= 2;

		if (!data[i*80 + Mid_Line[i+2]])//ǰ2���е��ڱ���Ϊ�ڵ㣬��������
		{
			i += 1;
			break;
		}
		else
		{
			Traversal_Mid_Line(i, data, Mid_Line[i+2], 1, 79, Left_Line, Right_Line, Left_Add_Line, Right_Add_Line);
			/************************** ���߼�⿪ʼ **************************/
//			if (Width_Real[i] >= Width_Min)	// ������ȱ��,�����󻷵�
//			{
                        
                               if(i>=55 && Width_Real[i] >= Width_Min &&Left_Add_Flag[i]==1&&Right_Add_Flag[i]==0&&fourth_signal==0)
                              {       
                                      
                                     first_signal=1;
                                     if (first_signal==1)	//��һ����־λ  ��ֱ��
                                     {
                                       Right_Add_Line[i] = Right_Line[i];                           
                                       Left_Add_Line[i]=range_protect((Right_Add_Line[i] - Width_Max[i]),0,79);
                                     }
                                
                                     
                             }
                             if( (i>=43&& i<=55)&& first_signal==1&& Left_Add_Flag[i]==0&&(Width_Real[i]-Width_Real[i-2]>0) && (Width_Real[i-4]-Width_Real[i-6]<0)&&second_signal==0&&fourth_signal==0)//(Width_Real[i] >= Width_MinWidth_Real[i]-Width_Real[i-2]>0) && (Width_Real[i-4]-Width_Real[i-6]<0)
                             {
                                       //��⵽�м��Բ��Ϊ�ڶ�����־λ�������̵�
                                second_signal=1;
                                
                             }
                                  
                             if( Width_Real[i-2] >= Width_Min && second_signal==1&&fourth_signal==0)//Width_Real[i-2] >= Width_Min && second_signal==1&&fourth_signal==0
                             {
                                 third_signal=1;
                                 first_signal=0;
                                 if(second_signal==1)				// �ڶ�����־λ  ����
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
                                                                                                                            //��������־λ������
                             }
                             if(fourth_signal==1&&i>=55&&Right_Add_Flag[i]==0&&Left_Add_Flag[i]==1)//&&i>=55&&Width_Real[i]>=Width_Min&&Right_Add_Flag[i]==0&&Left_Add_Flag[i]==1
                             {
                              
                                gpio_init(PTE24,GPO,0);
                                fifth_signal=1;                                                                                //�������־λ��������
                                Right_Add_Line[i] = Right_Line[i];                           
                                Left_Add_Line[i]=range_protect((Right_Add_Line[i] - (Width_Max[i]+30)),0,79);

                             }

                             if(fifth_signal==1&&i>=56&&Left_Add_Flag[i]==0 && Right_Add_Flag[i]==0&&Width_Real[i]<=(Width_Min+8) )
                             {
                               first_signal=0;
                               second_signal=0;
                               third_signal=0;
                               fourth_signal=0;
                               fifth_signal=0;                                                                          //��������־λ�����־λ
                               Annulus1_Flag = 0;
                               gpio_init(PTE24,GPO,1);
                            
                             }
                              

			/************************** ���߼����� **************************/
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
		/**//************************* OLED��ʾ�߽� *************************/
		/**/data[i*80 + Left_Add_Line[i] + 2] = 0;	// ��ʾ���ߺ����߽磬����ʱ����
		/**/data[i*80 + Right_Add_Line[i] - 2] = 0;	// ��ʾ���ߺ���ұ߽磬����ʱ����
		/**/data[i*80 + Mid_Line[i]] = 0;			// ��ʾ���ߣ�����ʱ����
//		/**/data[i*80 + Left_Line[i] + 2] = 0;		// ��ʾԭʼ��߽磬����ʱ����
//		/**/data[i*80 + Right_Line[i] - 2] = 0;		// ��ʾԭʼ�ұ߽磬����ʱ����
		/**//************************* OLED��ʾ�߽� *************************/
	}
#endif

        //Annulus_Flag = 0;	// �����·״̬��־λ
	/************************** ��·����״̬�жϽ��� **************************/
}




/************************************ ���㷨 **********************************/

/*
*	ͼ���㷨������ʼ��
*
*	˵������Ӱ���һ�����⴦��
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
*	ͼ�����㷨
*
*	˵����������ͨͼ�񣬰���ʮ�֡��ϰ�
*/
void Image_Handle(uchar *data)
{
	uchar i;	// ������
    uint8 j;

	uchar res;	// ���ڽ��״̬�ж�
	float Result;	// ���ڽ��״̬�ж�
        uchar Add_Start_Max;

	Line_Count = 0;	// ����������λ
	Starting_Line_Times=0;
        Starting_Line_Time=0;
        Starting_Line_Flag=0;
	Left_Hazard_Flag = 0;	// ��λ�����ϰ����־λ
	Right_Hazard_Flag = 0;
	Hazard_Flag=0;

	Left_Add_Start = 0;		// ��λ������ʼ������
	Right_Add_Start = 0;
	Left_Add_Stop = 0;
	Right_Add_Stop = 0;

	/***************************** ��һ�����⴦�� *****************************/
	res = First_Line_Handle(data);
	if (res == 0)
	{
		Out_Side = 1;	// ����
		return;
	}
	Out_Side = 0;
	Line_Count = 59;
	/*************************** ��һ�����⴦����� ***************************/

	for (i = 59; i >= 10;)	// ������ǰ50��ͼ�񣬸��к������25������
	{
		i -= 2;	// ���д�����С��Ƭ������
		Traversal_Mid_Line(i, data, Mid_Line[i+2], 1, 79, Left_Line, Right_Line, Left_Add_Line, Right_Add_Line);
		
        /**************************** �����߼�⿪ʼ ****************************/
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
         /**************************** �򵥵Ļ�����⣬����Ӱ��Сʱʹ�� ****************************/
        
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
        
        
       
          

                
                        /**************************** �򵥵Ļ�����⣬����Ӱ��Сʱʹ�� ****************************/
        /**************************** �ϰ���� ****************************/
        if (i < 57 && i > 30 && Width_Real[i] < 50 && Annulus_Check && !Starting_Line_Flag)
	      {
            if (Right_Add_Line[i+2] - Right_Add_Line[i]<4 && Left_Add_Line[i] > Left_Add_Line[i+2] + 6 && !Left_Add_Start && !Right_Add_Flag[i])	// ������ǰһ����߽��нϴ�ͻ����û�в���
		    {
                for (j=Left_Add_Line[i+2];j<Left_Add_Line[i]-1;j++) //j<�����Ż�
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
                if(Hazard_Flag && Left_Add_Line[i] - Hazard_Flag > 4) //�����жϿ��Բ�Ҫ
                {
                    Hazard_Flag = (Left_Add_Line[i] + Hazard_Flag)/2;
                    for (j=i;j>(i-30);j--)  //i-20
                    {
                        if(data[80*j + Hazard_Flag] && data[80*(j-1) + Hazard_Flag] && data[80*(j-2) + Hazard_Flag])
                        {
                            //Left_Hazard_Flag = i;	// �ϰ����־λ��λ
                            //save_var(VAR2, Left_Hazard_Flag);
                            if(data[80*(i-2) + Left_Add_Line[i]+1] && data[80*(i-2) + Left_Add_Line[i]+3] && data[80*(i-2) + Left_Add_Line[i]+5])
                            {
                                Left_Hazard_Flag = i;	 // �ϰ����־λ��λ
                                // save_var(VAR3, Left_Hazard_Flag);
                            }
                        }
                    }
                }
            }
            else if (Left_Add_Line[i] - Left_Add_Line[i+2] <4 && Right_Add_Line[i] < Right_Add_Line[i+2] - 6 && !Right_Add_Start && !Left_Add_Flag[i])	// ������ǰһ���ұ߽��нϴ�ͻ����û�в���
		    {
		        for (j=Right_Add_Line[i+2];j>Right_Add_Line[i]+1;j--) //j<�����Ż�
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
              if(Hazard_Flag && Hazard_Flag - Right_Add_Line[i]> 4) //�����жϿ��Բ�Ҫ
              {
                  Hazard_Flag = (Right_Add_Line[i] + Hazard_Flag)/2;
                  for (j=i;j>(i-30);j--)  //i-20
                  {
                      if(data[80*j + Hazard_Flag] && data[80*(j-1) + Hazard_Flag] && data[80*(j-2) + Hazard_Flag])
                      {
                        if(data[80*(i-2) + Right_Add_Line[i]-1] && data[80*(i-2) + Right_Add_Line[i]-3] && data[80*(i-2) + Right_Add_Line[i]-5])
                        {
                            Right_Hazard_Flag = i;    // �ϰ����־λ��λ
                            //save_var(VAR3, Right_Hazard_Flag);
                        }
                      }
                  }
              }
		    }
        }
        /**********************************************************************/

		/**************************** ���߼�⿪ʼ ****************************/
        if (Width_Real[i] >= Width_Min+2) 	// ������ȱ��������ʮ�ֻ�·
		{

            if (Left_Add_Flag[i+2])	//ǰһ�в�����
			{
				if (Left_Line[i] < Left_Add_Line[i+2]-1)	//��ǰһ�е���߽�ʵ�߱Ƚ�
				{
					Left_Add_Flag[i] = 1;
				}
			}
			else	//ǰһ��û�в���
			{
				if (Left_Line[i] < Left_Line[i+2]-1)	//��ǰһ�е���߽�ʵ�߱Ƚ�
				{
					Left_Add_Flag[i] = 1;
				}
			}

			if (Right_Add_Flag[i+2])	//ǰһ���ұ߽粹����
			{
				if (Right_Line[i] > Right_Add_Line[i+2]+1)	//��ǰһ�е��ұ߽�ʵ�߽��бȽ�
				{
					Right_Add_Flag[i] = 1;
				}
			}
			else	//ǰһ���ұ߽�û�в���
			{
				if (Right_Line[i] > Right_Line[i+2]+1)		//��ǰһ�е��ұ߽�ʵ�߽��бȽ�
				{
					Right_Add_Flag[i] = 1;
				}
			}
		}
		/**************************** ���߼����� ****************************/



		/*************************** ��һ�ֲ��߿�ʼ ***************************/
		if (Left_Add_Flag[i])	// �����Ҫ����
		{
			if (i >= 55)	// ǰ���в��߲���
			{
				Left_Add_Line[i] = Left_Add_Line[i+2];// ʹ��ǰһ����߽�ֱ�����ϲ��ߣ�������¼���߿�ʼ��
				Left_Add_Start = 53;	// ǿ���϶�53��Ϊ���߿�ʼ��
				Left_Ka = 0;
				Left_Kb = Left_Add_Line[i];
			}
			else
			{
				if (!Left_Add_Start)	// ֮ǰû�в���
				{	// ���߿�ʼ������Ϊ53
					Left_Add_Start=i;
                    //ʹ�����㷨���ֱ��
					Curve_Fitting(&Left_Ka, &Left_Kb, Left_Add_Start+4, Left_Add_Line);
				}
				Left_Add_Line[i] = Calculate_Add(i, Left_Ka, Left_Kb);	// �������
			}
		}
		else
		{
			if (!Left_Add_Stop && Left_Add_Start)	//
			{
				Result = Calculate_Angle(Left_Line[i+4], Left_Line[i+2], Left_Line[i]);	// ͻ�����
				if (Result <= -3)	// -1��135
				{
					Left_Add_Stop = i;	// ��¼��ಹ�߽�����
				}
			}
		}

		if (Right_Add_Flag[i])	// �Ҳ���Ҫ����
		{
			if (i >= 55)	// ǰ���в��߲���
			{
				Right_Add_Line[i] = Right_Add_Line[i+2];// ʹ��ǰһ���ұ߽�ֱ�����ϲ��ߣ�������¼���߿�ʼ��
				Right_Add_Start = 53;	// ǿ���϶�53��Ϊ���߿�ʼ��
				Right_Ka = 0;
				Right_Kb = Right_Add_Line[i];
			}
			else
			{
				if (!Right_Add_Start)	// ֮ǰû�в���
				{   //��¼�Ҳಹ�߿�ʼ��
					Right_Add_Start = i;
                    // ʹ�����㷨���ֱ��
					Curve_Fitting(&Right_Ka, &Right_Kb, Right_Add_Start+4, Right_Add_Line);

				}
				Right_Add_Line[i] = Calculate_Add(i, Right_Ka, Right_Kb);	// �������
			}
		}
		else
		{
			if (!Right_Add_Stop && Right_Add_Start)
			{
				Result = Calculate_Angle(Right_Line[i+4], Right_Line[i+2], Right_Line[i]);	// ͻ�����
				if (Result >= 3)	// -1��135
				{
					Right_Add_Stop = i;	// ��¼�Ҳಹ�߽�����
				}
			}
		}
		/*************************** ��һ�ֲ��߽��� ***************************/

		Width_Add[i] = Right_Add_Line[i] - Left_Add_Line[i];	// ���¼����������
		if ((Left_Add_Flag[59] && Left_Add_Start && !Left_Add_Stop) && (Right_Add_Flag[59] && Right_Add_Start && !Right_Add_Stop))
		{
			Mid_Line[i] = Mid_Line[i+2];
		}
		else
		{
			Mid_Line[i] = (Right_Add_Line[i] + Left_Add_Line[i]) / 2;	// ��������
		}
		if (Width_Add[i] < Width_Min)
		{
			Width_Min = Width_Add[i];	// ������С�������
		}

		Line_Count = i;	// ��¼�ɹ�ʶ�𵽵���������
                if(Left_Add_Stop&&Right_Add_Stop)    //���߽�������Ҫ�����ⲻ��
                {
                    if(Left_Add_Flag[i]||Right_Add_Flag[i])
                    {
                        Line_Count = i+2;  //break;
                        break;             //Line_Count = i+2;
                    }
                }
	}
	/*************************** �ڶ��ֲ����޸���ʼ ***************************/
	if (Left_Add_Start && Right_Add_Start)	// ���Ҷ���Ҫ����
	{
		if (Left_Add_Stop)	// ��ʼ����
		{
			Line_Repair(Left_Add_Start, Left_Add_Stop, data, Left_Line, Left_Add_Line, Left_Add_Flag);
		}
		if (Right_Add_Stop)	// ��ʼ����
		{
			Line_Repair(Right_Add_Start,Right_Add_Stop , data, Right_Line, Right_Add_Line, Right_Add_Flag);
		}
	}
	/*************************** �ڶ��ֲ����޸����� ***************************/

	/****************************** �����޸���ʼ ******************************/
	Mid_Line_Repair(Line_Count, data);
	/****************************** �����޸����� ******************************/
}

/*
*	����ƽ��
*
*	˵�����������⴦��ֱ��ȡ��ֵ
*/
uchar Point_Average(void)
{
	uchar i, Half_Line;
	int32 Point, Sum = 0;
	static uchar Last_Point = 40;

	if (Out_Side || Line_Count >= 55)	// �����������ͷͼ���쳣
	{
		Point  = Last_Point;			// ʹ���ϴ�Ŀ���
	}
	else
	{
		Half_Line = 59-(59-Line_Count)/2;	// ������Ч�е��м���
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
			Point = Sum / ((61-Line_Count)/2);	// ��������ƽ��
			if (Left_Add_Line[Half_Line] >= 20)				// ̫ƫ��
			{
				Point += (Left_Add_Line[Half_Line]-20);	// �ʵ�����
			}
			if (Right_Add_Line[Half_Line] <= 60)				// ̫ƫ��
			{
				Point -= (60-Right_Add_Line[Half_Line]);	// �ʵ�����
			}
		}

		//Point = Point*0.9 + Last_Point*0.1;	// ��ͨ�˲�
                Point = (int)(Point*0.9 + Last_Point*0.1);	// ��ͨ�˲�
		Point = range_protect(Point, 1, 79);		// �޷�����ֹ�������

		/********************* �ϰ�������������� ********************/
		if (Left_Hazard_Flag)			//������ϰ�������Ҫ���ߣ���ʹ����Ҳ�������Ӱ��
		{
			Point = Mid_Line[Left_Hazard_Flag]+6;	//ʹ���ϰ�����ֵ���һ���е���ΪĿ���
			if (Left_Hazard_Flag < 40)
			{
				Point += 3;
			}
		}
		else if (Right_Hazard_Flag)	//�Ҳ����ϰ�������Ҫ���ߣ���ʹ����Ҳ�������Ӱ��
		{
			Point = Mid_Line[Right_Hazard_Flag]-6;//ʹ���ϰ�����ֵ���һ���е���ΪĿ���
			if (Right_Hazard_Flag < 40)
			{
				Point -= 5;
			}
		}
        /************************************************************/
		Last_Point = Point;	// �����ϴ�Ŀ���
	}

	return Point;
}

/*
*	��С���˷�����б��
*
*	˵��������б����Ϊת�������,����Ŵ�100��
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
*	б�ʼ���
*
*	˵����ʹ��б����Ϊת�������������б�ʵĵ���
*/
float Slope_Weight(uchar *Mid)
{
	float Slope;

	Slope = 1.0 * (Mid[Line_Count] - 40) / (60-Line_Count);

	return Slope;
}

/*
*	�����Ƕȼ���
*
*	˵�������ؽ��Ϊ �ң�0��180�㣬1��135�㣬2��90�㣬>= 1����Ϊֱ��ͻ��
*					 ��0��180�㣬-1��135�㣬-2��90�㣬<= -1����Ϊֱ��ͻ��
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
*	���㷨��ֱ��
*
*	˵�������ֱ�� y = Ka * x + Kb
*/
void Curve_Fitting(float *Ka, float *Kb, uchar Start, uchar *Line)
{
	*Ka = 1.0*(Line[Start+4] - Line[Start]) / 4;
	*Kb = 1.0*Line[Start] - (*Ka * Start);
}

/*
*	���㷨��ֱ��
*
*	˵�������ֱ�� y = Ka * x + Kb   Mode == 1������߽磬Mode == 2�����ұ߽�
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
*	���㲹������
*
*	˵����ʹ�����㷨������ϳ��Ĳ�������
*/
uchar Calculate_Add(uchar i, float Ka, float Kb)	// ���㲹������
{
	float res;
	uchar Result;

	res = i * Ka + Kb;
	Result = range_protect((int32)res, 1, 79);

	return Result;
}

/*
*	��ͷ����
*
*	˵������ĳһ�㿪ʼ��ֱ����������������Զ������
*/
uchar Limit_Scan(uchar i, uchar *data, uchar Point)
{
	for ( ; i >= 20; i--)
	{
		if (!data[80*i + Point])	// �������ڵ�
		{
			break;
		}
	}

	return i;	// ������Զ������
}

/*
*	��һ�����⴦��
*
*	˵������ʹ�õ�60���е���Ϊ��59��(��һ��)������ʼλ�ã��ɹ����������ұ߽��
 	����59���е㸳ֵ����60�б�����һ֡ͼ��ʹ�á������60���е��ڱ���Ϊ�ڵ㣬��
 	�ֱ�ʹ����������ұ����ķ��������߽磬��������Ƚϴ�Ľ����Ϊ��59�б߽磬
 	����Ȼ���������߽�������쳣��Ϊ���磬���緵��0
*/
uchar First_Line_Handle(uchar *data)
{
	uchar i;	// ������
	uchar Weight_Left, Weight_Right;	// �����������
	uchar Mid_Left, Mid_Right;
	Image_Para_Init();  //ͼ���㷨������ʼ��
	i = 59;

	if (!data[i*80 + Mid_Line[61]])	// ��61���е��ڵ�59��Ϊ�ڵ�
	{
		Weight_Left = Traversal_Left(i, data, &Mid_Left, 1, 79);	// ����������߽�
		Weight_Right = Traversal_Right(i, data, &Mid_Right, 1, 79); // ���Ҳ������߽�
		if (Weight_Left >= Weight_Right && Weight_Left)	// ��������ȴ�������������Ҳ�Ϊ0
		{
			Traversal_Left_Line(i, data, Left_Line, Right_Line);	// ʹ���������ȡ�����߽�
		}
		else if (Weight_Left < Weight_Right && Weight_Right)
		{
			Traversal_Right_Line(i, data, Left_Line, Right_Line);	// ʹ���ұ�����ȡ�����߽�
		}
		else	// ˵��û�鵽
		{
			return 0;
		}
	}
	else
	{
		Traversal_Mid_Line(i, data, Mid_Line[i+2], 1, 79, Left_Line, Right_Line, Left_Add_Line, Right_Add_Line);	// ��ǰһ���е�������ɨ��
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
		Mid_Line[61] = Mid_Line[59];	// ���µ�60�������е㣬������һ֡ͼ��ʹ��
	}
	Width_Real[61] = Width_Real[59];
	Width_Add[61] = Width_Add[59];
	Width_Min = Width_Add[59];

	return 1;
}

/*
*	����࿪ʼ�����߽磬�����������
*
*	˵����������������Ϊ̽������ʹ�ã�������������ȣ�������߽�����
*/
uchar Traversal_Left(uchar i, uchar *data, uchar *Mid, uchar Left_Min, uchar Right_Max)
{
	uchar j, White_Flag = 0;
	uchar Left_Line = Left_Min, Right_Line = Right_Max;

	for (j = Left_Min; j <= Right_Max; j++)	// �߽����� 1��79
	{
		if (!White_Flag)	// �Ȳ�����߽�
		{
			if (data[i*80 + j])	// ��⵽�׵�
			{
				Left_Line = j;	// ��¼��ǰjֵΪ������߽�
				White_Flag = 1;	// ��߽����ҵ��������ұ߽�

				continue;	// ��������ѭ���������´�ѭ��
			}
		}
		else
		{
			if (!data[i*80 + j])//���ڵ�
			{
				Right_Line = j-1;//��¼��ǰjֵΪ�����ұ߽�

				break;	// ���ұ߽綼�ҵ�������ѭ��
			}
		}
	}

	if (!White_Flag)	// δ�ҵ����ұ߽�
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
*	���Ҳ࿪ʼ�����߽磬�����������
*
*	˵����������������Ϊ̽������ʹ�ã�������������ȣ�������߽�����
*/
uchar Traversal_Right(uchar i, uchar *data, uchar *Mid, uchar Left_Min, uchar Right_Max)
{
	uchar j, White_Flag = 0;
	uchar Left_Line = Left_Min, Right_Line = Right_Max;

	for (j = Right_Max; j >= Left_Min; j--)	// �߽����� 1��79
	{
		if (!White_Flag)	// �Ȳ����ұ߽�
		{
			if (data[i*80 + j])	// ��⵽�׵�
			{
				Right_Line = j;	// ��¼��ǰjֵΪ�����ұ߽�
				White_Flag = 1;	// �ұ߽����ҵ���������߽�

				continue;	// ��������ѭ���������´�ѭ��
			}
		}
		else
		{
			if (!data[i*80 + j])	//���ڵ�
			{
				Left_Line = j+1;	//��¼��ǰjֵΪ������߽�

				break;	// ���ұ߽綼�ҵ�������ѭ��
			}
		}
	}

	if (!White_Flag)	// δ�ҵ����ұ߽�
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
*	����࿪ʼ�����߽磬���������߽磬����1�ɹ� 0ʧ��
*
*	˵����������ʹ�ú󽫱���߽�����
*/
uchar Traversal_Left_Line(uchar i, uchar *data, uchar *Left_Line, uchar *Right_Line)
{
	uchar j, White_Flag = 0;

	Left_Line[i] = 1;
	Right_Line[i] = 79;

	for (j = 1; j < 80; j++)	// �߽����� 1��79
	{
		if (!White_Flag)	// �Ȳ�����߽�
		{
			if (data[i*80 + j])	// ��⵽�׵�
			{
				Left_Line[i] = j;	// ��¼��ǰjֵΪ������߽�
				White_Flag = 1;		// ��߽����ҵ��������ұ߽�

				continue;	// ��������ѭ���������´�ѭ��
			}
		}
		else
		{
			if (!data[i*80 + j])	//���ڵ�
			{
				Right_Line[i] = j-1;//��¼��ǰjֵΪ�����ұ߽�

				break;	// ���ұ߽綼�ҵ�������ѭ��
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

	return White_Flag;	// �����������
}

/*
*	���Ҳ࿪ʼ�����߽磬���������߽磬����1�ɹ� 0ʧ��
*
*	˵����������ʹ�ú󽫱���߽�����
*/
uchar Traversal_Right_Line(uchar i, uchar *data, uchar *Left_Line, uchar *Right_Line)
{
	uchar j, White_Flag = 0;

	Left_Line[i] = 1;
	Right_Line[i] = 79;

	for (j = 79; j > 0; j--)	// �߽����� 1��79
	{
		if (!White_Flag)	// �Ȳ����ұ߽�
		{
			if (data[i*80 + j])	// ��⵽�׵�
			{
				Right_Line[i] = j;	// ��¼��ǰjֵΪ�����ұ߽�
				White_Flag = 1;		// �ұ߽����ҵ���������߽�

				continue;	// ��������ѭ���������´�ѭ��
			}
		}
		else
		{
			if (!data[i*80 + j])	//���ڵ�
			{
				Left_Line[i] = j+1;//��¼��ǰjֵΪ������߽�

				break;	// ���ұ߽綼�ҵ�������ѭ��
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

	return White_Flag;	// �����������
}

/*
*	���м������������߽�
*
*	˵����������ʹ�ú󽫱���߽�����
*/
void Traversal_Mid_Line(uchar i, uchar *data, uchar Mid, uchar Left_Min, uchar Right_Max, uchar *Left_Line, uchar *Right_Line, uchar *Left_Add_Line, uchar *Right_Add_Line)
{
	uchar j;

	Left_Add_Flag[i] = 1;	// ��ʼ�����߱�־λ
	Right_Add_Flag[i] = 1;

////	Left_Min = range_protect(Left_Min, 1, 79);	// �޷�����ֹ����
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
	Left_Line[i] = Left_Min;	// �����߽��ʼֵ

	for (j = Mid; j >= Left_Min+1; j--)	// ��ǰһ���е�Ϊ���������ұ߽�
	{
		if (!data[i*80 + j]&&!data[i*80 + j-1])	// ��⵽�ڵ�
		{
			Left_Add_Flag[i] = 0;	//��߽粻��Ҫ���ߣ������־λ
			Left_Line[i] = j;	    //��¼��ǰjֵΪ����ʵ����߽�
			Left_Add_Line[i] = j;	// ��¼ʵ����߽�Ϊ������߽�

			break;
		}
	}
	for (j = Mid; j <= Right_Max-1; j++)	// ��ǰһ���е�Ϊ������Ҳ����ұ߽�
	{
		if (!data[i*80 + j]&&!data[i*80 + j+1])	//��⵽�ڵ�
		{
			Right_Add_Flag[i] = 0;  //�ұ߽粻��Ҫ���ߣ������־λ
			Right_Line[i] = j;	    //��¼��ǰjֵΪ�����ұ߽�
			Right_Add_Line[i] = j;	// ��¼ʵ���ұ߽�Ϊ������߽�

			break;
		}
	}
	if (Left_Add_Flag[i])	// ��߽���Ҫ����
	{
		if (i >= 55)	// ǰ6��
		{
			Left_Add_Line[i] = Left_Line[59];	// ʹ�õ�������
		}
		else                     ///
		{
			Left_Add_Line[i] = Left_Add_Line[i+2];	// ʹ��ǰ2����߽���Ϊ������߽�
		}
	}
	if (Right_Add_Flag[i])	// �ұ߽���Ҫ����
	{
		if (i >= 55)	// ǰ6��
		{
			Right_Add_Line[i] = Right_Line[59];	// ʹ�õ�������
		}
		else         //////
		{
			Right_Add_Line[i] = Right_Add_Line[i+2];	// ʹ��ǰ2���ұ߽���Ϊ�����ұ߽�
		}
	}
	Width_Real[i] = Right_Line[i] - Left_Line[i];			// ����ʵ���������
	Width_Add[i] = Right_Add_Line[i] - Left_Add_Line[i];	// ���㲹���������

}

/*
*	�����޸�
*
*	˵������ʼ���ղ�ʹ�ã�ֱ��ʹ������б�ʽ��в���
*/
void Line_Repair(uchar Start, uchar Stop, uchar *data, uchar *Line, uchar *Line_Add, uchar *Add_Flag)
{
	float res;
	uchar i;	// ������
	float Ka, Kb;

	if (Start >= 55)
	{
		Start = 59;
	}
	else
	{
		Start += 4;            ////
	}

	if (Add_Flag[59] && Line_Count+4 <= Stop)	// �������²���
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
*	�����޸�
*
*	˵������ͨ�������ʹ��ƽ��������ʽ���е㵽��߽����
*/
void Mid_Line_Repair(uchar count, uchar *data)
{
	char res;
	uchar i,j;  //Left_Hazard,Right_Hazard;	// ������
	//float Ka, Kb;

    //Left_Hazard = 0;
    //Right_Hazard = 0;

	Left_Max = Left_Add_Line[59];
	Right_Min = Right_Add_Line[59];

	for (i = 61; i >= count+2; )
	{
		i -= 2;
		if (Left_Add_Flag[i] && Right_Add_Flag[i])	// ���Ҷ���Ҫ����
		{
			Mid_Line[i] = (Right_Add_Line[i] + Left_Add_Line[i]) / 2;	// ���������е�
			Width_Add[i] = Right_Add_Line[i] - Left_Add_Line[i];		// �����������
		}
		else if (Left_Add_Flag[i])	// �������Ҫ����
		{
			if (Left_Add_Line[i] == 1 && !Left_Add_Stop)	// ���������ˣ����߱��ο���ֵ
			{
                      //ƽ���������һ��
			res = Right_Add_Line[i] - Width_Add[i+2] / 2 + (Right_Add_Line[i+2] - Right_Add_Line[i]) / 2;
				Width_Add[i] = 2 * (Right_Add_Line[i] - res);	// �����ұ߽�����������������
				if (res <= 0)	// �е��ѵ���ͼ���ޣ����پ߱��ο���ֵ
				{
					i += 2;	// ��¼�ɹ�ʶ�𵽵��е�����
					break;
				}
			}
			else
			{
				res = (Right_Add_Line[i] + Left_Add_Line[i]) / 2;		// ���������е�
				Width_Add[i] = Right_Add_Line[i] - Left_Add_Line[i];	// �����������
			}
			Mid_Line[i] = res;	// �����е�
		}
		else if (Right_Add_Flag[i])	// ���Ҳ���Ҫ����
		{
			if (Right_Add_Line[i] == 79 && !Right_Add_Stop)	// ���������ˣ����߱��ο���ֵ
			{
				res = Left_Add_Line[i] + Width_Add[i+2] / 2 - (Left_Add_Line[i]-Left_Add_Line[i+2]) / 2;	// ƽ���������һ��
				Width_Add[i] = 2 * (res - Left_Add_Line[i]);	// �������ߺ���߽������������
				if (res >= 80)	// �е��ѵ���ͼ���ޣ����پ߱��ο���ֵ
				{
					i += 2;	// ��¼�ɹ�ʶ�𵽵��е�����
					break;
				}
			}
			else
			{
				res = (Right_Add_Line[i] + Left_Add_Line[i]) / 2;		// ���������е�
				Width_Add[i] = Right_Add_Line[i] - Left_Add_Line[i];	// �����������
			}
			Mid_Line[i] = res;	// �����е�
		}

        //�ϰ�����
        if(Left_Hazard_Flag)
        {
            Curve2_Fitting(&Left_Ka, &Left_Kb, 59,Left_Hazard_Flag, Left_Line, 1);
            for (j=59;j>=Left_Hazard_Flag;j--)
            {
                Left_Add_Line[j] = Calculate_Add(j, Left_Ka,Left_Kb);	// �������
                Mid_Line[j] = (Right_Add_Line[j] + Left_Add_Line[j]) / 2;
            }
        }
        //BuzzerTest(Left_Hazard_Flag);
        if(Right_Hazard_Flag)
        {
            Curve2_Fitting(&Right_Ka, &Right_Kb, 59,Right_Hazard_Flag, Right_Line, 2);
            for (j=59;j>=Right_Hazard_Flag;j--)
            {
                Right_Add_Line[j] = Calculate_Add(j, Right_Ka, Right_Kb);	// �������
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
	/**//*************************** ��λ����ʾ�߽� ***************************/
	/**/data[i*80 + Left_Add_Line[i] + 2] = 0;	// ��λ����ʾ���ߺ����߽磬����ʱ����
	/**/data[i*80 + Right_Add_Line[i] - 2] = 0;	// ��λ����ʾ���ߺ���ұ߽磬����ʱ����
	/**/data[i*80 + Mid_Line[i]] = 0;			// ��λ����ʾ���ߣ�����ʱ����
//	/**/data[i*80 + Left_Line[i] + 2] = 0;		// ��λ����ʾԭʼ��߽磬����ʱ����
//	/**/data[i*80 + Right_Line[i] - 2] = 0;		// ��λ����ʾԭʼ�ұ߽磬����ʱ����
	/**//*************************** ��λ����ʾ�߽� ***************************/
#endif
	}
	Mid_Line[61] = Mid_Line[59];
	Line_Count = i;		// ��¼�ɹ�ʶ�𵽵��е�����

	if (Left_Add_Stop)
	{
		Curve_Fitting(&Left_Ka, &Left_Kb, Left_Add_Stop, Left_Add_Line);
	}
	if (Right_Add_Stop)
	{
		Curve_Fitting(&Right_Ka, &Right_Kb, Right_Add_Stop, Right_Add_Line);
	}
}

/****************** ���㷨 ******************/

/*
*	��Ȩƽ��
*
*	˵����Ȩ�����Ҵյģ�Ч������
*/
uchar Point_Weight(void)
{
	uchar i ,Point, Point_Mid;

	int32 Sum = 0, Weight_Count = 0;

	if (Out_Side || Line_Count >= 57)	//�����������ͷͼ���쳣
	{
		Point  = Last_Point;
               // Speed=0;
                if(Annulus_Check)
                {
                   Stop_Flag = 1;
                }
	}
    if(Starting_Line_Flag)   //������ʶ�����1
    {
        Point  = Last_Point;
        // Speed=0;
        if(Annulus_Check)    //���ܺ���ʱ50ms��1
        {
            Stop_Flag = 1;   //�ٴμ����ͣ��
        }
    }
	else
	{
          	if (Line_Count <= 20)  //20
                {
                  Line_Count = 21; //21
                }
		for (i = 61; i >= Line_Count+2; )		//ʹ�ü�Ȩƽ��
		{
			i -= 2;
			Sum += Mid_Line[i] * Weight[59-i];
			Weight_Count += Weight[59-i];
		}
		Point = range_protect(Sum / Weight_Count, 2, 78);
		Point = range_protect(Point, 2, 78);
		Last_Point = Point;

							/***** ʹ����Զ�����ݺ�Ŀ�����Ϊǰհ *****/
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
//            Foresight = 0.8 * Error_Transform(Point_Mid, 40)	//ʹ����Զ��ƫ��ͼ�Ȩƫ��ȷ��ǰհ
//			  + 0.2 * Error_Transform(Point,  40);//+0.1*Ek_compar;
//        }
//	else
//        {
            //Foresight = 0.8 * Error_Transform(Point_Mid, 40)	//ʹ����Զ��ƫ��ͼ�Ȩƫ��ȷ��ǰհ
			//  + 0.2 * Error_Transform(Point,  40);//+0.1*Ek_compar;
            Foresight = (int)(0.8 * Error_Transform(Point_Mid, 40)	//ʹ����Զ��ƫ��ͼ�Ȩƫ��ȷ��ǰհ
			  + 0.2 * Error_Transform(Point,  40));//+0.1*Ek_compar;
//        }

	return Point;
}

char Error_Transform(uchar Data, uchar Set)   //�����ֵ
{
	char Error;

	Error = Set - Data;
	if (Error <= 0)
	{
		Error = -Error;
	}

	return Error;
}
int32 abs_int(int32 i)//���ã� �����ֵ  int32��
{
     int32 B;
      B=i;
     if(B<0) return (-B);
     else    return B;
}
/*************��������********************
          �˲�������ֵ�˲�

          ���Ż�
*************3��13��  ��֤ͨ�� ***********/
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
*  @brief      ��ֵ��ͼ���ѹ���ռ� �� ʱ�� ��ѹ��
*  @param      dst             ͼ���ѹĿ�ĵ�ַ
*  @param      src             ͼ���ѹԴ��ַ
*  @param      srclen          ��ֵ��ͼ���ռ�ÿռ��С
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
*  @brief      ��ֵ��ͼ��ѹ�����ռ� �� ʱ�� ѹ����
*  @param      dst             ͼ��ѹ��Ŀ�ĵ�ַ
*  @param      src             ͼ��ѹ��Դ��ַ
*  @param      srclen          ��ֵ��ͼ���ռ�ÿռ��С
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
*  @brief      ȡ�߽���
*  @param      dst             ͼ��ѹ��Ŀ�ĵ�ַ
*  @param      src             ͼ��ѹ��Դ��ַ
*  @param      srclen          ��ֵ��ͼ���ռ�ÿռ��С
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

