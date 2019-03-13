
#include <msp430x14x.h>
#include <stdio.h>
#include "string.h"
#include "MCU_Init.h"
#include  "TFT.h"
#include  "font.h"
//#include "ADS1292.h"
#include "ADS1292R.h"
#include "Delay.h"
#include "denoising.h"

char MST_Data = 0xAA,SLV_Data = 0xff;

unsigned char ECG[9]={0X91,0X92,0X93,0X94,0X95,0X96,0X97,0X98,0X99};
unsigned char In[15]={0x00,0x01,0xE3,0xF0,0x25,0x25,0x2F,0x3F,0x00,0x02,0x05,0x0C};
unsigned char Out[9];

unsigned char DRDY_flag=0;
uint16_t  y=0,j=0;

/*******************************************
函数名称：Init_Clock
功    能：设置系统时钟
参    数：无
返回值  ：无
********************************************/
void Init_Clock(void)
{
  
   uchar i;
  /*------选择系统主时钟为8MHz-------*/
    BCSCTL1 &= ~XT2OFF;                 //打开XT2高频晶体振荡器

    do
    {
        IFG1 &= ~OFIFG;                 //清除晶振失败标志
        for (i = 0xFF; i > 0; i--);     //等待8MHz晶体起振
    }
   while ((IFG1 & OFIFG));             //晶振失效标志仍然存在？

    
    BCSCTL2 |= SELM_2 + SELS;           //MCLK和SMCLK选择高频晶振
}



int main()
{	
//	unsigned char i=0 ;
   WDTCTL = WDTPW + WDTHOLD;       //关闭看门狗
        Init_Clock();
        
	MCU_Periph_Init();	//初始化MCU内部
                
        TFT_Initial();           //初始化LCD
        CLR_Screen(white);
        delayms(50);  //延时一段时间              	                                 
        intSPI1();              //SPI1初始化
        intUART0();             //UART0初始化        
        ADS1292_Init();
          
////////////////////////////////////////////////////////////////////////////////
        LCD_WriteBMP(200, 250, 5, 1, gImage_hhh1);
        delayms(1000); 
        delayms(1000);
////////////////////////////////////////////////////////////////////////////////
        _EINT();
              
	while(1)
	{      
        }	       
}




        
#pragma vector=UART0RX_VECTOR
__interrupt void usart0_rx (void)
{
   while (!(IFG1 & UTXIFG0));                // USART0 TX buffer ready?
                                             // RXBUF0 to TXBUF0
  _BIC_SR_IRQ(LPM3_bits);                   // Clear LPM3 bits from 0(SR)
}




//////////////////////////P1中断：自动向PC机发送数据///////////////////////////

#pragma vector = PORT1_VECTOR               //P1中断服务子程序
__interrupt void P1_Interrupt(void)
{
  
static uint8_t buf[32]={0x00,0x00,0x00,0x00,0x0F,0xA0,0x00,0x00,0xFF}; 
///////////////////////////////MY///////////////////////////////////////////////
                for(y=0;y<320;y++)
		{
                  
                    if(P1IFG==0x01)
                      {   
                        ADS1292_Read(buf);
                        ADS1292_ECGData_Process(buf);           
                       }
                                 
                    if(ADS1292Data.updata == 1)//一次ADS1292数据采集完成，进行数据滤波
                      {
                            denoising(ADS1292Data.readbuf);//去噪              
                            LCD_WriteBMP(ADS1292Data.CH1_Voltage-120, y, 1, 1, gImage_hhh0);
                            LCD_WriteBMP(ADS1292Data.CH2_Voltage-120, y, 1, 1, gImage_hhh0);
                            int i;
                            for(i=0;i<32;i++){
                              TXBUF0=ADS1292Data.readbuf[i];
                              delayms(100);
                            }
                            
                            ADS1292Data.updata =  0;   
                          
                      }                                                                            
		}
                
                CLR_Screen(black);
///////////////////////////////MY///////////////////////////////////////////////
                 

 P1IFG &=~0x01;//清除标志位
  
}   


