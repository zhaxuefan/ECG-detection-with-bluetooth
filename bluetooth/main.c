#include <reg52.h>                
#include "i2c.h"
#include "delay.h"
#include "display.h"

#define AddWr 0x90   //写数据地址 
#define AddRd 0x91   //读数据地址
unsigned char date;
extern bit ack;
bit WriteDAC(unsigned char dat);
static unsigned char num2=0;
unsigned char  irtime;//红外用全局变量


void tim0_isr (void) interrupt 1 using 1
{
 //irtime++;  //用于计数2个下降沿之间的时间
}




void delay1ms()
{
 unsigned int i;
 for(i=10000;i>0;i--);  //延时124*8+10=1002us
}
Initial_com()
{
 EA=1;        //开总中断
 ES=1;        //允许串口中断
 ET1=1;        //允许定时器T1的中断
 TMOD=0x20;   //定时器T1，在方式2中断产生波特率
 PCON=0x00;   //SMOD=0
 SCON=0x50;   // 方式1 由定时器控制
 TH1=0xfd;    //波特率设置为9600
 TL1=0xfd;
 TR1=1;       //开定时器T1运行控制位

}

//*****串口初始化函数***********
/*------------------------------------------------
                定时器0初始化
------------------------------------------------*/
void TIM0init(void)//定时器0初始化
{

  TMOD=0x02;//定时器0工作方式2，TH0是重装值，TL0是初值
TH0=0x00;	      //给定初值，这里使用定时器最大值从0开始计数一直到65535溢出
 TL0=0x00;
 ET0=1;           //定时器中断打开
 TR0=1;           //定时器开关打开 
}
/*------------------------------------------------
                  外部中断0初始化
------------------------------------------------*/
void EX0init(void)
{
 IT0 = 1;   //指定外部中断0下降沿触发，INT0 (P3.2)
 EX0 = 1;   //使能外部中断
 EA = 1;    //开总中断
}




main()
{
////////////////////////////////////////////////
  Init_Timer0();
  Initial_com();
EX0init(); //初始化外部中断
 TIM0init();//初始化定时器
//num2=100;
WriteDAC(num2);
////////////////////////////////////////////////
	 Initial_com();
	 while(1)
	 {
		 SBUF=0XAA;
		 delay1ms();
		 SBUF=0XBB;
		 delay1ms();
		  if(RI)
		  {

			   date=SBUF;    //单片机接受
if (date=='a')  { WriteDAC(30);}
if (date=='b')  {  WriteDAC(60); }
if (date=='c')  {  WriteDAC(90); }
if (date=='d')  {  WriteDAC(120); }
if (date=='e')  {  WriteDAC(150); }
if (date=='f')  {  WriteDAC(180); }
if (date=='g')  {  WriteDAC(210); }
if (date=='h')  {  WriteDAC(240); }
if (date=='i')  {  WriteDAC(255); }
if (date=='j')  {  WriteDAC(0); }
 //delay1ms();
			   SBUF=date;    //单片机发送
delay1ms();

			   RI=0;
		  }
		
		 
	  }
}


/*------------------------------------------------
               写入DA转换数值
输入参数：dat 表示需要转换的DA数值，范围是0-255
------------------------------------------------*/
bit WriteDAC(unsigned char dat)
{
   Start_I2c();               //启动总线
   SendByte(AddWr);             //发送器件地址
     if(ack==0)return(0);
   SendByte(0x40);            //发送器件子地址
     if(ack==0)return(0);
   SendByte(dat);             //发送数据
     if(ack==0)return(0);
   Stop_I2c();  
}

