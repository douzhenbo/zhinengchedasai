#ifndef __LCD_H
#define __LCD_H

/****************************************************************************************************
//=======================================液晶屏数据线接线==========================================//
//CS		接PTA15	//片选信号
//SCK	    接PTA7	//SPI时钟信号
//SDI(DIN)	接PTA	//SPI总线数据信号
//=======================================液晶屏控制线接线==========================================//
//LED	    接PTA5	//背光控制信号，高电平点亮
//RS(D/C)   接PTA	//寄存器/数据选择信号(RS=0数据总线发送的是指令；RS=1数据总线发送的是像素数据)
//RST	    接PTA13	//液晶屏复位信号，低电平复位
/////////////////////////////////////////////////////////////////////////////////////////////////
//==================================如何精简到只需要3个IO=======================================//
//1.CS信号可以精简，不作SPI复用片选可将CS接地常低，节省1个IO
//2.LED背光控制信号可以接高电平3.3V背光常亮，节省1个IO
//3.RST复位信号可以接到单片机的复位端，利用系统上电复位，节省1个IO
//==================================如何切换横竖屏显示=======================================//
//lcd.h头文件，修改宏#define USE_HORIZONTAL 值为0使用竖屏模式.1,使用横屏模式
//===========================如何切换模拟SPI总线驱动和硬件SPI总线驱动=========================//
//lcd.h头文件，修改宏#define USE_HARDWARE_SPI  值为0使用模拟SPI总线.1,使用硬件SPI总线
**************************************************************************************************/

//LCD结构体
typedef struct
{
	uint16 width;			//LCD 宽度
	uint16 height;			//LCD 高度
	uint16 id;				//LCD ID
	uint8  dir;			//横屏还是竖屏控制：0，竖屏；1，横屏。
	uint16	 wramcmd;		//开始写gram指令
	uint16  setxcmd;		//设置x坐标指令
	uint16  setycmd;		//设置y坐标指令
}_lcd_dev;

//LCD参数
extern _lcd_dev lcddev;	//管理LCD重要参数
/*****************************  用户配置区  ***********************************/
//支持横竖屏快速定义切换，支持8/16位模式切换
#define USE_HORIZONTAL  	0	//定义是否使用横屏  0,不使用.1,使用.
//使用硬件SPI 还是模拟SPI作为测试
#define USE_HARDWARE_SPI    0  // 1:使能硬件SPI;  0:使用模拟SPI   注意：只使用模拟SPI 硬件代码被注释
/******************************************************************************/
//定义LCD的尺寸
//#if ( (USE_HORIZONTAL&1) == 1 )	//使用横屏
//#define LCD_W 320
//#define LCD_H 240
//#else
//#define LCD_W 240
//#define LCD_H 320
//#endif
//x为LCD_W  y为LCD_H


//TFTLCD部分外要调用的函数
extern uint16  POINT_COLOR;//默认红色
extern uint16  BACK_COLOR; //背景颜色.默认为白色

////////////////////////////////////////////////////////////////////
//-----------------LCD端口定义----------------
//QDtech全系列模块采用了三极管控制背光亮灭，用户也可以接PWM调节背光亮度
//#define LCD_LED        	PTE8 //PB9 连接至TFT -LED
//接口定义在Lcd_Driver.h内定义，请根据接线修改并修改相应IO初始化LCD_GPIO_Init()

#define LCD_RS         	PTD3	//PB10连接至TFT --AO
#define LCD_CS        	PTD4    //PB11 连接至TFT --CS
#define LCD_RST     	PTD7	//PB12连接至TFT --RST
#define LCD_SCL        	PTD5	//PB13连接至TFT -- CLK
#define LCD_SDA        	PTD6	//PB15连接至TFT - SDI

//////////////////////////////////////////////////////////////////////
//液晶控制口置1操作语句宏定义
#define	LCD_CS_SET  	gpio_set(LCD_CS,1)
#define	LCD_RS_SET  	gpio_set(LCD_RS,1)
#define	LCD_SDA_SET  	gpio_set(LCD_SDA,1)
#define	LCD_SCL_SET  	gpio_set(LCD_SCL,1)
#define	LCD_RST_SET  	gpio_set(LCD_RST,1)
#define	LCD_LED_SET  	gpio_set(LCD_LED,1)

//液晶控制口置0操作语句宏定义
#define	LCD_CS_CLR  	gpio_set(LCD_CS,0)
#define	LCD_RS_CLR  	gpio_set(LCD_RS,0)
#define	LCD_SDA_CLR  	gpio_set(LCD_SDA,0)
#define	LCD_SCL_CLR  	gpio_set(LCD_SCL,0)
#define	LCD_RST_CLR  	gpio_set(LCD_RST,0)
#define	LCD_LED_CLR  	gpio_set(LCD_LED,0)

//画笔颜色
#define LCD_WHITE       0xFFFF
#define LCD_BLACK       0x0000
#define LCD_BLUE        0x001F
#define LCD_BRED        0XF81F
#define LCD_GRED 	0XFFE0
#define LCD_GBLUE	0X07FF
#define LCD_RED         0xF800
#define LCD_MAGENTA     0xF81F
#define LCD_GREEN       0x07E0
#define LCD_CYAN        0x7FFF
#define LCD_YELLOW      0xFFE0
#define LCD_BROWN 	0XBC40 //棕色
#define LCD_BRRED       0XFC07 //棕红色
#define LCD_GRAY  	0X8430 //灰色
//GUI颜色
#define LCD_DARKBLUE    0X01CF	//深蓝色
#define LCD_LIGHTBLUE   0X7D7C	//浅蓝色
#define LCD_GRAYBLUE    0X5458 //灰蓝色
//以上三色为PANEL的颜色
#define LCD_LIGHTGREEN  0X841F //浅绿色
#define LCD_LIGHTGRAY   0XEF5B //浅灰色(PANNEL)
#define LCD_LGRAY 	0XC618 //浅灰色(PANNEL),窗体背景色
#define LCD_LGRAYBLUE   0XA651 //浅灰蓝色(中间层颜色)
#define LCD_LBBLUE      0X2B12 //浅棕蓝色(选择条目的反色)

//显示解压后img图像所用
#define GRAY_2_RGB565(gray)             ((uint16)((((uint8)(gray)>>3)<<11)|(((uint8)(gray)>>2)<<5)|((uint8)(gray)>>3)))

extern uint16 BACK_COLOR, POINT_COLOR ;

void LCD_Init(void);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);

void LCD_Clear(uint16 Color); //清屏   LCD全屏填充清屏函数
void LCD_SetCursor(uint16 Xpos, uint16 Ypos);  //设置光标位置
void LCD_SetWindows(int16 xStar, int16 yStar,int16 xEnd,int16 yEnd);  //设置lcd显示窗口，在此区域写点数据自动换行

void LCD_DrawPoint(uint16 x,uint16 y);   //画点
void GUI_DrawPoint(uint16 x,uint16 y,uint16 color);//GUI描绘一个点
uint16 LCD_ReadPoint(uint16 x,uint16 y); //读点

void LCD_DrawLine(uint16 x1, uint16 y1, uint16 x2, uint16 y2);          //LCD画线 
void LCD_DrawRectangle(uint16 x1, uint16 y1, uint16 x2, uint16 y2);     //LCD画矩形(非填充)
void LCD_DrawFillRectangle(uint16 x1, uint16 y1, uint16 x2, uint16 y2); //LCD画矩形(填充)
void gui_circle(int xc, int yc,uint16 c,int r, int fill);    //在指定位置画一个指定大小的圆(fill=0 不填充 1 填充)

uint16 LCD_RD_DATA(void);//读取LCD数据
void LCD_WriteReg(uint16 LCD_Reg, uint16 LCD_RegValue);  //写寄存器数据
void LCD_WR_DATA(uint8 data);  //向液晶屏总线写入写8位数据
void LCD_WR_DATA_16Bit(uint16 data);  //8位总线下如何写入一个16位数据
void LCD_WriteRAM_Prepare(void);  //开始写GRAM
void LCD_SetParam(void);  //设置LCD参数 方便进行横竖屏模式切换

void LCD_Fill(uint16 sx,uint16 sy,uint16 ex,uint16 ey,uint16 color);  //在指定区域内填充颜色
void LCD_DrawLine(uint16 x1, uint16 y1, uint16 x2, uint16 y2);        //LCD画线
void Draw_Circle(uint16 x0,uint16 y0,uint16 fc,uint8 r);

void LCD_ShowChar(uint16 x,uint16 y,uint16 fc, uint16 bc, uint8 num,uint8 size,uint8 mode);  //显示单个英文字符
void LCD_ShowString(uint16 x,uint16 y,uint8 size,uint8 *p,uint8 mode);  //显示英文字符串

void LCD_ShowNum(uint16 x,uint16 y,uint32 num,uint8 len,uint8 size);  //显示单个数字变量值
void LCD_ShowNum2412(uint16 x,uint16 y,uint16 fc, uint16 bc,uint8 *p ,uint8 size,uint8 mode);
void LCD_Num_BC(uint16 x,uint16 y, uint32 num, uint8 size, uint8 len,uint16 Color, uint16 bkColor);
void LCD_Num(uint16 x,uint16 y, uint32 num, uint8 size, uint16 Color, uint16 bkColor);

void GUI_DrawFont16(uint16 x, uint16 y, uint16 fc, uint16 bc, uint8 *s,uint8 mode);  //显示单个16X16中文字体
void GUI_DrawFont24(uint16 x, uint16 y, uint16 fc, uint16 bc, uint8 *s,uint8 mode);  //显示单个24X24中文字体
void GUI_DrawFont32(uint16 x, uint16 y, uint16 fc, uint16 bc, uint8 *s,uint8 mode);  //显示单个32X32中文字体
void Show_Str(uint16 x, uint16 y, uint16 fc, uint16 bc, uint8 *str,uint8 size,uint8 mode);  //显示一个字符串,包含中英文显示
void Gui_StrCenter(uint16 x, uint16 y, uint16 fc, uint16 bc, uint8 *str,uint8 size,uint8 mode);  //居中显示一个字符串,包含中英文显示

void Gui_Drawbmp16(uint16 x,uint16 y,const unsigned char *p); //显示一副16位BMP图像  40*40大小



#endif


