
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
�������ƣ�Init_Clock
��    �ܣ�����ϵͳʱ��
��    ������
����ֵ  ����
********************************************/
void Init_Clock(void)
{
  
   uchar i;
  /*------ѡ��ϵͳ��ʱ��Ϊ8MHz-------*/
    BCSCTL1 &= ~XT2OFF;                 //��XT2��Ƶ��������

    do
    {
        IFG1 &= ~OFIFG;                 //�������ʧ�ܱ�־
        for (i = 0xFF; i > 0; i--);     //�ȴ�8MHz��������
    }
   while ((IFG1 & OFIFG));             //����ʧЧ��־��Ȼ���ڣ�

    
    BCSCTL2 |= SELM_2 + SELS;           //MCLK��SMCLKѡ���Ƶ����
}



int main()
{	
//	unsigned char i=0 ;
   WDTCTL = WDTPW + WDTHOLD;       //�رտ��Ź�
        Init_Clock();
        
	MCU_Periph_Init();	//��ʼ��MCU�ڲ�
                
        TFT_Initial();           //��ʼ��LCD
        CLR_Screen(white);
        delayms(50);  //��ʱһ��ʱ��              	                                 
        intSPI1();              //SPI1��ʼ��
        intUART0();             //UART0��ʼ��        
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




//////////////////////////P1�жϣ��Զ���PC����������///////////////////////////

#pragma vector = PORT1_VECTOR               //P1�жϷ����ӳ���
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
                                 
                    if(ADS1292Data.updata == 1)//һ��ADS1292���ݲɼ���ɣ����������˲�
                      {
                            denoising(ADS1292Data.readbuf);//ȥ��              
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
                 

 P1IFG &=~0x01;//�����־λ
  
}   


