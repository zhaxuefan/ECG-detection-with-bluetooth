#ifndef __TFT_H
#define __TFT_H	

#include  "msp430x14x.h"

#define	u8 unsigned char
#define	u16 unsigned int
#define	u32 unsigned long
#define	uchar unsigned char
#define	uint unsigned int

//  RS  P25		//数据/命令选择
//  RW  P24         //写数据/命令
//  RD  P23         //读控制
//  CS  P22		//片选
//  RES P21		//复位
//  LE  P20	        //74HC573锁存控制


#define DataPort_L P2OUT     //数据口使用P2
#define DataPort_H P4OUT     //数据口使用P2
#define ComPort P6OUT


#define	LCD_LED P6OUT |= BIT0 //LCD背光

#define RES_1  ComPort |= BIT1
#define RES_0  ComPort &= ~BIT1

#define CS_1  ComPort |= BIT2
#define CS_0  ComPort &= ~BIT2

#define nRD_1  ComPort |= BIT3
#define nRD_0  ComPort &= ~BIT3

#define RW_1  ComPort |= BIT4
#define RW_0  ComPort &= ~BIT4

#define RS_1  ComPort |= BIT5
#define RS_0  ComPort &= ~BIT5

//******************全局变量***************************

#define white          0xFFFF   //LCD color
#define black          0x0000
#define blue           0x001F
#define blue2          0x051F
#define red            0xF800
#define magenta        0xF81F
#define green          0x07E0
#define cyan           0x7FFF
#define yellow         0xFFE0

#define L2R_U2D  (0<<5)|(1<<4)|(1<<3);  //从左到右,从上到下
#define L2R_D2U  (0<<5)|(0<<4)|(1<<3); //从左到右,从下到上
#define R2L_U2D  (0<<5)|(0<<4)|(0<<3);//从右到左,从上到下
#define R2L_D2U  (1<<5)|(0<<4)|(1<<3); //从右到左,从下到上

#define U2D_L2R  (0<<5)|(1<<4)|(0<<3); //从上到下,从左到右
#define U2D_R2L  (1<<5)|(1<<4)|(0<<3);//从上到下,从右到左
#define D2U_L2R  (0<<5)|(0<<4)|(0<<3); //从下到上,从左到右
#define D2U_R2L  (1<<5)|(0<<4)|(0<<3); //从下到上,从右到左


extern unsigned int Device_code;
void TFT_Initial(void);
void Write_Cmd_Data(unsigned char x, unsigned int y);
void Write_Cmd(unsigned char DH,unsigned char DL);
void Write_Data(unsigned char DH,unsigned char DL);
void delayms(unsigned int tt);
void  Write_Data_U16(unsigned int y);
void CLR_Screen(unsigned int bColor);
void Put_pixel(unsigned int x,unsigned int y,unsigned int color);
void address_set(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2);
void LCD_SetPos(u16 Xpos, u16 Ypos);
void LCD_SetDisplayWindow(u8 Xpos, u16 Ypos, u16 Height, u8 Width);
void LCD_WriteBMP(u8 Xpos, u16 Ypos, u16 Height, u8 Width, const u8 *bitmap);


#endif