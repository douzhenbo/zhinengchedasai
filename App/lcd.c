#include "common.h"
#include "lcd.h"
#include "font.h"
#include "MKL_port.h"
#include "MKL_gpio.h"
#include "VCAN_OV7725_Eagle.h"

//管理LCD重要参数
//默认为竖屏
_lcd_dev lcddev;

//画笔颜色：黑色,背景颜色：白色
uint16 POINT_COLOR = 0x0000,BACK_COLOR = 0xFFFF;
uint16 DeviceCode;

/****************************************************************************
* 名    称：void  SPIv_WriteData(uint8 Data)
* 功    能：STM32_模拟SPI写一个字节数据底层函数
* 入口参数：Data
* 出口参数：无
* 说    明：STM32_模拟SPI读写一个字节数据底层函数
****************************************************************************/
void  SPIv_WriteData(uint8 Data)
{
	unsigned char i=0;
	for(i=8;i>0;i--)
	{
		if(Data&0x80)
	  LCD_SDA_SET; //输出数据
      else LCD_SDA_CLR;

      LCD_SCL_CLR;
      LCD_SCL_SET;
      Data<<=1;
	}
}


//******************************************************************
//函数名：  LCD_WR_REG
//功能：    向液晶屏总线写入写16位指令
//输入参数：Reg:待写入的指令值
//返回值：  无
//******************************************************************
void LCD_WR_REG(uint16 data)
{
   LCD_CS_CLR;
   LCD_RS_CLR;
#if USE_HARDWARE_SPI
   //SPI_WriteByte(SPI2,data);
#else
   SPIv_WriteData(data);
#endif
   LCD_CS_SET;
}

//******************************************************************
//函数名：  LCD_WR_DATA
//功能：    向液晶屏总线写入写8位数据
//输入参数：Data:待写入的数据
//返回值：  无
//******************************************************************
void LCD_WR_DATA(uint8 data)
{

   LCD_CS_CLR;
   LCD_RS_SET;
#if USE_HARDWARE_SPI
   //SPI_WriteByte(SPI2,data);
#else
   SPIv_WriteData(data);
#endif
   LCD_CS_SET;

}
//******************************************************************
//函数名：  LCD_DrawPoint_16Bit
//功能：    8位总线下如何写入一个16位数据
//输入参数：(x,y):光标坐标
//返回值：  无
//******************************************************************
void LCD_WR_DATA_16Bit(uint16 data)
{
   LCD_CS_CLR;
   LCD_RS_SET;
#if USE_HARDWARE_SPI
  // SPI_WriteByte(SPI2,data>>8);
  // SPI_WriteByte(SPI2,data);
#else
   SPIv_WriteData(data>>8);
   SPIv_WriteData(data);
#endif
   LCD_CS_SET;
}

//******************************************************************
//函数名：  LCD_WriteReg
//功能：    写寄存器数据
//输入参数：LCD_Reg:寄存器地址
//			LCD_RegValue:要写入的数据
//返回值：  无
//******************************************************************
void LCD_WriteReg(uint16 LCD_Reg, uint16 LCD_RegValue)
{
	LCD_WR_REG(LCD_Reg);
	LCD_WR_DATA(LCD_RegValue);
}

//******************************************************************
//函数名：  LCD_WriteRAM_Prepare
//功能：    开始写GRAM
//			在给液晶屏传送RGB数据前，应该发送写GRAM指令
//输入参数：无
//返回值：  无
//******************************************************************
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.wramcmd);
}

//******************************************************************
//函数名：  LCD_Reset
//功能：    LCD复位函数，液晶初始化前要调用此函数
//输入参数：无
//返回值：  无
//******************************************************************
void LCD_RESET(void)
{
	LCD_RST_CLR;
	DELAY_MS(100);
	LCD_RST_SET;
	DELAY_MS(100);
}

//******************************************************************
//函数名：  LCD_GPIOInit
//功能：    液晶屏IO初始化，液晶初始化前要调用此函数
//输入参数：无
//返回值：  无
//******************************************************************
void LCD_GPIOInit(void)
{
    gpio_init (PTD3, GPO,0);
    gpio_init (PTD4, GPO,0);
    gpio_init (PTD5, GPO,0);
    gpio_init (PTD6, GPO,0);
    gpio_init (PTD7, GPO,0);
    //gpio_init (PTD8, GPO,0);

}

//******************************************************************
//函数名：  LCD_Init
//功能：    LCD初始化
//输入参数：无
//返回值：  无
//******************************************************************
void LCD_Init(void)
{

//#if USE_HARDWARE_SPI //使用硬件SPI
	//SPI2_Init();
//#else
	LCD_GPIOInit();//使用模拟SPI
//#endif

 	LCD_RESET(); //液晶屏复位

	//************* Start Initial Sequence **********//
	//开始初始化液晶屏
	LCD_WR_REG(0x11);//Sleep exit
	DELAY_MS(120);
	//ST7735R Frame Rate
	LCD_WR_REG(0xB1);
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x2C);
	LCD_WR_DATA(0x2D);

	LCD_WR_REG(0xB2);
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x2C);
	LCD_WR_DATA(0x2D);

	LCD_WR_REG(0xB3);
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x2C);
	LCD_WR_DATA(0x2D);
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x2C);
	LCD_WR_DATA(0x2D);

	LCD_WR_REG(0xB4); //Column inversion
	LCD_WR_DATA(0x07);

	//ST7735R Power Sequence
	LCD_WR_REG(0xC0);
	LCD_WR_DATA(0xA2);
	LCD_WR_DATA(0x02);
	LCD_WR_DATA(0x84);
	LCD_WR_REG(0xC1);
	LCD_WR_DATA(0xC5);

	LCD_WR_REG(0xC2);
	LCD_WR_DATA(0x0A);
	LCD_WR_DATA(0x00);

	LCD_WR_REG(0xC3);
	LCD_WR_DATA(0x8A);
	LCD_WR_DATA(0x2A);
	LCD_WR_REG(0xC4);
	LCD_WR_DATA(0x8A);
	LCD_WR_DATA(0xEE);

	LCD_WR_REG(0xC5); //VCOM
	LCD_WR_DATA(0x0E);

	LCD_WR_REG(0x36); //MX, MY, RGB mode
	LCD_WR_DATA(0xC8);

	//ST7735R Gamma Sequence
	LCD_WR_REG(0xe0);
	LCD_WR_DATA(0x0f);
	LCD_WR_DATA(0x1a);
	LCD_WR_DATA(0x0f);
	LCD_WR_DATA(0x18);
	LCD_WR_DATA(0x2f);
	LCD_WR_DATA(0x28);
	LCD_WR_DATA(0x20);
	LCD_WR_DATA(0x22);
	LCD_WR_DATA(0x1f);
	LCD_WR_DATA(0x1b);
	LCD_WR_DATA(0x23);
	LCD_WR_DATA(0x37);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x07);
	LCD_WR_DATA(0x02);
	LCD_WR_DATA(0x10);

	LCD_WR_REG(0xe1);
	LCD_WR_DATA(0x0f);
	LCD_WR_DATA(0x1b);
	LCD_WR_DATA(0x0f);
	LCD_WR_DATA(0x17);
	LCD_WR_DATA(0x33);
	LCD_WR_DATA(0x2c);
	LCD_WR_DATA(0x29);
	LCD_WR_DATA(0x2e);
	LCD_WR_DATA(0x30);
	LCD_WR_DATA(0x30);
	LCD_WR_DATA(0x39);
	LCD_WR_DATA(0x3f);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x07);
	LCD_WR_DATA(0x03);
	LCD_WR_DATA(0x10);

	LCD_WR_REG(0x2a);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x7f);

	LCD_WR_REG(0x2b);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x9f);

	LCD_WR_REG(0xF0); //Enable test command
	LCD_WR_DATA(0x01);
	LCD_WR_REG(0xF6); //Disable ram power save mode
	LCD_WR_DATA(0x00);

	LCD_WR_REG(0x3A); //65k mode
	LCD_WR_DATA(0x05);
	LCD_WR_REG(0x29);//Display on

	LCD_SetParam();//设置LCD参数  横竖屏模式切换
	//LCD_LED_SET;//点亮背光
	//LCD_Clear(WHITE);
}

/*******************************************************************
//函数名：  LCD_Clear
//功能：    LCD全屏填充清屏函数
//输入参数：Color:要清屏的填充色
//返回值：  无
//备注：		已校准
8******************************************************************/
void LCD_Clear(uint16 Color)
{
	uint16 i,j;
	//LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);
	LCD_SetWindows(0-2,29,128-1-2,128-1+29);
	for(i=0;i<lcddev.width;i++)
	{
		for(j=0;j<lcddev.height;j++)
		LCD_WR_DATA_16Bit(Color);	//写入数据
	}
}
/*************************************************
函数名：LCD_SetWindows
功能：设置lcd显示窗口，在此区域写点数据自动换行
入口参数：xy起点和终点
返回值：无
说明：LCD_SetWindows(0-2,0+29,lcddev.width-1-2,lcddev.height-1+29);//恢复窗口设置为全屏
*************************************************/
void LCD_SetWindows(int16 xStar, int16 yStar,int16 xEnd,int16 yEnd)
{
#if USE_HORIZONTAL==1	//使用横屏
	LCD_WR_REG(lcddev.setxcmd);
	LCD_WR_DATA(xStar>>8);
	LCD_WR_DATA(0x00FF&xStar+3);
	LCD_WR_DATA(xEnd>>8);
	LCD_WR_DATA(0x00FF&xEnd+3);

	LCD_WR_REG(lcddev.setycmd);
	LCD_WR_DATA(yStar>>8);
	LCD_WR_DATA(0x00FF&yStar+2);
	LCD_WR_DATA(yEnd>>8);
	LCD_WR_DATA(0x00FF&yEnd+2);

#else

	LCD_WR_REG(lcddev.setxcmd);
	LCD_WR_DATA(xStar>>8);
	LCD_WR_DATA(0x00FF&xStar+2);
	LCD_WR_DATA(xEnd>>8);
	LCD_WR_DATA(0x00FF&xEnd+2);

	LCD_WR_REG(lcddev.setycmd);
	LCD_WR_DATA(yStar>>8);
	LCD_WR_DATA(0x00FF&yStar+3);
	LCD_WR_DATA(yEnd>>8);
	LCD_WR_DATA(0x00FF&yEnd+3);
#endif

	LCD_WriteRAM_Prepare();	//开始写入GRAM
}

/*******************************************************************
函数名：LCD_SetParam
功能：设置LCD参数 方便进行横竖屏模式切换
入口参数：无
返回值：无
********************************************************************/
void LCD_SetParam(void)
{
	lcddev.wramcmd=0x2C;
#if USE_HORIZONTAL==1	//使用横屏
	lcddev.dir=1;//横屏
	lcddev.width=128;
	lcddev.height=128;
	lcddev.setxcmd=0x2A;
	lcddev.setycmd=0x2B;
	LCD_WriteReg(0x36,0xA8);

#else//竖屏
	lcddev.dir=0;//竖屏
	lcddev.width=128;
	lcddev.height=128;
	lcddev.setxcmd=0x2A;
	lcddev.setycmd=0x2B;
	LCD_WriteReg(0x36,0xC8);
#endif
        
}
/******************************************************************
函数名：LCD_SetCursor
功能：设置光标位置
入口参数：xy坐标
返回值：无
备注：	已校准
******************************************************************/
void LCD_SetCursor(uint16 Xpos, uint16 Ypos)
{
	//LCD_SetWindows(Xpos,Ypos,Xpos,Ypos);
	LCD_SetWindows(Xpos-2,Ypos+29,Xpos-2,Ypos+29);
}
/*******************************************************************
//函数名：  LCD_DrawPoint
//功能：    在指定位置写入一个像素点数据
//输入参数：(x,y):光标坐标
//返回值：  无
//备注： 	已校准  颜色为画笔颜色黑色，颜色可修改
*******************************************************************/
void LCD_DrawPoint(uint16 x,uint16 y)
{
	LCD_SetCursor(x,y);//设置光标位置
	LCD_WR_DATA_16Bit(POINT_COLOR);
}



//分界线，以上为基本代码，下面为GUI应用
//*********************************************************************************/



/*******************************************************************
//函数名：  GUI_DrawPoint
//日期：    2013-02-22
//功能：    GUI描绘一个点
//输入参数：x:光标位置x坐标
//          y:光标位置y坐标
//	    color:要填充的颜色
//备注：		已校准
**8*****************************************************************/
void GUI_DrawPoint(uint16 x,uint16 y,uint16 color)
{
	LCD_SetCursor(x,y);//设置光标位置
	LCD_WR_DATA_16Bit(color);
}

/*******************************************************************
//函数名：  LCD_Fill
//功能：    在指定区域内填充颜色
//输入参数：sx:指定区域开始点x坐标
//          sy:指定区域开始点y坐标
//	    ex:指定区域结束点x坐标
//	    ey:指定区域结束点y坐标
//        	color:要填充的颜色
//返回值：  无
//备注：		已校准
*******************************************************************/
void LCD_Fill(uint16 sx,uint16 sy,uint16 ex,uint16 ey,uint16 color)
{

	uint16 i,j;
	uint16 width=ex-sx+1; 		//得到填充的宽度
	uint16 height=ey-sy+1;		//高度
	//LCD_SetWindows(sx,sy,ex-1,ey-1);//设置显示窗口
	LCD_SetWindows(sx-2,sy+29,ex-1-2,ey-1+29);//设置显示窗口
	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
		LCD_WR_DATA_16Bit(color);	//写入数据
	}

	LCD_SetWindows(0-2,0+29,lcddev.width-1-2,lcddev.height-1+29);//恢复窗口设置为全屏
}

/******************************************************************
//函数名：  LCD_DrawLine
//功能：    GUI画线
//输入参数：x1,y1:起点坐标
//        	x2,y2:终点坐标
//返回值：  无
//备注： 	已校准
8*****************************************************************/
void LCD_DrawLine(uint16 x1, uint16 y1, uint16 x2, uint16 y2)
{
	uint16 t;
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;

	delta_x=x2-x1; //计算坐标增量
	delta_y=y2-y1;
	uRow=x1;
	uCol=y1;
	if(delta_x>0)incx=1; //设置单步方向
	else if(delta_x==0)incx=0;//垂直线
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if(delta_y==0)incy=0;//水平线
	else{incy=-1;delta_y=-delta_y;}
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴
	else distance=delta_y;
	for(t=0;t<=distance+1;t++ )//画线输出
	{
		LCD_DrawPoint(uRow,uCol);//画点
		xerr+=delta_x ;
		yerr+=delta_y ;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}

/*******************************************************************
//函数名：  LCD_DrawRectangle
//功能：    GUI画矩形(非填充)
//输入参数：(x1,y1),(x2,y2):矩形的对角坐标
//返回值：  无   验证正确
//说明：		已校准
********************************************************************/
void LCD_DrawRectangle(uint16 x1, uint16 y1, uint16 x2, uint16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}

/*******************************************************************
//函数名：  LCD_DrawFillRectangle
//功能：    GUI画矩形(填充)
//输入参数：(x1,y1),(x2,y2):矩形的对角坐标
//返回值：  无
//说明：		已校准
*******************************************************************/
void LCD_DrawFillRectangle(uint16 x1, uint16 y1, uint16 x2, uint16 y2)
{
	LCD_Fill(x1,y1,x2,y2,POINT_COLOR);

}
/*******************************************************************
//函数名：  _draw_circle_8
//功能：    8对称性画圆算法(内部调用)
//输入参数：(xc,yc) :圆中心坐标
// 			(x,y):光标相对于圆心的坐标
//         	c:填充的颜色
//返回值：  无
*******************************************************************/
void _draw_circle_8(int xc, int yc, int x, int y, uint16 c)
{
	GUI_DrawPoint(xc + x, yc + y, c);

	GUI_DrawPoint(xc - x, yc + y, c);

	GUI_DrawPoint(xc + x, yc - y, c);

	GUI_DrawPoint(xc - x, yc - y, c);

	GUI_DrawPoint(xc + y, yc + x, c);

	GUI_DrawPoint(xc - y, yc + x, c);

	GUI_DrawPoint(xc + y, yc - x, c);

	GUI_DrawPoint(xc - y, yc - x, c);
}

/*******************************************************************
//函数名：  gui_circle
//功能：    在指定位置画一个指定大小的圆(填充)
//输入参数：(xc,yc) :圆中心坐标
//           c:填充的颜色
//	     r:圆半径
//	     fill:填充判断标志，1-填充，0-不填充
//返回值：  无
//说明：		已校准
*******************************************************************/
void gui_circle(int xc, int yc,uint16 c,int r, int fill)
{
	int x = 0, y = r, yi, d;

	d = 3 - 2 * r;


	if (fill)
	{
		// 如果填充（画实心圆）
		while (x <= y) {
			for (yi = x; yi <= y; yi++)
				_draw_circle_8(xc, yc, x, yi, c);

			if (d < 0) {
				d = d + 4 * x + 6;
			} else {
				d = d + 4 * (x - y) + 10;
				y--;
			}
			x++;
		}
	} else
	{
		// 如果不填充（画空心圆）
		while (x <= y) {
			_draw_circle_8(xc, yc, x, y, c);
			if (d < 0) {
				d = d + 4 * x + 6;
			} else {
				d = d + 4 * (x - y) + 10;
				y--;
			}
			x++;
		}
	}
}

/*******************************************************************
//函数名：  LCD_ShowChar
//功能：    显示单个英文字符
//输入参数：(x,y):字符显示位置起始坐标
//        	fc:前置画笔颜色
//		bc:背景颜色
//		num:数值（0-94）
//		size:字体大小
//		mode:模式  0,填充模式;1,叠加模式
//返回值：  无
********************************************************************/
void LCD_ShowChar(uint16 x,uint16 y,uint16 fc, uint16 bc, uint8 num,uint8 size,uint8 mode)
{
    uint8 temp;
    uint8 pos,t;
	uint16 colortemp=POINT_COLOR;

	num=num-' ';//得到偏移后的值
	LCD_SetWindows(x,y,x+size/2-1,y+size-1);//设置单个文字显示窗口
	if(!mode) //非叠加方式
	{

		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=asc2_1206[num][pos];//调用1206字体
			else temp=asc2_1608[num][pos];		 //调用1608字体
			for(t=0;t<size/2;t++)
		    {
		        if(temp&0x01)LCD_WR_DATA(fc);
				else LCD_WR_DATA(bc);
				temp>>=1;

		    }

		}
	}else//叠加方式
	{
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=asc2_1206[num][pos];//调用1206字体
			else temp=asc2_1608[num][pos];		 //调用1608字体
			for(t=0;t<size/2;t++)
		    {
				POINT_COLOR=fc;
		        if(temp&0x01)LCD_DrawPoint(x+t,y+pos);//画一个点
		        temp>>=1;
		    }
		}
	}
	POINT_COLOR=colortemp;
	LCD_SetWindows(0-2,0+29,lcddev.width-1-2,lcddev.height-1+29);//恢复窗口设置为全屏
}

/*******************************************************************
//函数名：  LCD_ShowString
//功能：    显示英文字符串
//输入参数：x,y :起点坐标
//			size:字体大小
//			*p:字符串起始地址
//			mode:模式	0,填充模式;1,叠加模式
//返回值：  无
********************************************************************/
void LCD_ShowString(uint16 x,uint16 y,uint8 size,uint8 *p,uint8 mode)
{
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {
		if(x>(lcddev.width-1)||y>(lcddev.height-1))
		return;
        LCD_ShowChar(x,y,POINT_COLOR,BACK_COLOR,*p,size,mode);
        x+=size/2;
        p++;
    }
}

/*******************************************************************
//函数名：  LCD_ShowNum2412
//功能：    
//输入参数：(x,y):字符显示位置起始坐标
//        	fc:前置画笔颜色
//			bc:背景颜色
//			num:数值（0-94）
//			size:字体大小
//			mode:模式  0,填充模式;1,叠加模式
//返回值：  无
********************************************************************/
void LCD_ShowNum2412(uint16 x,uint16 y,uint16 fc, uint16 bc,uint8 *p ,uint8 size,uint8 mode)
{
    uint16 temp;
    uint8 pos,t;
	uint16 colortemp=POINT_COLOR;
	uint16 x0=x;
	uint16 y0=y;
	uint8 num=0;


    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {

		if(x>(lcddev.width-1)||y>(lcddev.height-1))
		return;
		num=*p;
		if(':'==num) //特殊字符":"
		num=10;
		else if('.'==num)//特殊字符"."
		num=11;
		else  //纯数字
		num=num-'0';
		x0=x;

		for(pos=0;pos<48;pos++)
		{
			temp=asc2_2412[num][pos];
			for(t=0;t<8;t++)
		    {
				POINT_COLOR=fc;
		        if(temp&0x80)LCD_DrawPoint(x,y);//画一个点
				//else LCD_WR_DATA_16Bit(bc);
		        temp<<=1;
				x++;
				if((x-x0)==12)
				{
					x=x0;
					y++;
					break;
				}
		    }
		}
	if(num<10)
	x+=16;	//人为控制字距，使得排版更好看，原值为12
	else
	x+=8; //人为控制字距，使得排版更好看，原值为12

	y=y0;
    p++;
    }
	POINT_COLOR=colortemp;
}


/*******************************************************************
//函数名：  mypow
//功能：    求m的n次方(gui内部调用)
//输入参数：m:乘数
//	        n:幂
//返回值：  m的n次方
********************************************************************/
uint32 mypow(uint8 m,uint8 n)
{
	uint32 result=1;
	while(n--)result*=m;
	return result;
}

/*******************************************************************
//函数名：  LCD_ShowNum
//功能：    显示单个数字变量值
//输入参数：x,y :起点坐标
//			len :指定显示数字的位数
//			size:字体大小(12,16)
//			color:颜色
//			num:数值(0~4294967295)
//返回值：  无
********************************************************************/
void LCD_ShowNum(uint16 x,uint16 y,uint32 num,uint8 len,uint8 size)
{
	uint8 t,temp;
	uint8 enshow=0;
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,POINT_COLOR,BACK_COLOR,' ',size,1);
				continue;
			}else enshow=1;

		}
	 	LCD_ShowChar(x+(size/2)*t,y,POINT_COLOR,BACK_COLOR,temp+'0',size,1);
	}
}


/*******************************************************************
//函数名：  GUI_DrawFont16
//功能：    显示单个16X16中文字体
//输入参数：x,y :起点坐标
//			fc:前置画笔颜色
//			bc:背景颜色
//			s:字符串地址
//			mode:模式	0,填充模式;1,叠加模式
//返回值：  无
********************************************************************/
void GUI_DrawFont16(uint16 x, uint16 y, uint16 fc, uint16 bc, uint8 *s,uint8 mode)
{
	uint8 i,j;
	uint16 k;
	uint16 HZnum;
	uint16 x0=x;
	HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);	//自动统计汉字数目


	for (k=0;k<HZnum;k++)
	{
	  if ((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1)))
	  { 	LCD_SetWindows(x,y,x+16-1,y+16-1);
		    for(i=0;i<16*2;i++)
		    {
				for(j=0;j<8;j++)
		    	{
					if(!mode) //非叠加方式
					{
						if(tfont16[k].Msk[i]&(0x80>>j))	LCD_WR_DATA_16Bit(fc);
						else LCD_WR_DATA_16Bit(bc);
					}
					else
					{
						POINT_COLOR=fc;
						if(tfont16[k].Msk[i]&(0x80>>j))	LCD_DrawPoint(x,y);//画一个点
						x++;
						if((x-x0)==16)
						{
							x=x0;
							y++;
							break;
						}
					}

				}

			}


		}
		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}

	LCD_SetWindows(0-2,0+29,lcddev.width-1-2,lcddev.height-1+29);//恢复窗口设置为全屏
}

/*******************************************************************
//函数名：  GUI_DrawFont24
//功能：    显示单个24X24中文字体
//输入参数：x,y :起点坐标
//			fc:前置画笔颜色
//			bc:背景颜色
//			s:字符串地址
//			mode:模式	0,填充模式;1,叠加模式
//返回值：  无
********************************************************************/
void GUI_DrawFont24(uint16 x, uint16 y, uint16 fc, uint16 bc, uint8 *s,uint8 mode)
{
	uint8 i,j;
	uint16 k;
	uint16 HZnum;
	uint16 x0=x;
	HZnum=sizeof(tfont24)/sizeof(typFNT_GB24);	//自动统计汉字数目

			for (k=0;k<HZnum;k++)
			{
			  if ((tfont24[k].Index[0]==*(s))&&(tfont24[k].Index[1]==*(s+1)))
			  { 	LCD_SetWindows(x,y,x+24-1,y+24-1);
				    for(i=0;i<24*3;i++)
				    {
							for(j=0;j<8;j++)
							{
								if(!mode) //非叠加方式
								{
									if(tfont24[k].Msk[i]&(0x80>>j))	LCD_WR_DATA_16Bit(fc);
									else LCD_WR_DATA_16Bit(bc);
								}
							else
							{
								POINT_COLOR=fc;
								if(tfont24[k].Msk[i]&(0x80>>j))	LCD_DrawPoint(x,y);//画一个点
								x++;
								if((x-x0)==24)
								{
									x=x0;
									y++;
									break;
								}
							}
						}
					}


				}
				continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
			}

	LCD_SetWindows(0-2,0+29,lcddev.width-1-2,lcddev.height-1+29);//恢复窗口设置为全屏
}

/*******************************************************************
//函数名：  GUI_DrawFont32
//功能：    显示单个32X32中文字体
//输入参数：x,y :起点坐标
//			fc:前置画笔颜色
//			bc:背景颜色
//			s:字符串地址
//			mode:模式	0,填充模式;1,叠加模式
//返回值：  无
********************************************************************/
void GUI_DrawFont32(uint16 x, uint16 y, uint16 fc, uint16 bc, uint8 *s,uint8 mode)
{
	uint8 i,j;
	uint16 k;
	uint16 HZnum;
	uint16 x0=x;
	HZnum=sizeof(tfont32)/sizeof(typFNT_GB32);	//自动统计汉字数目
	for (k=0;k<HZnum;k++)
	{
	  if ((tfont32[k].Index[0]==*(s))&&(tfont32[k].Index[1]==*(s+1)))
	  {
			LCD_SetWindows(x,y,x+32-1,y+32-1);
			for(i=0;i<32*4;i++)
			{
				for(j=0;j<8;j++)
				{
					if(!mode) //非叠加方式
					{
						if(tfont32[k].Msk[i]&(0x80>>j))	LCD_WR_DATA_16Bit(fc);
						else LCD_WR_DATA_16Bit(bc);
					}
					else
					{
						POINT_COLOR=fc;
						if(tfont32[k].Msk[i]&(0x80>>j))	LCD_DrawPoint(x,y);//画一个点
						x++;
						if((x-x0)==32)
						{
							x=x0;
							y++;
							break;
						}
					}
				}
			}
		}
		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}

	LCD_SetWindows(0-2,0+29,lcddev.width-1-2,lcddev.height-1+29);//恢复窗口设置为全屏
}

/*******************************************************************
//函数名：  Show_Str
//功能：    显示一个字符串,包含中英文显示
//输入参数：x,y :起点坐标
// 			fc:前置画笔颜色
//			bc:背景颜色
//			str :字符串
//			size:字体大小
//			mode:模式	0,填充模式;1,叠加模式
//返回值：  无
8*******************************************************************/
void Show_Str(uint16 x, uint16 y, uint16 fc, uint16 bc, uint8 *str,uint8 size,uint8 mode)
{
	uint16 x0=x;
  	uint8 bHz=0;     //字符或者中文
    while(*str!=0)//数据未结束
    {
        if(!bHz)
        {
			if(x>(lcddev.width-size/2)||y>(lcddev.height-size))
			return;
	        if(*str>0x80)bHz=1;//中文
	        else              //字符
	        {
		        if(*str==0x0D)//换行符号
		        {
		            y+=size;
					x=x0;
		            str++;
		        }
		        else
				{
					if(size==12||size==16)
					{
					LCD_ShowChar(x,y,fc,bc,*str,size,mode);
					x+=size/2; //字符,为全字的一半
					}
					else//字库中没有集成16X32的英文字体,用8X16代替
					{
					 	LCD_ShowChar(x,y,fc,bc,*str,16,mode);
						x+=8; //字符,为全字的一半
					}
				}
				str++;

	        }
        }
		else//中文
        {
			if(x>(lcddev.width-size)||y>(lcddev.height-size))
			return;
            bHz=0;//有汉字库
			if(size==32)
			GUI_DrawFont32(x,y,fc,bc,str,mode);
			else if(size==24)
			GUI_DrawFont24(x,y,fc,bc,str,mode);
			else
			GUI_DrawFont16(x,y,fc,bc,str,mode);

	        str+=2;
	        x+=size;//下一个汉字偏移
        }
    }
}

/*******************************************************************
//函数名：  Gui_StrCenter
//功能：    居中显示一个字符串,包含中英文显示
//输入参数：x,y :起点坐标
// 			fc:前置画笔颜色
//			bc:背景颜色
//			str :字符串
//			size:字体大小
//			mode:模式	0,填充模式;1,叠加模式
//返回值：  无
*******************************************************************/
void Gui_StrCenter(uint16 x, uint16 y, uint16 fc, uint16 bc, uint8 *str,uint8 size,uint8 mode)
{
	uint16 len=strlen((const char *)str);
	uint16 x1=(lcddev.width-len*8)/2;
	Show_Str(x+x1,y,fc,bc,str,size,mode);
}

/*******************************************************************
//函数名：  Gui_Drawbmp16
//功能：    显示一副16位BMP图像
//输入参数：x,y :起点坐标
// 			*p :图像数组起始地址
//返回值：  无
********************************************************************/
void Gui_Drawbmp16(uint16 x,uint16 y,const unsigned char *p) //显示40*40 QQ图片
{
  	int i;
	unsigned char picH,picL;
	LCD_SetWindows(x,y,x+40-1,y+40-1);//窗口设置
    for(i=0;i<40*40;i++)
	{
	 	picL=*(p+i*2);	//数据低位在前
		picH=*(p+i*2+1);
		LCD_WR_DATA_16Bit(picH<<8|picL);
	}
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//恢复显示窗口为全屏

}


//新修改，可用
void LCD_char(uint16 x,uint16 y, uint8 num, uint8 size,uint16 Color, uint16 bkColor)
{
#define MAX_CHAR_POSX (240-8)
#define MAX_CHAR_POSY (320-16)

    uint8 temp, t, pos;    
    if(x > MAX_CHAR_POSX || y > MAX_CHAR_POSY)
    {
        return;
    }
    num=num-' ';//得到偏移后的值
    LCD_SetWindows(x, y,x+8,y+16);
    //LCD_RAMWR();                    //写内存

    for (pos = 0; pos < size; pos++)
    {
        if(size==12)temp=asc2_1206[num][pos];//调用1206字体
        else temp=asc2_1608[num][pos];       //调用1608字体
        
        for(t = 0; t < size/2; t++)
        {
            
            if(temp & 0x01)
            {
                POINT_COLOR=Color;
                LCD_DrawPoint(x+t,y+pos);
            }
            else
            {
              POINT_COLOR=bkColor;
              LCD_DrawPoint(x+t,y+pos);
            }
            temp >>= 1;
          
        }
    }
    return;
#undef MAX_CHAR_POSX
#undef MAX_CHAR_POSY
}
//新LCD显示数字  数字可刷新
//功能：    显示单个数字变量值
//输入参数：x,y :起点坐标
//          num：数字变量
//          size:字体大小(12,16)
//	    color:颜色
//	    num:数值(0~4294967295)
//返回值：  无
void LCD_Num(uint16 x,uint16 y, uint32 num, uint8 size, uint16 Color, uint16 bkColor)
{
    uint32 res = num;
    uint8 t = 0;
    uint16 x0=x;
    uint16 y0=y;
      
    if( num == 0 )
    {
        LCD_char(x,y,'0',size,Color,bkColor);
        //LCD_ShowChar(x,y,Color,bkColor,'0',16,1);
        return;
    }
    while( res )  /*得到数字长度t*/
    {
        res /= 10;
        t++;
    }

    while(num)
    {
        x0 = x + (8 * (t--) - 8);
        LCD_char(x0,y0,(num % 10 +'0'),size,Color,bkColor);
        //LCD_ShowChar(x0,y0,Color,bkColor,(num % 10) + '0',16,1);
        num /= 10 ;
    }
}
//新LCD显示数字  数字可刷新 (清空多余的位）
//功能：    显示单个数字变量值
//输入参数：x,y :起点坐标
//          num：数字变量
//          size:字体大小(12,16)
//          len: 数字长度
//	    color:颜色
//	    num:数值(0~4294967295)
//返回值：  无
void LCD_Num_BC(uint16 x,uint16 y, uint32 num, uint8 size, uint8 len,uint16 Color, uint16 bkColor)
{
    uint32 res = num;
    uint8 t = 0;
    //Site_t sitetemp;
    //Size_t size;
    uint16 x0=x;
    uint16 y0=y;
    uint16 h;
    uint16 w;
    if( num == 0 )
    {
        //LCD_char(site, '0', Color, bkColor);
        LCD_char(x,y,'0',size,Color,bkColor);
        x += 8;
        //size.H  = 16;
        //size.W  = 8 * (len - 1);
        //LCD_rectangle(site, size, bkColor);
        h  = 16;
        w  = 8 * (len - 1);
        LCD_Fill(x,y,x+h,y+w,bkColor);
        return;
    }
    while( res )            /*得到数字长度t*/
    {
        res /= 10;
        t++;
    }
    if(t >= len )    //限制最大长度
    {
        t = len;
    }

    res = t;

    while( t != 0 )
    {
        //sitetemp.x = site.x + (8 * (t--) - 8);
        //LCD_char(sitetemp, (num % 10) + '0', Color, bkColor);
        //num /= 10 ;
        x0 = x + (8 * (t--) - 8);
        LCD_char(x0,y0,(num % 10 +'0'),size,Color,bkColor);
        num /= 10 ;
    }

    if(res != len )
    {
        //size.W = 8 * (max_num_bit - res);
        //site.x += (8 * res);
        //size.H  = 16;
        //LCD_rectangle(site, size, bkColor);
        h  = 16;
        w = 8 * (len - res);
        x += (8 * res);
        LCD_Fill(x,y,x+h,x+w,bkColor);
    }
}


