
/******************************ADS1292R��������*******************************/

#include <msp430x14x.h>
#include "ADS1292.h"
#include "Delay.h"

unsigned char a[12];
unsigned char b[12];

unsigned char regdata [7]={0x02,0Xab,0x10,0x05,0x05,0x00,0x00};//��ͨ���ĵ�����źţ��޺���0x02,0xeb,0x18,0x00,0x00,0x30,0x0f,0x40,0x02,0x01,0x03fCLK = 512 kHz
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
///  TXBUF1 =addr+0x40;                        //���͵�ַ��Ϣ;  �׵�ַ��0x01;(zhao)
  TXBUF1 =addr;
  
  while (!(IFG2&UTXIFG1));                  // Wait for TXBUF ready
  TXBUF1 =count-1;                          //����header;  //���͵ļĴ�������Ϊ10����zhao��
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


////////////////////////////////ADS1292R�ϵ縴λ ///////////////////////////////
void ADS_PowerOnInit(void)
{
        unsigned char ECG[9];
//        unsigned char ECG[12];
	START_H;
	SPI_DISABLE;
//        SPI_ENABLE;
        SCLK_SEL_H;
	REST_L;                     //�������ģʽ
	Delay_ms(1000);
	REST_H;                     //�˳�����ģʽ
	Delay_ms(1000);                 //�ȴ��ȶ�
	REST_L;                     //������λ���� 
	Delay_us(500);
	REST_H;
	Delay_ms(1000);                 //�ȴ��ȶ������Կ�ʼʹ��ADS1292R
	START_L;
	SPI_ENABLE;
        
    
	Stop_Read_Data_Continuous ();            //ֹͣ������ȡ��������

        SPI_Regwrite(WREG|CONFIG2,0XA0,1);        //ʹ���ڲ��ο���ѹ
	         

	Delay_ms(1000);                          //�ȴ��ڲ��ο���ѹ�ȶ�

        SPI_Regwrite(WREG|CONFIG1,0X02,1);       //����ת������Ϊ500SPS
	         
        
       
/*       
        SPI_Regwrite(WREG|CH1SET,0X01,1);         //����ͨ��1Ϊ��·
	         
        SPI_Regwrite(WREG|CH2SET,0X01,1);        //����ͨ��2Ϊ��·
	                 
        
         START_H;
    
         Start_Read_Data_Continuous ();
    
         Delay_us(700);
         SPI_RegRead(0x00,ECG,9);
*/
                 
        
/*      
/////////////////// ADS1292�ڲ������ź�����/////////////////////////////////////
        START_L;
        Stop_Read_Data_Continuous ();        
        SPI_Regwrite(WREG|CONFIG2,0XA3,1);
        SPI_Regwrite(WREG|CH1SET,0X05,1);
        SPI_Regwrite(WREG|CH2SET,0X05,1);
        START_H;
        Start_Read_Data_Continuous ();       
/////////////////// ADS1292�ڲ������ź�����/////////////////////////////////////
*/
        


/////////////////// ADS1292���ӵ���/////////////////////////////////////////////
//        START_L;
//        Stop_Read_Data_Continuous ();            //ֹͣ������ȡ�������� 
        
//        SPI_Regwrite(WREG|CH1SET,0X20,1);        //���ӵ���,2������
        SPI_Regwrite(WREG|CH1SET,0X00,1);          //���ӵ���,6������


//        SPI_Regwrite(WREG|CH2SET,0X20,1);        //���ӵ���,2������
        SPI_Regwrite(WREG|CH2SET,0X00,1);          //���ӵ���,6������
        
        START_H;
        Start_Read_Data_Continuous ();
        
        Delay_us(700);
        SPI_RegRead(0x00,ECG,9);        
/////////////////// ADS1292���ӵ���/////////////////////////////////////////////
        
                              		 	

}





/*

////////////////////////////����MSP430��ADS1292��SPIͨ��////////////////////////

void ADS_SPI_test()

{
  //SPI_RegRead(0x00,a,12);       //��ȡ��ʼ�Ĵ����������·�����Ƿ���ȷ
  SPI_Regwrite(0x08,regdata1,3); //д�����üĴ�����
  Delay_ms(5);
  SPI_Regwrite(0x0b,regdata2,1); //д�����üĴ�����
  Delay_ms(5);
  
 
  Delay_ms(5);  
  SPI_Regwrite(0x01,regdata,7);  //д�����üĴ�����
  Delay_ms(5);
  SPI_RegRead(0x00,b,12);        // �ٴζ����Ĵ���   �����Ƿ�д����ȷ
  Delay_ms(5);

}  
  
////////////////////////////����MSP430��ADS1292��SPIͨ��////////////////////////

//////////////////////////��ADS1292R�ڲ��Ĵ������в��� ////////////////////////

unsigned char ADS_REG(unsigned char com,unsigned data)
{
	unsigned char i;
	for(i=0;i<45;i++);
	ADS_SPI(com);
	for(i=0;i<45;i++);
	ADS_SPI(0X00);
	for(i=0;i<45;i++);
	if((com&0x20)==0x20)//�ж��Ƿ�Ϊ���Ĵ���ָ��
	{	ADS_SPI(0X00);
		for(i=0;i<45;i++);	
	}
	return (ADS_SPI(data));

}


///////////////////////////ͨ��SPI������ADS1292Rͨ��////////////////////////////

unsigned char ADS_SPI(unsigned char com)
{
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);//��ⷢ�ͼĴ����Ƿ�Ϊ�գ���һ�����ݷ������
	SPI_I2S_SendData(SPI1, com);//��������      
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);//�����ձ�־�Ƿ�ǿգ��������  
	return (SPI_I2S_ReceiveData(SPI1));	//���ؽ��յ�������
}


/////��ȡ72λ������1100+LOFF_STAT[4:0]+GPIO[1:0]+13��0+2CHx24λ����9�ֽ�///////	

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
