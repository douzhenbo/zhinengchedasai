/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2013,ɽ��Ƽ�
 *     All rights reserved.
 *     �������ۣ�ɽ����̳ http://www.vcan123.com
 *
 *     ��ע�������⣬�����������ݰ�Ȩ����ɽ��Ƽ����У�δ����������������ҵ��;��
 *     �޸�����ʱ���뱣��ɽ��Ƽ��İ�Ȩ������
 *
 * @file       MK60_it.c
 * @brief      ɽ��K60 ƽ̨�жϷ�����
 * @author     ɽ��Ƽ�
 * @version    v5.0
 * @date       2013-06-26
 */

#include    "MKL_it.h"
#include    "common.h"
#include    "include.h"

/*********************************�жϷ�����********************************/

/*!
 *  @brief      PORTA�жϷ�����
 *  @since      v5.0
 */
void PORTA_IRQHandler()
{
    uint8  n = 0;    //���ź�
    uint32 flag = PORTA_ISFR;
    PORTA_ISFR  = ~0;                                   //���жϱ�־λ

    n = 6;                                              //���ж�
    if(flag & (1 << n))                                 //PTA6�����ж�
    {
        camera_vsync();
    }
}

/*!
 *  @brief      DMA0�жϷ�����
 *  @since      v5.0
 */
void DMA0_IRQHandler()
{
    camera_dma();
}

/*!
*  @brief      PIT0�жϷ�����
*  @since      v5.0
*/
extern uchar Annulus1_Flag;
extern uchar Annulus2_Flag;
extern uint16 Annulus_DelayMs;
extern int16 Starting_Time;
extern int8 Starting1_Time;
extern int8 Stop_Time;
extern int16 Stop1_Time;
extern uint8 Ramp_Flag;
extern uint16 Ramp_Time;
extern uint8 Hazard_Time;
extern uchar Left_Hazard_Flag;
extern uchar Right_Hazard_Flag;

void PIT_IRQHandler()
{	
    static uchar Time_150ms = 0;
    /******** ������ʱ *********/      
    Time_150ms++;
    if (Starting_Time>0)
    {
          Starting_Time--;
    }
    if(Starting1_Time>0)
    {
        Starting1_Time--;
    }
    if(Stop_Time>0)
    {
          Stop_Time--;
    }
    if(Stop1_Time>0)
    {
          Stop1_Time--;
    }
    
//    if(Start_Pian_Delaytime>0)
//    {
//          Start_Pian_Delaytime--;
//    }
    
    
    /******** �µ���ʱ *********/  
    if (Ramp_Flag /*|| Ramp_Time*/ )
    {
        if(Ramp_Delay>0)
        {  Ramp_Delay--;  }
        if(Ramp_Delay==0)
        {
            Ramp_Flag=0;
        }
        
    }
    /******** �ϰ���ʱ *********/  
    if (Left_Hazard_Flag || Right_Hazard_Flag)
    {
       Hazard_Time++;
       if(Hazard_Time>200)
       {
          Left_Hazard_Flag=0;
          Right_Hazard_Flag=0;
       }
    }   
    /******** ͼ����ʱ *********/ 
    if (Time_150ms == 15)
    {
      Time_150ms = 0;
      Send_OK = 1;	// ͼ��150ms����һ��
    }
    /******** ��·��ʱ *********/ 
    if (Annulus1_Flag)
    {
       Annulus_DelayMs++;
    }
    if (Annulus2_Delay)
    {
        Annulus2_Delay--;
    }   
    
    speed_measure();	    //���ټ�ͣ��	�����ڼ���ʱ����
    
    PIT_Flag_Clear(PIT0);   //���жϱ�־λ
}


/******************************�жϷ���������*******************************/





