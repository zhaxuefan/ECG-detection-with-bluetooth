
/*************************DS1292R驱动程序头文件***************************/
#ifndef __ADS1292_H
#define __ADS1292_H

#include <msp430x14x.h>
#include "delay.h"
#include <stdio.h>

/******************************接口宏定义*********************************/

//      #define SPI_ENABLE      P2OUT &=~0x08;
//      #define  SPI_DISABLE    P2OUT |= 0x08;

    #define  SPI_ENABLE      P5OUT &=~0x01;
    #define  SPI_DISABLE     P5OUT |= 0x01;


    #define  START_H         P3OUT |= 0x02;      //RST高电平
    #define  START_L         P3OUT &= ~0x02;      //RST低电平

    #define  REST_H          P3OUT |= 0x04;      //START高电平
    #define  REST_L          P3OUT &= ~0x04;      //START低电平

    #define  SCLK_SEL_H      P3OUT |= 0x08;      //SCLK_SEL高电平
    #define  SCLK_SEL_L      P3OUT &= ~0x08;      //SCLK_SEL低电平                            
                                            


/**************************ADS1292R命令定义*******************************/

	/**********************系统命令**************************/
	#define    WAKEUP	0X02	//从待机模式唤醒
	#define    STANDBY	0X04	//进入待机模式
	#define    RESET	0X06	//复位ADS1292R
	#define    START	0X08	//启动或转换
	#define    STOP	        0X0A	//停止转换
	#define    OFFSETCAL	0X1A	//通道偏移校准

	/**********************数据读取命令**********************/
	#define RDATAC	0X10	//启用连续的数据读取模式,默认使用此模式
	#define SDATAC	0X11	//停止连续的数据读取模式
	#define RDATA	0X12	//通过命令读取数据;支持多种读回。

	/**********************寄存器读取命令********************/
	#define	RREG	0X20	//读取001r rrrr 000n nnnn  这里定义的只有高八位，低8位在发送命令时设置
	#define WREG	0X40	//写入010r rrrr 000n nnnn
	/*     r rrrr=要读、写的寄存器地址       ;      n nnnn=要读、写的数据*/
		

/************************ADS1292R内部寄存器地址定义***********************/
	#define ID		0X00	//ID控制寄存器
	#define CONFIG1		0X01	//配置寄存器1
	#define CONFIG2		0X02	//配置寄存器2
	#define LOFF		0X03	//导联脱落控制寄存器
	#define CH1SET		0X04	//通道1设置寄存器
	#define CH2SET		0X05	//通道2设置寄存器
	#define RLD_SENS	0X06	//右腿驱动选择寄存器
	#define LOFF_SENS	0X07	//导联脱落检测选择寄存器
	#define LOFF_STAT	0X08	//导联脱落检测状态寄存器
	#define	RESP1		0X09	//呼吸检测控制寄存器1
	#define	RESP2		0X0A	//呼吸检测控制寄存器2
	#define	GPIO		0X0B	//GPIO控制寄存器


// void ADS_SPI_test();
 void INT_enable();
 void ADS_PowerOnInit(void);
 void SPI_RegRead(unsigned char addr, unsigned char *buffer, unsigned char count);
 void Start_Read_Data_Continuous (void);
 void Stop_Read_Data_Continuous (void);


#endif


