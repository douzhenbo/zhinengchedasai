#include "common.h"
#include "lcd.h"
#include "font.h"
#include "MKL_port.h"
#include "MKL_gpio.h"
#include "VCAN_OV7725_Eagle.h"

//����LCD��Ҫ����
//Ĭ��Ϊ����
_lcd_dev lcddev;

//������ɫ����ɫ,������ɫ����ɫ
uint16 POINT_COLOR = 0x0000,BACK_COLOR = 0xFFFF;
uint16 DeviceCode;

/****************************************************************************
* ��    �ƣ�void  SPIv_WriteData(uint8 Data)
* ��    �ܣ�STM32_ģ��SPIдһ���ֽ����ݵײ㺯��
* ��ڲ�����Data
* ���ڲ�������
* ˵    ����STM32_ģ��SPI��дһ���ֽ����ݵײ㺯��
****************************************************************************/
void  SPIv_WriteData(uint8 Data)
{
	unsigned char i=0;
	for(i=8;i>0;i--)
	{
		if(Data&0x80)
	  LCD_SDA_SET; //�������
      else LCD_SDA_CLR;

      LCD_SCL_CLR;
      LCD_SCL_SET;
      Data<<=1;
	}
}


//******************************************************************
//��������  LCD_WR_REG
//���ܣ�    ��Һ��������д��д16λָ��
//���������Reg:��д���ָ��ֵ
//����ֵ��  ��
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
//��������  LCD_WR_DATA
//���ܣ�    ��Һ��������д��д8λ����
//���������Data:��д�������
//����ֵ��  ��
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
//��������  LCD_DrawPoint_16Bit
//���ܣ�    8λ���������д��һ��16λ����
//���������(x,y):�������
//����ֵ��  ��
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
//��������  LCD_WriteReg
//���ܣ�    д�Ĵ�������
//���������LCD_Reg:�Ĵ�����ַ
//			LCD_RegValue:Ҫд�������
//����ֵ��  ��
//******************************************************************
void LCD_WriteReg(uint16 LCD_Reg, uint16 LCD_RegValue)
{
	LCD_WR_REG(LCD_Reg);
	LCD_WR_DATA(LCD_RegValue);
}

//******************************************************************
//��������  LCD_WriteRAM_Prepare
//���ܣ�    ��ʼдGRAM
//			�ڸ�Һ��������RGB����ǰ��Ӧ�÷���дGRAMָ��
//�����������
//����ֵ��  ��
//******************************************************************
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.wramcmd);
}

//******************************************************************
//��������  LCD_Reset
//���ܣ�    LCD��λ������Һ����ʼ��ǰҪ���ô˺���
//�����������
//����ֵ��  ��
//******************************************************************
void LCD_RESET(void)
{
	LCD_RST_CLR;
	DELAY_MS(100);
	LCD_RST_SET;
	DELAY_MS(100);
}

//******************************************************************
//��������  LCD_GPIOInit
//���ܣ�    Һ����IO��ʼ����Һ����ʼ��ǰҪ���ô˺���
//�����������
//����ֵ��  ��
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
//��������  LCD_Init
//���ܣ�    LCD��ʼ��
//�����������
//����ֵ��  ��
//******************************************************************
void LCD_Init(void)
{

//#if USE_HARDWARE_SPI //ʹ��Ӳ��SPI
	//SPI2_Init();
//#else
	LCD_GPIOInit();//ʹ��ģ��SPI
//#endif

 	LCD_RESET(); //Һ������λ

	//************* Start Initial Sequence **********//
	//��ʼ��ʼ��Һ����
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

	LCD_SetParam();//����LCD����  ������ģʽ�л�
	//LCD_LED_SET;//��������
	//LCD_Clear(WHITE);
}

/*******************************************************************
//��������  LCD_Clear
//���ܣ�    LCDȫ�������������
//���������Color:Ҫ���������ɫ
//����ֵ��  ��
//��ע��		��У׼
8******************************************************************/
void LCD_Clear(uint16 Color)
{
	uint16 i,j;
	//LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);
	LCD_SetWindows(0-2,29,128-1-2,128-1+29);
	for(i=0;i<lcddev.width;i++)
	{
		for(j=0;j<lcddev.height;j++)
		LCD_WR_DATA_16Bit(Color);	//д������
	}
}
/*************************************************
��������LCD_SetWindows
���ܣ�����lcd��ʾ���ڣ��ڴ�����д�������Զ�����
��ڲ�����xy�����յ�
����ֵ����
˵����LCD_SetWindows(0-2,0+29,lcddev.width-1-2,lcddev.height-1+29);//�ָ���������Ϊȫ��
*************************************************/
void LCD_SetWindows(int16 xStar, int16 yStar,int16 xEnd,int16 yEnd)
{
#if USE_HORIZONTAL==1	//ʹ�ú���
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

	LCD_WriteRAM_Prepare();	//��ʼд��GRAM
}

/*******************************************************************
��������LCD_SetParam
���ܣ�����LCD���� ������к�����ģʽ�л�
��ڲ�������
����ֵ����
********************************************************************/
void LCD_SetParam(void)
{
	lcddev.wramcmd=0x2C;
#if USE_HORIZONTAL==1	//ʹ�ú���
	lcddev.dir=1;//����
	lcddev.width=128;
	lcddev.height=128;
	lcddev.setxcmd=0x2A;
	lcddev.setycmd=0x2B;
	LCD_WriteReg(0x36,0xA8);

#else//����
	lcddev.dir=0;//����
	lcddev.width=128;
	lcddev.height=128;
	lcddev.setxcmd=0x2A;
	lcddev.setycmd=0x2B;
	LCD_WriteReg(0x36,0xC8);
#endif
        
}
/******************************************************************
��������LCD_SetCursor
���ܣ����ù��λ��
��ڲ�����xy����
����ֵ����
��ע��	��У׼
******************************************************************/
void LCD_SetCursor(uint16 Xpos, uint16 Ypos)
{
	//LCD_SetWindows(Xpos,Ypos,Xpos,Ypos);
	LCD_SetWindows(Xpos-2,Ypos+29,Xpos-2,Ypos+29);
}
/*******************************************************************
//��������  LCD_DrawPoint
//���ܣ�    ��ָ��λ��д��һ�����ص�����
//���������(x,y):�������
//����ֵ��  ��
//��ע�� 	��У׼  ��ɫΪ������ɫ��ɫ����ɫ���޸�
*******************************************************************/
void LCD_DrawPoint(uint16 x,uint16 y)
{
	LCD_SetCursor(x,y);//���ù��λ��
	LCD_WR_DATA_16Bit(POINT_COLOR);
}



//�ֽ��ߣ�����Ϊ�������룬����ΪGUIӦ��
//*********************************************************************************/



/*******************************************************************
//��������  GUI_DrawPoint
//���ڣ�    2013-02-22
//���ܣ�    GUI���һ����
//���������x:���λ��x����
//          y:���λ��y����
//	    color:Ҫ������ɫ
//��ע��		��У׼
**8*****************************************************************/
void GUI_DrawPoint(uint16 x,uint16 y,uint16 color)
{
	LCD_SetCursor(x,y);//���ù��λ��
	LCD_WR_DATA_16Bit(color);
}

/*******************************************************************
//��������  LCD_Fill
//���ܣ�    ��ָ�������������ɫ
//���������sx:ָ������ʼ��x����
//          sy:ָ������ʼ��y����
//	    ex:ָ�����������x����
//	    ey:ָ�����������y����
//        	color:Ҫ������ɫ
//����ֵ��  ��
//��ע��		��У׼
*******************************************************************/
void LCD_Fill(uint16 sx,uint16 sy,uint16 ex,uint16 ey,uint16 color)
{

	uint16 i,j;
	uint16 width=ex-sx+1; 		//�õ����Ŀ��
	uint16 height=ey-sy+1;		//�߶�
	//LCD_SetWindows(sx,sy,ex-1,ey-1);//������ʾ����
	LCD_SetWindows(sx-2,sy+29,ex-1-2,ey-1+29);//������ʾ����
	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
		LCD_WR_DATA_16Bit(color);	//д������
	}

	LCD_SetWindows(0-2,0+29,lcddev.width-1-2,lcddev.height-1+29);//�ָ���������Ϊȫ��
}

/******************************************************************
//��������  LCD_DrawLine
//���ܣ�    GUI����
//���������x1,y1:�������
//        	x2,y2:�յ�����
//����ֵ��  ��
//��ע�� 	��У׼
8*****************************************************************/
void LCD_DrawLine(uint16 x1, uint16 y1, uint16 x2, uint16 y2)
{
	uint16 t;
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;

	delta_x=x2-x1; //������������
	delta_y=y2-y1;
	uRow=x1;
	uCol=y1;
	if(delta_x>0)incx=1; //���õ�������
	else if(delta_x==0)incx=0;//��ֱ��
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if(delta_y==0)incy=0;//ˮƽ��
	else{incy=-1;delta_y=-delta_y;}
	if( delta_x>delta_y)distance=delta_x; //ѡȡ��������������
	else distance=delta_y;
	for(t=0;t<=distance+1;t++ )//�������
	{
		LCD_DrawPoint(uRow,uCol);//����
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
//��������  LCD_DrawRectangle
//���ܣ�    GUI������(�����)
//���������(x1,y1),(x2,y2):���εĶԽ�����
//����ֵ��  ��   ��֤��ȷ
//˵����		��У׼
********************************************************************/
void LCD_DrawRectangle(uint16 x1, uint16 y1, uint16 x2, uint16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}

/*******************************************************************
//��������  LCD_DrawFillRectangle
//���ܣ�    GUI������(���)
//���������(x1,y1),(x2,y2):���εĶԽ�����
//����ֵ��  ��
//˵����		��У׼
*******************************************************************/
void LCD_DrawFillRectangle(uint16 x1, uint16 y1, uint16 x2, uint16 y2)
{
	LCD_Fill(x1,y1,x2,y2,POINT_COLOR);

}
/*******************************************************************
//��������  _draw_circle_8
//���ܣ�    8�Գ��Ի�Բ�㷨(�ڲ�����)
//���������(xc,yc) :Բ��������
// 			(x,y):��������Բ�ĵ�����
//         	c:������ɫ
//����ֵ��  ��
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
//��������  gui_circle
//���ܣ�    ��ָ��λ�û�һ��ָ����С��Բ(���)
//���������(xc,yc) :Բ��������
//           c:������ɫ
//	     r:Բ�뾶
//	     fill:����жϱ�־��1-��䣬0-�����
//����ֵ��  ��
//˵����		��У׼
*******************************************************************/
void gui_circle(int xc, int yc,uint16 c,int r, int fill)
{
	int x = 0, y = r, yi, d;

	d = 3 - 2 * r;


	if (fill)
	{
		// �����䣨��ʵ��Բ��
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
		// �������䣨������Բ��
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
//��������  LCD_ShowChar
//���ܣ�    ��ʾ����Ӣ���ַ�
//���������(x,y):�ַ���ʾλ����ʼ����
//        	fc:ǰ�û�����ɫ
//		bc:������ɫ
//		num:��ֵ��0-94��
//		size:�����С
//		mode:ģʽ  0,���ģʽ;1,����ģʽ
//����ֵ��  ��
********************************************************************/
void LCD_ShowChar(uint16 x,uint16 y,uint16 fc, uint16 bc, uint8 num,uint8 size,uint8 mode)
{
    uint8 temp;
    uint8 pos,t;
	uint16 colortemp=POINT_COLOR;

	num=num-' ';//�õ�ƫ�ƺ��ֵ
	LCD_SetWindows(x,y,x+size/2-1,y+size-1);//���õ���������ʾ����
	if(!mode) //�ǵ��ӷ�ʽ
	{

		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=asc2_1206[num][pos];//����1206����
			else temp=asc2_1608[num][pos];		 //����1608����
			for(t=0;t<size/2;t++)
		    {
		        if(temp&0x01)LCD_WR_DATA(fc);
				else LCD_WR_DATA(bc);
				temp>>=1;

		    }

		}
	}else//���ӷ�ʽ
	{
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=asc2_1206[num][pos];//����1206����
			else temp=asc2_1608[num][pos];		 //����1608����
			for(t=0;t<size/2;t++)
		    {
				POINT_COLOR=fc;
		        if(temp&0x01)LCD_DrawPoint(x+t,y+pos);//��һ����
		        temp>>=1;
		    }
		}
	}
	POINT_COLOR=colortemp;
	LCD_SetWindows(0-2,0+29,lcddev.width-1-2,lcddev.height-1+29);//�ָ���������Ϊȫ��
}

/*******************************************************************
//��������  LCD_ShowString
//���ܣ�    ��ʾӢ���ַ���
//���������x,y :�������
//			size:�����С
//			*p:�ַ�����ʼ��ַ
//			mode:ģʽ	0,���ģʽ;1,����ģʽ
//����ֵ��  ��
********************************************************************/
void LCD_ShowString(uint16 x,uint16 y,uint8 size,uint8 *p,uint8 mode)
{
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {
		if(x>(lcddev.width-1)||y>(lcddev.height-1))
		return;
        LCD_ShowChar(x,y,POINT_COLOR,BACK_COLOR,*p,size,mode);
        x+=size/2;
        p++;
    }
}

/*******************************************************************
//��������  LCD_ShowNum2412
//���ܣ�    
//���������(x,y):�ַ���ʾλ����ʼ����
//        	fc:ǰ�û�����ɫ
//			bc:������ɫ
//			num:��ֵ��0-94��
//			size:�����С
//			mode:ģʽ  0,���ģʽ;1,����ģʽ
//����ֵ��  ��
********************************************************************/
void LCD_ShowNum2412(uint16 x,uint16 y,uint16 fc, uint16 bc,uint8 *p ,uint8 size,uint8 mode)
{
    uint16 temp;
    uint8 pos,t;
	uint16 colortemp=POINT_COLOR;
	uint16 x0=x;
	uint16 y0=y;
	uint8 num=0;


    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {

		if(x>(lcddev.width-1)||y>(lcddev.height-1))
		return;
		num=*p;
		if(':'==num) //�����ַ�":"
		num=10;
		else if('.'==num)//�����ַ�"."
		num=11;
		else  //������
		num=num-'0';
		x0=x;

		for(pos=0;pos<48;pos++)
		{
			temp=asc2_2412[num][pos];
			for(t=0;t<8;t++)
		    {
				POINT_COLOR=fc;
		        if(temp&0x80)LCD_DrawPoint(x,y);//��һ����
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
	x+=16;	//��Ϊ�����־࣬ʹ���Ű���ÿ���ԭֵΪ12
	else
	x+=8; //��Ϊ�����־࣬ʹ���Ű���ÿ���ԭֵΪ12

	y=y0;
    p++;
    }
	POINT_COLOR=colortemp;
}


/*******************************************************************
//��������  mypow
//���ܣ�    ��m��n�η�(gui�ڲ�����)
//���������m:����
//	        n:��
//����ֵ��  m��n�η�
********************************************************************/
uint32 mypow(uint8 m,uint8 n)
{
	uint32 result=1;
	while(n--)result*=m;
	return result;
}

/*******************************************************************
//��������  LCD_ShowNum
//���ܣ�    ��ʾ�������ֱ���ֵ
//���������x,y :�������
//			len :ָ����ʾ���ֵ�λ��
//			size:�����С(12,16)
//			color:��ɫ
//			num:��ֵ(0~4294967295)
//����ֵ��  ��
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
//��������  GUI_DrawFont16
//���ܣ�    ��ʾ����16X16��������
//���������x,y :�������
//			fc:ǰ�û�����ɫ
//			bc:������ɫ
//			s:�ַ�����ַ
//			mode:ģʽ	0,���ģʽ;1,����ģʽ
//����ֵ��  ��
********************************************************************/
void GUI_DrawFont16(uint16 x, uint16 y, uint16 fc, uint16 bc, uint8 *s,uint8 mode)
{
	uint8 i,j;
	uint16 k;
	uint16 HZnum;
	uint16 x0=x;
	HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);	//�Զ�ͳ�ƺ�����Ŀ


	for (k=0;k<HZnum;k++)
	{
	  if ((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1)))
	  { 	LCD_SetWindows(x,y,x+16-1,y+16-1);
		    for(i=0;i<16*2;i++)
		    {
				for(j=0;j<8;j++)
		    	{
					if(!mode) //�ǵ��ӷ�ʽ
					{
						if(tfont16[k].Msk[i]&(0x80>>j))	LCD_WR_DATA_16Bit(fc);
						else LCD_WR_DATA_16Bit(bc);
					}
					else
					{
						POINT_COLOR=fc;
						if(tfont16[k].Msk[i]&(0x80>>j))	LCD_DrawPoint(x,y);//��һ����
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
		continue;  //���ҵ���Ӧ�����ֿ������˳�����ֹ��������ظ�ȡģ����Ӱ��
	}

	LCD_SetWindows(0-2,0+29,lcddev.width-1-2,lcddev.height-1+29);//�ָ���������Ϊȫ��
}

/*******************************************************************
//��������  GUI_DrawFont24
//���ܣ�    ��ʾ����24X24��������
//���������x,y :�������
//			fc:ǰ�û�����ɫ
//			bc:������ɫ
//			s:�ַ�����ַ
//			mode:ģʽ	0,���ģʽ;1,����ģʽ
//����ֵ��  ��
********************************************************************/
void GUI_DrawFont24(uint16 x, uint16 y, uint16 fc, uint16 bc, uint8 *s,uint8 mode)
{
	uint8 i,j;
	uint16 k;
	uint16 HZnum;
	uint16 x0=x;
	HZnum=sizeof(tfont24)/sizeof(typFNT_GB24);	//�Զ�ͳ�ƺ�����Ŀ

			for (k=0;k<HZnum;k++)
			{
			  if ((tfont24[k].Index[0]==*(s))&&(tfont24[k].Index[1]==*(s+1)))
			  { 	LCD_SetWindows(x,y,x+24-1,y+24-1);
				    for(i=0;i<24*3;i++)
				    {
							for(j=0;j<8;j++)
							{
								if(!mode) //�ǵ��ӷ�ʽ
								{
									if(tfont24[k].Msk[i]&(0x80>>j))	LCD_WR_DATA_16Bit(fc);
									else LCD_WR_DATA_16Bit(bc);
								}
							else
							{
								POINT_COLOR=fc;
								if(tfont24[k].Msk[i]&(0x80>>j))	LCD_DrawPoint(x,y);//��һ����
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
				continue;  //���ҵ���Ӧ�����ֿ������˳�����ֹ��������ظ�ȡģ����Ӱ��
			}

	LCD_SetWindows(0-2,0+29,lcddev.width-1-2,lcddev.height-1+29);//�ָ���������Ϊȫ��
}

/*******************************************************************
//��������  GUI_DrawFont32
//���ܣ�    ��ʾ����32X32��������
//���������x,y :�������
//			fc:ǰ�û�����ɫ
//			bc:������ɫ
//			s:�ַ�����ַ
//			mode:ģʽ	0,���ģʽ;1,����ģʽ
//����ֵ��  ��
********************************************************************/
void GUI_DrawFont32(uint16 x, uint16 y, uint16 fc, uint16 bc, uint8 *s,uint8 mode)
{
	uint8 i,j;
	uint16 k;
	uint16 HZnum;
	uint16 x0=x;
	HZnum=sizeof(tfont32)/sizeof(typFNT_GB32);	//�Զ�ͳ�ƺ�����Ŀ
	for (k=0;k<HZnum;k++)
	{
	  if ((tfont32[k].Index[0]==*(s))&&(tfont32[k].Index[1]==*(s+1)))
	  {
			LCD_SetWindows(x,y,x+32-1,y+32-1);
			for(i=0;i<32*4;i++)
			{
				for(j=0;j<8;j++)
				{
					if(!mode) //�ǵ��ӷ�ʽ
					{
						if(tfont32[k].Msk[i]&(0x80>>j))	LCD_WR_DATA_16Bit(fc);
						else LCD_WR_DATA_16Bit(bc);
					}
					else
					{
						POINT_COLOR=fc;
						if(tfont32[k].Msk[i]&(0x80>>j))	LCD_DrawPoint(x,y);//��һ����
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
		continue;  //���ҵ���Ӧ�����ֿ������˳�����ֹ��������ظ�ȡģ����Ӱ��
	}

	LCD_SetWindows(0-2,0+29,lcddev.width-1-2,lcddev.height-1+29);//�ָ���������Ϊȫ��
}

/*******************************************************************
//��������  Show_Str
//���ܣ�    ��ʾһ���ַ���,������Ӣ����ʾ
//���������x,y :�������
// 			fc:ǰ�û�����ɫ
//			bc:������ɫ
//			str :�ַ���
//			size:�����С
//			mode:ģʽ	0,���ģʽ;1,����ģʽ
//����ֵ��  ��
8*******************************************************************/
void Show_Str(uint16 x, uint16 y, uint16 fc, uint16 bc, uint8 *str,uint8 size,uint8 mode)
{
	uint16 x0=x;
  	uint8 bHz=0;     //�ַ���������
    while(*str!=0)//����δ����
    {
        if(!bHz)
        {
			if(x>(lcddev.width-size/2)||y>(lcddev.height-size))
			return;
	        if(*str>0x80)bHz=1;//����
	        else              //�ַ�
	        {
		        if(*str==0x0D)//���з���
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
					x+=size/2; //�ַ�,Ϊȫ�ֵ�һ��
					}
					else//�ֿ���û�м���16X32��Ӣ������,��8X16����
					{
					 	LCD_ShowChar(x,y,fc,bc,*str,16,mode);
						x+=8; //�ַ�,Ϊȫ�ֵ�һ��
					}
				}
				str++;

	        }
        }
		else//����
        {
			if(x>(lcddev.width-size)||y>(lcddev.height-size))
			return;
            bHz=0;//�к��ֿ�
			if(size==32)
			GUI_DrawFont32(x,y,fc,bc,str,mode);
			else if(size==24)
			GUI_DrawFont24(x,y,fc,bc,str,mode);
			else
			GUI_DrawFont16(x,y,fc,bc,str,mode);

	        str+=2;
	        x+=size;//��һ������ƫ��
        }
    }
}

/*******************************************************************
//��������  Gui_StrCenter
//���ܣ�    ������ʾһ���ַ���,������Ӣ����ʾ
//���������x,y :�������
// 			fc:ǰ�û�����ɫ
//			bc:������ɫ
//			str :�ַ���
//			size:�����С
//			mode:ģʽ	0,���ģʽ;1,����ģʽ
//����ֵ��  ��
*******************************************************************/
void Gui_StrCenter(uint16 x, uint16 y, uint16 fc, uint16 bc, uint8 *str,uint8 size,uint8 mode)
{
	uint16 len=strlen((const char *)str);
	uint16 x1=(lcddev.width-len*8)/2;
	Show_Str(x+x1,y,fc,bc,str,size,mode);
}

/*******************************************************************
//��������  Gui_Drawbmp16
//���ܣ�    ��ʾһ��16λBMPͼ��
//���������x,y :�������
// 			*p :ͼ��������ʼ��ַ
//����ֵ��  ��
********************************************************************/
void Gui_Drawbmp16(uint16 x,uint16 y,const unsigned char *p) //��ʾ40*40 QQͼƬ
{
  	int i;
	unsigned char picH,picL;
	LCD_SetWindows(x,y,x+40-1,y+40-1);//��������
    for(i=0;i<40*40;i++)
	{
	 	picL=*(p+i*2);	//���ݵ�λ��ǰ
		picH=*(p+i*2+1);
		LCD_WR_DATA_16Bit(picH<<8|picL);
	}
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//�ָ���ʾ����Ϊȫ��

}


//���޸ģ�����
void LCD_char(uint16 x,uint16 y, uint8 num, uint8 size,uint16 Color, uint16 bkColor)
{
#define MAX_CHAR_POSX (240-8)
#define MAX_CHAR_POSY (320-16)

    uint8 temp, t, pos;    
    if(x > MAX_CHAR_POSX || y > MAX_CHAR_POSY)
    {
        return;
    }
    num=num-' ';//�õ�ƫ�ƺ��ֵ
    LCD_SetWindows(x, y,x+8,y+16);
    //LCD_RAMWR();                    //д�ڴ�

    for (pos = 0; pos < size; pos++)
    {
        if(size==12)temp=asc2_1206[num][pos];//����1206����
        else temp=asc2_1608[num][pos];       //����1608����
        
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
//��LCD��ʾ����  ���ֿ�ˢ��
//���ܣ�    ��ʾ�������ֱ���ֵ
//���������x,y :�������
//          num�����ֱ���
//          size:�����С(12,16)
//	    color:��ɫ
//	    num:��ֵ(0~4294967295)
//����ֵ��  ��
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
    while( res )  /*�õ����ֳ���t*/
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
//��LCD��ʾ����  ���ֿ�ˢ�� (��ն����λ��
//���ܣ�    ��ʾ�������ֱ���ֵ
//���������x,y :�������
//          num�����ֱ���
//          size:�����С(12,16)
//          len: ���ֳ���
//	    color:��ɫ
//	    num:��ֵ(0~4294967295)
//����ֵ��  ��
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
    while( res )            /*�õ����ֳ���t*/
    {
        res /= 10;
        t++;
    }
    if(t >= len )    //������󳤶�
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


