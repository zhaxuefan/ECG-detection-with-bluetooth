
#include <msp430x14x.h>
#include "ADS1292.h"

void MCU_Periph_Init()
{
  
  WDTCTL = WDTPW + WDTHOLD;       //关闭看门狗
  
  P2DIR=0xff;		 
  P4DIR=0xff;
  P6DIR=0xff;
  

  P1DIR |= 0x00;                  //P1.5,6输入
  P1IE  |= 0x01;                  //P1.0   中断允许
  P1IES |= 0x01;                  //P1.0下降沿触发
  P1IFG  = 0x00;                  //中断标志位清零
  P1OUT  = 0x00;  
  P5SEL |= 0x0E;                  //设置P5端口为SPI模式
  
  P3DIR |= 0x0E;                  //1,2输出
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

