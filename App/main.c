#include "common.h"
#include "include.h"

uint8 imgbuff[CAMERA_SIZE];                             //����洢����ͼ�������
uint8 img[CAMERA_H * CAMERA_W];
uint8 screen_mid = 40;		  //��Ļ�е�
uint8 Point_Mid  = 40;
extern int32 S3010_Duty;      //4300
uint8 Point;	//Ŀ��������
extern uint8 CNST_VALUE;
extern uint8 CNST_Value ;

void LCD_dispaly(void);

//��������
void vcan_sendimg(uint8 *imgaddr, uint32 imgsize);
void img_extract(uint8 *dst, uint8 *src, uint32 srclen);


void  main(void)
{
    Point = 40;     //������
    System_Init();  //��ʼ������ģ��
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
        Point = Point_Weight();  //��ȡĿ���	����Ŀ��Ƕ�
        S3010_Duty = PlacePID_Control(&S3010_PID,Point,Point_Mid);	     //ʹ��λ��ʽPID
        S3010_Duty = range_protect(S3010_Duty, S3010_LEFT, S3010_RIGHT); //���ռ�ձ��޷�����
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


//���Ͻ���ʾimg80*60ͼ��
void LCD_dispaly(void)
{

    uint32 temp, tempY;
    uint16 x, y;
    uint16 X, Y;
    uint8 *pimg = (uint8 *)img;
    uint16 rgb;

//    Site_t site     = {0, 0};    //��ʾͼ�����Ͻ�λ��
    Size_t imgsize  = {80, 60};    //ͼ���С
    Size_t size     = {160, 60};    //��ʾ����ͼ���С


    LCD_SetWindows(0-2,0+29,80-1-2,60-1+29);
    //LCD_RAMWR();                                //д�ڴ�

    for(y = 0; y < size.H; y++)
    {
        Y = ( (  y * imgsize.H   ) / size.H) ;
        tempY = Y * imgsize.W ;

        for(x = 0; x < size.W; x++)
        {
            X = ( x * imgsize.W  ) / size.W ;
            temp = tempY + X;
            rgb = GRAY_2_RGB565(pimg[temp]);    //
            LCD_WR_DATA(rgb);    //д��λ����
            //LCD_WR_DATA_16Bit(rgb);//дʮ��λ����
        }
    }

}

//����ͼ����λ����ʾ
//��ͬ����λ������ͬ������
//���ʹ��������λ��������Ҫ�޸Ĵ���
void vcan_sendimg(uint8 *imgaddr, uint32 imgsize)
{
#define CMD_IMG     1
    uint8 cmdf[2] = {CMD_IMG, ~CMD_IMG};    //ɽ����λ�� ʹ�õ�����
    uint8 cmdr[2] = {~CMD_IMG, CMD_IMG};    //ɽ����λ�� ʹ�õ�����

    uart_putbuff(VCAN_PORT, cmdf, sizeof(cmdf));    //�ȷ�������

    uart_putbuff(VCAN_PORT, imgaddr, imgsize); //�ٷ���ͼ��

    uart_putbuff(VCAN_PORT, cmdr, sizeof(cmdr));    //�ȷ�������
}


/*!
 *  @brief      ��ֵ��ͼ���ѹ���ռ� �� ʱ�� ��ѹ��
 *  @param      dst             ͼ���ѹĿ�ĵ�ַ
 *  @param      src             ͼ���ѹԴ��ַ
 *  @param      srclen          ��ֵ��ͼ���ռ�ÿռ��С
 *  @since      v5.0            img_extract(img, imgbuff,CAMERA_SIZE);
 *  Sample usage:
 */
void img_extract(uint8 *dst, uint8 *src, uint32 srclen)
{
    uint8 colour[2] = {255, 0}; //0 �� 1 �ֱ��Ӧ����ɫ
    //ע��ɽ�������ͷ 0 ��ʾ ��ɫ��1��ʾ ��ɫ
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

