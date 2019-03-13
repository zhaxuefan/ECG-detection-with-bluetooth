#include <reg52.h>                
#include "i2c.h"
#include "delay.h"
#include "display.h"

#define AddWr 0x90   //д���ݵ�ַ 
#define AddRd 0x91   //�����ݵ�ַ
unsigned char date;
extern bit ack;
bit WriteDAC(unsigned char dat);
static unsigned char num2=0;
unsigned char  irtime;//������ȫ�ֱ���


void tim0_isr (void) interrupt 1 using 1
{
 //irtime++;  //���ڼ���2���½���֮���ʱ��
}




void delay1ms()
{
 unsigned int i;
 for(i=10000;i>0;i--);  //��ʱ124*8+10=1002us
}
Initial_com()
{
 EA=1;        //�����ж�
 ES=1;        //�������ж�
 ET1=1;        //����ʱ��T1���ж�
 TMOD=0x20;   //��ʱ��T1���ڷ�ʽ2�жϲ���������
 PCON=0x00;   //SMOD=0
 SCON=0x50;   // ��ʽ1 �ɶ�ʱ������
 TH1=0xfd;    //����������Ϊ9600
 TL1=0xfd;
 TR1=1;       //����ʱ��T1���п���λ

}

//*****���ڳ�ʼ������***********
/*------------------------------------------------
                ��ʱ��0��ʼ��
------------------------------------------------*/
void TIM0init(void)//��ʱ��0��ʼ��
{

  TMOD=0x02;//��ʱ��0������ʽ2��TH0����װֵ��TL0�ǳ�ֵ
TH0=0x00;	      //������ֵ������ʹ�ö�ʱ�����ֵ��0��ʼ����һֱ��65535���
 TL0=0x00;
 ET0=1;           //��ʱ���жϴ�
 TR0=1;           //��ʱ�����ش� 
}
/*------------------------------------------------
                  �ⲿ�ж�0��ʼ��
------------------------------------------------*/
void EX0init(void)
{
 IT0 = 1;   //ָ���ⲿ�ж�0�½��ش�����INT0 (P3.2)
 EX0 = 1;   //ʹ���ⲿ�ж�
 EA = 1;    //�����ж�
}




main()
{
////////////////////////////////////////////////
  Init_Timer0();
  Initial_com();
EX0init(); //��ʼ���ⲿ�ж�
 TIM0init();//��ʼ����ʱ��
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

			   date=SBUF;    //��Ƭ������
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
			   SBUF=date;    //��Ƭ������
delay1ms();

			   RI=0;
		  }
		
		 
	  }
}


/*------------------------------------------------
               д��DAת����ֵ
���������dat ��ʾ��Ҫת����DA��ֵ����Χ��0-255
------------------------------------------------*/
bit WriteDAC(unsigned char dat)
{
   Start_I2c();               //��������
   SendByte(AddWr);             //����������ַ
     if(ack==0)return(0);
   SendByte(0x40);            //���������ӵ�ַ
     if(ack==0)return(0);
   SendByte(dat);             //��������
     if(ack==0)return(0);
   Stop_I2c();  
}

