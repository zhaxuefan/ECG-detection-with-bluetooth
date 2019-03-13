
/*************************DS1292R��������ͷ�ļ�***************************/
#ifndef __ADS1292_H
#define __ADS1292_H

#include <msp430x14x.h>
#include "delay.h"
#include <stdio.h>

/******************************�ӿں궨��*********************************/

//      #define SPI_ENABLE      P2OUT &=~0x08;
//      #define  SPI_DISABLE    P2OUT |= 0x08;

    #define  SPI_ENABLE      P5OUT &=~0x01;
    #define  SPI_DISABLE     P5OUT |= 0x01;


    #define  START_H         P3OUT |= 0x02;      //RST�ߵ�ƽ
    #define  START_L         P3OUT &= ~0x02;      //RST�͵�ƽ

    #define  REST_H          P3OUT |= 0x04;      //START�ߵ�ƽ
    #define  REST_L          P3OUT &= ~0x04;      //START�͵�ƽ

    #define  SCLK_SEL_H      P3OUT |= 0x08;      //SCLK_SEL�ߵ�ƽ
    #define  SCLK_SEL_L      P3OUT &= ~0x08;      //SCLK_SEL�͵�ƽ                            
                                            


/**************************ADS1292R�����*******************************/

	/**********************ϵͳ����**************************/
	#define    WAKEUP	0X02	//�Ӵ���ģʽ����
	#define    STANDBY	0X04	//�������ģʽ
	#define    RESET	0X06	//��λADS1292R
	#define    START	0X08	//������ת��
	#define    STOP	        0X0A	//ֹͣת��
	#define    OFFSETCAL	0X1A	//ͨ��ƫ��У׼

	/**********************���ݶ�ȡ����**********************/
	#define RDATAC	0X10	//�������������ݶ�ȡģʽ,Ĭ��ʹ�ô�ģʽ
	#define SDATAC	0X11	//ֹͣ���������ݶ�ȡģʽ
	#define RDATA	0X12	//ͨ�������ȡ����;֧�ֶ��ֶ��ء�

	/**********************�Ĵ�����ȡ����********************/
	#define	RREG	0X20	//��ȡ001r rrrr 000n nnnn  ���ﶨ���ֻ�и߰�λ����8λ�ڷ�������ʱ����
	#define WREG	0X40	//д��010r rrrr 000n nnnn
	/*     r rrrr=Ҫ����д�ļĴ�����ַ       ;      n nnnn=Ҫ����д������*/
		

/************************ADS1292R�ڲ��Ĵ�����ַ����***********************/
	#define ID		0X00	//ID���ƼĴ���
	#define CONFIG1		0X01	//���üĴ���1
	#define CONFIG2		0X02	//���üĴ���2
	#define LOFF		0X03	//����������ƼĴ���
	#define CH1SET		0X04	//ͨ��1���üĴ���
	#define CH2SET		0X05	//ͨ��2���üĴ���
	#define RLD_SENS	0X06	//��������ѡ��Ĵ���
	#define LOFF_SENS	0X07	//����������ѡ��Ĵ���
	#define LOFF_STAT	0X08	//����������״̬�Ĵ���
	#define	RESP1		0X09	//���������ƼĴ���1
	#define	RESP2		0X0A	//���������ƼĴ���2
	#define	GPIO		0X0B	//GPIO���ƼĴ���


// void ADS_SPI_test();
 void INT_enable();
 void ADS_PowerOnInit(void);
 void SPI_RegRead(unsigned char addr, unsigned char *buffer, unsigned char count);
 void Start_Read_Data_Continuous (void);
 void Stop_Read_Data_Continuous (void);


#endif


