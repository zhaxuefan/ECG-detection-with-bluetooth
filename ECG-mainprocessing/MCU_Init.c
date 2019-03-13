
#include <msp430x14x.h>
#include "ADS1292.h"

void MCU_Periph_Init()
{
  
  WDTCTL = WDTPW + WDTHOLD;       //�رտ��Ź�
  
  P2DIR=0xff;		 
  P4DIR=0xff;
  P6DIR=0xff;
  

  P1DIR |= 0x00;                  //P1.5,6����
  P1IE  |= 0x01;                  //P1.0   �ж�����
  P1IES |= 0x01;                  //P1.0�½��ش���
  P1IFG  = 0x00;                  //�жϱ�־λ����
  P1OUT  = 0x00;  
  P5SEL |= 0x0E;                  //����P5�˿�ΪSPIģʽ
  
  P3DIR |= 0x0E;                  //1,2���
}
////////////////////////////////////////////////////////////////////////////////
void intSPI1()
{
  P5SEL |= 0x0E;                            // P5.1,2,3 SPI option select
  P5OUT &= ~0x01;
  P5DIR |= 0x01;     
  U1CTL  = CHAR + SYNC + MM + SWRST;         // 8-bit, SPI, Master
  U1TCTL =  SSEL1 + STC;                    // Polarity, SMCLK, 3-wire
  U1BR0  = 0xa0;                             // SPICLK = SMCLK/2
  U1BR1  = 0x00;
  U1MCTL = 0x00;
  ME2 |= USPIE1;                            // Module enable
  U1CTL &= ~SWRST;                          // SPI enable
  IE2 |= URXIE1 ;                           // RX and TX interrupt enable
}
void INT_enable()
{

  _EINT();
  
}



void intUART0()
{
  
  P3SEL |= 0x30;                            // P3.4,5 = USART0 TXD/RXD
  ME1   |= UTXE0 + URXE0;                   // Enable USART0 TXD/RXD
  UCTL0 |= CHAR;                            // 8-bit character
  UTCTL0|= SSEL0;                           // UCLK = ACLK
  UBR00  = 0x03;                            // 32k/9600 - 3.41
  UBR10  = 0x00;                            //
  UMCTL0 = 0x4A;                            // Modulation
  UCTL0 &= ~SWRST;                          // Initialize USART state machine
  IE1   |= URXIE0;                          // Enable USART0 RX interrupt

}

