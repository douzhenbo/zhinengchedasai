/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2013,山外科技
 *     All rights reserved.
 *     技术讨论：山外论坛 http://www.vcan123.com
 *
 *     除注明出处外，以下所有内容版权均属山外科技所有，未经允许，不得用于商业用途，
 *     修改内容时必须保留山外科技的版权声明。
 *
 * @file       MK60_it.c
 * @brief      山外K60 平台中断服务函数
 * @author     山外科技
 * @version    v5.0
 * @date       2013-06-26
 */

#include    "MKL_it.h"
#include    "common.h"
#include    "include.h"

/*********************************中断服务函数********************************/

/*!
 *  @brief      PORTA中断服务函数
 *  @since      v5.0
 */
void PORTA_IRQHandler()
{
    uint8  n = 0;    //引脚号
    uint32 flag = PORTA_ISFR;
    PORTA_ISFR  = ~0;                                   //清中断标志位

    n = 6;                                              //场中断
    if(flag & (1 << n))                                 //PTA6触发中断
    {
        camera_vsync();
    }
}

/*!
 *  @brief      DMA0中断服务函数
 *  @since      v5.0
 */
void DMA0_IRQHandler()
{
    camera_dma();
}

/*!
*  @brief      PIT0中断服务函数
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
    /******** 起跑延时 *********/      
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
    
    
    /******** 坡道延时 *********/  
    if (Ramp_Flag /*|| Ramp_Time*/ )
    {
        if(Ramp_Delay>0)
        {  Ramp_Delay--;  }
        if(Ramp_Delay==0)
        {
            Ramp_Flag=0;
        }
        
    }
    /******** 障碍延时 *********/  
    if (Left_Hazard_Flag || Right_Hazard_Flag)
    {
       Hazard_Time++;
       if(Hazard_Time>200)
       {
          Left_Hazard_Flag=0;
          Right_Hazard_Flag=0;
       }
    }   
    /******** 图像延时 *********/ 
    if (Time_150ms == 15)
    {
      Time_150ms = 0;
      Send_OK = 1;	// 图像150ms发送一次
    }
    /******** 环路延时 *********/ 
    if (Annulus1_Flag)
    {
       Annulus_DelayMs++;
    }
    if (Annulus2_Delay)
    {
        Annulus2_Delay--;
    }   
    
    speed_measure();	    //测速及停车	测试期间暂时不用
    
    PIT_Flag_Clear(PIT0);   //清中断标志位
}


/******************************中断服务函数结束*******************************/





