
/******************************ADS1292R驱动程序*******************************/

#include <msp430x14x.h>
#include "ADS1292.h"
#include "Delay.h"

unsigned char a[12];
unsigned char b[12];

unsigned char regdata [7]={0x02,0Xab,0x10,0x05,0x05,0x00,0x00};//二通道心电测试信号，无呼吸0x02,0xeb,0x18,0x00,0x00,0x30,0x0f,0x40,0x02,0x01,0x03fCLK = 512 kHz
unsigned char regdata1[3]={0x02,0x02,0x01};
unsigned char regdata2[11]={0x00};

void Stop_Read_Data_Continuous (void)
{
    unsigned int i=0;
    SPI_ENABLE;                                // /CS enable
    for(i=0;i<10;i++);
    while (!(IFG2&UTXIFG1));                   // Wait for TXBUF ready
    TXBUF1 =0x11;                              // Send SDATAC
    SPI_DISABLE;                               // /CS disable
    for(i=0;i<10;i++);

}


void Start_Read_Data_Continuous (void)
{
    unsigned int i=0;
    SPI_ENABLE;
    for(i=0;i<10;i++);
    while (!(IFG2&UTXIFG1));                  // Wait for TXBUF ready
    TXBUF1 =0x10;                              // Send RDATAC
    SPI_DISABLE;
    for(i=0;i<10;i++);

}


void SPI_Regwrite(unsigned char addr,unsigned char data,unsigned char count)   
{
  unsigned int i=0,j=0;
  Stop_Read_Data_Continuous ();
  SPI_ENABLE;                               // /CS enable
  while (!(IFG2&UTXIFG1));                  // Wait for TXBUF ready
///  TXBUF1 =addr+0x40;                        //发送地址信息;  首地址是0x01;(zhao)
  TXBUF1 =addr;
  
  while (!(IFG2&UTXIFG1));                  // Wait for TXBUF ready
  TXBUF1 =count-1;                          //发送header;  //发送的寄存器数量为10个（zhao）
   for(j=0;j<500;j++);
   for(j=0;j<500;j++);
  for (i = 0; i<count;i++)   
  {
    while (!(IFG2&UTXIFG1));                 // Wait for TXBUF ready
    TXBUF1 =data;    
  }
  
  SPI_DISABLE;         //  CS disable
  //Start_Read_Data_Continuous ();
 
}




void SPI_RegRead(unsigned char addr, unsigned char *buffer, unsigned char count)   
{   
  unsigned int i=0,j=0;
  
/*  
  Stop_Read_Data_Continuous ();     //When in RDATAC mode, the RREG command is ignored.Send SDATAC MUST BE SENT
  for(i=0;i<10;i++);
  SPI_ENABLE;                              // /CS enable
  for(j=0;j<10;j++);
  while (!(IFG2&UTXIFG1));                 // Wait for TXBUF ready
///  TXBUF1 =addr|0x20;                       // Send address
  TXBUF1 =addr;
  
  while (!(IFG2&UTXIFG1));                 // Wait for TXBUF ready
  TXBUF1  = 0x00|(count-1);                //  regedit count 
*/ 
  
  SPI_ENABLE;
  

  for (i = 0; i<count;i++)   
  { 
   
    while (!(IFG2&UTXIFG1));               // Wait for TXBUF ready    
//    TXBUF1  = 0x00;                         //Initiate next data RX, meanwhile..
    TXBUF1 =addr;
    
    Delay_ms(20);
    
    while (!(IFG2&URXIFG1));               // Wait for TXBUF ready  
    buffer[i] =  RXBUF1;                   // Store data from last data RX
///    TXBUF0= buffer[i];
    for(j=0;j<10;j++);   
     
  }
  
  for(i=0;i<10;i++);

//  Start_Read_Data_Continuous ();
///  for(i=0;i<10;i++);
 
  
  SPI_DISABLE;                             // /CS disable
 
}


////////////////////////////////ADS1292R上电复位 ///////////////////////////////
void ADS_PowerOnInit(void)
{
        unsigned char ECG[9];
//        unsigned char ECG[12];
	START_H;
	SPI_DISABLE;
//        SPI_ENABLE;
        SCLK_SEL_H;
	REST_L;                     //进入掉电模式
	Delay_ms(1000);
	REST_H;                     //退出掉电模式
	Delay_ms(1000);                 //等待稳定
	REST_L;                     //发出复位脉冲 
	Delay_us(500);
	REST_H;
	Delay_ms(1000);                 //等待稳定，可以开始使用ADS1292R
	START_L;
	SPI_ENABLE;
        
    
	Stop_Read_Data_Continuous ();            //停止连续读取数据命令

        SPI_Regwrite(WREG|CONFIG2,0XA0,1);        //使用内部参考电压
	         

	Delay_ms(1000);                          //等待内部参考电压稳定

        SPI_Regwrite(WREG|CONFIG1,0X02,1);       //设置转换速率为500SPS
	         
        
       
/*       
        SPI_Regwrite(WREG|CH1SET,0X01,1);         //设置通道1为短路
	         
        SPI_Regwrite(WREG|CH2SET,0X01,1);        //设置通道2为短路
	                 
        
         START_H;
    
         Start_Read_Data_Continuous ();
    
         Delay_us(700);
         SPI_RegRead(0x00,ECG,9);
*/
                 
        
/*      
/////////////////// ADS1292内部测试信号配置/////////////////////////////////////
        START_L;
        Stop_Read_Data_Continuous ();        
        SPI_Regwrite(WREG|CONFIG2,0XA3,1);
        SPI_Regwrite(WREG|CH1SET,0X05,1);
        SPI_Regwrite(WREG|CH2SET,0X05,1);
        START_H;
        Start_Read_Data_Continuous ();       
/////////////////// ADS1292内部测试信号配置/////////////////////////////////////
*/
        


/////////////////// ADS1292连接导联/////////////////////////////////////////////
//        START_L;
//        Stop_Read_Data_Continuous ();            //停止连续读取数据命令 
        
//        SPI_Regwrite(WREG|CH1SET,0X20,1);        //连接导联,2倍增益
        SPI_Regwrite(WREG|CH1SET,0X00,1);          //连接导联,6倍增益


//        SPI_Regwrite(WREG|CH2SET,0X20,1);        //连接导联,2倍增益
        SPI_Regwrite(WREG|CH2SET,0X00,1);          //连接导联,6倍增益
        
        START_H;
        Start_Read_Data_Continuous ();
        
        Delay_us(700);
        SPI_RegRead(0x00,ECG,9);        
/////////////////// ADS1292连接导联/////////////////////////////////////////////
        
                              		 	

}





/*

////////////////////////////测试MSP430与ADS1292的SPI通信////////////////////////

void ADS_SPI_test()

{
  //SPI_RegRead(0x00,a,12);       //读取初始寄存器，检验电路连接是否正确
  SPI_Regwrite(0x08,regdata1,3); //写入配置寄存器，
  Delay_ms(5);
  SPI_Regwrite(0x0b,regdata2,1); //写入配置寄存器，
  Delay_ms(5);
  
 
  Delay_ms(5);  
  SPI_Regwrite(0x01,regdata,7);  //写入配置寄存器，
  Delay_ms(5);
  SPI_RegRead(0x00,b,12);        // 再次读出寄存器   检验是否写入正确
  Delay_ms(5);

}  
  
////////////////////////////测试MSP430与ADS1292的SPI通信////////////////////////

//////////////////////////对ADS1292R内部寄存器进行操作 ////////////////////////

unsigned char ADS_REG(unsigned char com,unsigned data)
{
	unsigned char i;
	for(i=0;i<45;i++);
	ADS_SPI(com);
	for(i=0;i<45;i++);
	ADS_SPI(0X00);
	for(i=0;i<45;i++);
	if((com&0x20)==0x20)//判断是否为读寄存器指令
	{	ADS_SPI(0X00);
		for(i=0;i<45;i++);	
	}
	return (ADS_SPI(data));

}


///////////////////////////通过SPI总线与ADS1292R通信////////////////////////////

unsigned char ADS_SPI(unsigned char com)
{
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);//检测发送寄存器是否为空，上一个数据发送完成
	SPI_I2S_SendData(SPI1, com);//发送数据      
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);//检测接收标志是否非空，发送完成  
	return (SPI_I2S_ReceiveData(SPI1));	//返回接收到的数据
}


/////读取72位的数据1100+LOFF_STAT[4:0]+GPIO[1:0]+13个0+2CHx24位，共9字节///////	

void ADS_Read(unsigned char *data)
{	unsigned char i;
//	unsigned char j;
	ADS_CS_L;
	for(i=0;i<9;i++)
	{	*data=ADS_SPI(0X00);
		data++;		
	}
	ADS_CS_H;
}

*/
