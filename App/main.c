#include "common.h"
#include "include.h"

uint8 imgbuff[CAMERA_SIZE];                             //定义存储接收图像的数组
uint8 img[CAMERA_H * CAMERA_W];
uint8 screen_mid = 40;		  //屏幕中点
uint8 Point_Mid  = 40;
extern int32 S3010_Duty;      //4300
uint8 Point;	//目标点横坐标
extern uint8 CNST_VALUE;
extern uint8 CNST_Value ;

void LCD_dispaly(void);

//函数声明
void vcan_sendimg(uint8 *imgaddr, uint32 imgsize);
void img_extract(uint8 *dst, uint8 *src, uint32 srclen);


void  main(void)
{
    Point = 40;     //中心线
    System_Init();  //初始化所有模块
    if(img_display==1)
    {
        LCD_Clear(LCD_WHITE);
        LCD_DrawRectangle(0,0,80,60);
    }
  
    
    while(1)
    {
        camera_get_img();
        img_extract(img, imgbuff,CAMERA_SIZE);
        image_processing(img);
        Point = Point_Weight();  //获取目标点	计算目标角度
        S3010_Duty = PlacePID_Control(&S3010_PID,Point,Point_Mid);	     //使用位置式PID
        S3010_Duty = range_protect(S3010_Duty, S3010_LEFT, S3010_RIGHT); //舵机占空比限幅保护
        tpm_pwm_duty(S3010_TPM, S3010_CH,S3010_Duty);//S3010_Duty
//        //vcan_sendimg(img,CAMERA_SIZE);
        if(img_display==1)
        {
            LCD_dispaly();
            LCD_Num_BC(105,69,MOTOR_Speed,16,3,LCD_BLACK,LCD_WHITE);
        }
        else
        {       }
          
    }
  
//  while(1)
//  {
//     tpm_pwm_init(S3010_TPM, S3010_CH,S3010_HZ,S3010_MID);
//  }
  
  
}


//左上角显示img80*60图像
void LCD_dispaly(void)
{

    uint32 temp, tempY;
    uint16 x, y;
    uint16 X, Y;
    uint8 *pimg = (uint8 *)img;
    uint16 rgb;

//    Site_t site     = {0, 0};    //显示图像左上角位置
    Size_t imgsize  = {80, 60};    //图像大小
    Size_t size     = {160, 60};    //显示区域图像大小


    LCD_SetWindows(0-2,0+29,80-1-2,60-1+29);
    //LCD_RAMWR();                                //写内存

    for(y = 0; y < size.H; y++)
    {
        Y = ( (  y * imgsize.H   ) / size.H) ;
        tempY = Y * imgsize.W ;

        for(x = 0; x < size.W; x++)
        {
            X = ( x * imgsize.W  ) / size.W ;
            temp = tempY + X;
            rgb = GRAY_2_RGB565(pimg[temp]);    //
            LCD_WR_DATA(rgb);    //写八位数据
            //LCD_WR_DATA_16Bit(rgb);//写十六位数据
        }
    }

}

//发送图像到上位机显示
//不同的上位机，不同的命令
//如果使用其他上位机，则需要修改代码
void vcan_sendimg(uint8 *imgaddr, uint32 imgsize)
{
#define CMD_IMG     1
    uint8 cmdf[2] = {CMD_IMG, ~CMD_IMG};    //山外上位机 使用的命令
    uint8 cmdr[2] = {~CMD_IMG, CMD_IMG};    //山外上位机 使用的命令

    uart_putbuff(VCAN_PORT, cmdf, sizeof(cmdf));    //先发送命令

    uart_putbuff(VCAN_PORT, imgaddr, imgsize); //再发送图像

    uart_putbuff(VCAN_PORT, cmdr, sizeof(cmdr));    //先发送命令
}


/*!
 *  @brief      二值化图像解压（空间 换 时间 解压）
 *  @param      dst             图像解压目的地址
 *  @param      src             图像解压源地址
 *  @param      srclen          二值化图像的占用空间大小
 *  @since      v5.0            img_extract(img, imgbuff,CAMERA_SIZE);
 *  Sample usage:
 */
void img_extract(uint8 *dst, uint8 *src, uint32 srclen)
{
    uint8 colour[2] = {255, 0}; //0 和 1 分别对应的颜色
    //注：山外的摄像头 0 表示 白色，1表示 黑色
    uint8 tmpsrc;
    while(srclen --)
    {
        tmpsrc = *src++;
        *dst++ = colour[ (tmpsrc >> 7 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 6 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 5 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 4 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 3 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 2 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 1 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 0 ) & 0x01 ];
    }
}

