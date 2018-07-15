#ifndef __LCD_H
#define __LCD_H

/****************************************************************************************************
//=======================================Һ���������߽���==========================================//
//CS		��PTA15	//Ƭѡ�ź�
//SCK	    ��PTA7	//SPIʱ���ź�
//SDI(DIN)	��PTA	//SPI���������ź�
//=======================================Һ���������߽���==========================================//
//LED	    ��PTA5	//��������źţ��ߵ�ƽ����
//RS(D/C)   ��PTA	//�Ĵ���/����ѡ���ź�(RS=0�������߷��͵���ָ�RS=1�������߷��͵�����������)
//RST	    ��PTA13	//Һ������λ�źţ��͵�ƽ��λ
/////////////////////////////////////////////////////////////////////////////////////////////////
//==================================��ξ���ֻ��Ҫ3��IO=======================================//
//1.CS�źſ��Ծ��򣬲���SPI����Ƭѡ�ɽ�CS�ӵس��ͣ���ʡ1��IO
//2.LED��������źſ��ԽӸߵ�ƽ3.3V���ⳣ������ʡ1��IO
//3.RST��λ�źſ��Խӵ���Ƭ���ĸ�λ�ˣ�����ϵͳ�ϵ縴λ����ʡ1��IO
//==================================����л���������ʾ=======================================//
//lcd.hͷ�ļ����޸ĺ�#define USE_HORIZONTAL ֵΪ0ʹ������ģʽ.1,ʹ�ú���ģʽ
//===========================����л�ģ��SPI����������Ӳ��SPI��������=========================//
//lcd.hͷ�ļ����޸ĺ�#define USE_HARDWARE_SPI  ֵΪ0ʹ��ģ��SPI����.1,ʹ��Ӳ��SPI����
**************************************************************************************************/

//LCD�ṹ��
typedef struct
{
	uint16 width;			//LCD ���
	uint16 height;			//LCD �߶�
	uint16 id;				//LCD ID
	uint8  dir;			//���������������ƣ�0��������1��������
	uint16	 wramcmd;		//��ʼдgramָ��
	uint16  setxcmd;		//����x����ָ��
	uint16  setycmd;		//����y����ָ��
}_lcd_dev;

//LCD����
extern _lcd_dev lcddev;	//����LCD��Ҫ����
/*****************************  �û�������  ***********************************/
//֧�ֺ��������ٶ����л���֧��8/16λģʽ�л�
#define USE_HORIZONTAL  	0	//�����Ƿ�ʹ�ú���  0,��ʹ��.1,ʹ��.
//ʹ��Ӳ��SPI ����ģ��SPI��Ϊ����
#define USE_HARDWARE_SPI    0  // 1:ʹ��Ӳ��SPI;  0:ʹ��ģ��SPI   ע�⣺ֻʹ��ģ��SPI Ӳ�����뱻ע��
/******************************************************************************/
//����LCD�ĳߴ�
//#if ( (USE_HORIZONTAL&1) == 1 )	//ʹ�ú���
//#define LCD_W 320
//#define LCD_H 240
//#else
//#define LCD_W 240
//#define LCD_H 320
//#endif
//xΪLCD_W  yΪLCD_H


//TFTLCD������Ҫ���õĺ���
extern uint16  POINT_COLOR;//Ĭ�Ϻ�ɫ
extern uint16  BACK_COLOR; //������ɫ.Ĭ��Ϊ��ɫ

////////////////////////////////////////////////////////////////////
//-----------------LCD�˿ڶ���----------------
//QDtechȫϵ��ģ������������ܿ��Ʊ��������û�Ҳ���Խ�PWM���ڱ�������
//#define LCD_LED        	PTE8 //PB9 ������TFT -LED
//�ӿڶ�����Lcd_Driver.h�ڶ��壬����ݽ����޸Ĳ��޸���ӦIO��ʼ��LCD_GPIO_Init()

#define LCD_RS         	PTD3	//PB10������TFT --AO
#define LCD_CS        	PTD4    //PB11 ������TFT --CS
#define LCD_RST     	PTD7	//PB12������TFT --RST
#define LCD_SCL        	PTD5	//PB13������TFT -- CLK
#define LCD_SDA        	PTD6	//PB15������TFT - SDI

//////////////////////////////////////////////////////////////////////
//Һ�����ƿ���1�������궨��
#define	LCD_CS_SET  	gpio_set(LCD_CS,1)
#define	LCD_RS_SET  	gpio_set(LCD_RS,1)
#define	LCD_SDA_SET  	gpio_set(LCD_SDA,1)
#define	LCD_SCL_SET  	gpio_set(LCD_SCL,1)
#define	LCD_RST_SET  	gpio_set(LCD_RST,1)
#define	LCD_LED_SET  	gpio_set(LCD_LED,1)

//Һ�����ƿ���0�������궨��
#define	LCD_CS_CLR  	gpio_set(LCD_CS,0)
#define	LCD_RS_CLR  	gpio_set(LCD_RS,0)
#define	LCD_SDA_CLR  	gpio_set(LCD_SDA,0)
#define	LCD_SCL_CLR  	gpio_set(LCD_SCL,0)
#define	LCD_RST_CLR  	gpio_set(LCD_RST,0)
#define	LCD_LED_CLR  	gpio_set(LCD_LED,0)

//������ɫ
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
#define LCD_BROWN 	0XBC40 //��ɫ
#define LCD_BRRED       0XFC07 //�غ�ɫ
#define LCD_GRAY  	0X8430 //��ɫ
//GUI��ɫ
#define LCD_DARKBLUE    0X01CF	//����ɫ
#define LCD_LIGHTBLUE   0X7D7C	//ǳ��ɫ
#define LCD_GRAYBLUE    0X5458 //����ɫ
//������ɫΪPANEL����ɫ
#define LCD_LIGHTGREEN  0X841F //ǳ��ɫ
#define LCD_LIGHTGRAY   0XEF5B //ǳ��ɫ(PANNEL)
#define LCD_LGRAY 	0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ
#define LCD_LGRAYBLUE   0XA651 //ǳ����ɫ(�м����ɫ)
#define LCD_LBBLUE      0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)

//��ʾ��ѹ��imgͼ������
#define GRAY_2_RGB565(gray)             ((uint16)((((uint8)(gray)>>3)<<11)|(((uint8)(gray)>>2)<<5)|((uint8)(gray)>>3)))

extern uint16 BACK_COLOR, POINT_COLOR ;

void LCD_Init(void);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);

void LCD_Clear(uint16 Color); //����   LCDȫ�������������
void LCD_SetCursor(uint16 Xpos, uint16 Ypos);  //���ù��λ��
void LCD_SetWindows(int16 xStar, int16 yStar,int16 xEnd,int16 yEnd);  //����lcd��ʾ���ڣ��ڴ�����д�������Զ�����

void LCD_DrawPoint(uint16 x,uint16 y);   //����
void GUI_DrawPoint(uint16 x,uint16 y,uint16 color);//GUI���һ����
uint16 LCD_ReadPoint(uint16 x,uint16 y); //����

void LCD_DrawLine(uint16 x1, uint16 y1, uint16 x2, uint16 y2);          //LCD���� 
void LCD_DrawRectangle(uint16 x1, uint16 y1, uint16 x2, uint16 y2);     //LCD������(�����)
void LCD_DrawFillRectangle(uint16 x1, uint16 y1, uint16 x2, uint16 y2); //LCD������(���)
void gui_circle(int xc, int yc,uint16 c,int r, int fill);    //��ָ��λ�û�һ��ָ����С��Բ(fill=0 ����� 1 ���)

uint16 LCD_RD_DATA(void);//��ȡLCD����
void LCD_WriteReg(uint16 LCD_Reg, uint16 LCD_RegValue);  //д�Ĵ�������
void LCD_WR_DATA(uint8 data);  //��Һ��������д��д8λ����
void LCD_WR_DATA_16Bit(uint16 data);  //8λ���������д��һ��16λ����
void LCD_WriteRAM_Prepare(void);  //��ʼдGRAM
void LCD_SetParam(void);  //����LCD���� ������к�����ģʽ�л�

void LCD_Fill(uint16 sx,uint16 sy,uint16 ex,uint16 ey,uint16 color);  //��ָ�������������ɫ
void LCD_DrawLine(uint16 x1, uint16 y1, uint16 x2, uint16 y2);        //LCD����
void Draw_Circle(uint16 x0,uint16 y0,uint16 fc,uint8 r);

void LCD_ShowChar(uint16 x,uint16 y,uint16 fc, uint16 bc, uint8 num,uint8 size,uint8 mode);  //��ʾ����Ӣ���ַ�
void LCD_ShowString(uint16 x,uint16 y,uint8 size,uint8 *p,uint8 mode);  //��ʾӢ���ַ���

void LCD_ShowNum(uint16 x,uint16 y,uint32 num,uint8 len,uint8 size);  //��ʾ�������ֱ���ֵ
void LCD_ShowNum2412(uint16 x,uint16 y,uint16 fc, uint16 bc,uint8 *p ,uint8 size,uint8 mode);
void LCD_Num_BC(uint16 x,uint16 y, uint32 num, uint8 size, uint8 len,uint16 Color, uint16 bkColor);
void LCD_Num(uint16 x,uint16 y, uint32 num, uint8 size, uint16 Color, uint16 bkColor);

void GUI_DrawFont16(uint16 x, uint16 y, uint16 fc, uint16 bc, uint8 *s,uint8 mode);  //��ʾ����16X16��������
void GUI_DrawFont24(uint16 x, uint16 y, uint16 fc, uint16 bc, uint8 *s,uint8 mode);  //��ʾ����24X24��������
void GUI_DrawFont32(uint16 x, uint16 y, uint16 fc, uint16 bc, uint8 *s,uint8 mode);  //��ʾ����32X32��������
void Show_Str(uint16 x, uint16 y, uint16 fc, uint16 bc, uint8 *str,uint8 size,uint8 mode);  //��ʾһ���ַ���,������Ӣ����ʾ
void Gui_StrCenter(uint16 x, uint16 y, uint16 fc, uint16 bc, uint8 *str,uint8 size,uint8 mode);  //������ʾһ���ַ���,������Ӣ����ʾ

void Gui_Drawbmp16(uint16 x,uint16 y,const unsigned char *p); //��ʾһ��16λBMPͼ��  40*40��С



#endif


