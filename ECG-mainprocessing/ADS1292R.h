#ifndef __ADS1292R_H_
#define __ADS1292R_H_

//#include "stm32f10x.h"
#include "msp430x14x.h"

/////////////////////////////MY-def/////////////////////////////////////////////

#define	uint8_t unsigned char
#define	uint16_t unsigned int
#define	int32_t signed long int
#define	uint32_t unsigned long int
#define	uchar unsigned char


/******************************接口宏定义*********************************/

    #define  SPI_ENABLE      P5OUT &=~0x01;
    #define  SPI_DISABLE     P5OUT |= 0x01;


    #define  START_H         P3OUT |= 0x02;      //RST高电平
    #define  START_L         P3OUT &= ~0x02;      //RST低电平

    #define  REST_H          P3OUT |= 0x04;      //START高电平
    #define  REST_L          P3OUT &= ~0x04;      //START低电平

    #define  SCLK_SEL_H      P3OUT |= 0x08;      //SCLK_SEL高电平
    #define  SCLK_SEL_L      P3OUT &= ~0x08;      //SCLK_SEL低电平                            
                                            
/////////////////////////////MY-def/////////////////////////////////////////////


//				  ADDRESS REGISTER resetval(Hex) BIT7	BIT6	BIT5	BIT4	BIT3	BIT2	BIT1	BIT0
//Device Settings (Read-Only Registers)
#define ADS1292_ID			0x00// 	ID 			XX 	   REV_ID7 REV_ID6 REV_ID5 	  1 	  0 	  0	   REV_ID1 REV_ID0

//Global Settings Across Channels
#define ADS1292_CONFIG1		0x01// CONFIG1 		02 	SINGLE_SHOT	  0 	  0		  0		  0		 DR2	 DR1	 DR0
#define ADS1292_CONFIG2 	0x02// CONFIG2 		80 		1  PDB_LOFF_COM PDB_REFBUF VREF_4V CLK_EN 0   INT_TEST  TEST_FREQ
#define ADS1292_LOFF		0x03// LOFF    		10 	COMP_TH2  COMP_TH1 COMP_TH0   1  ILEAD_OFF1 ILEAD_OFF0 0 FLEAD_OFF

//Channel-Specific Settings
#define ADS1292_CH1SET		0x04// CH1SET 		00 		PD1    GAIN1_2 GAIN1_1 GAIN1_0 	MUX1_3 	MUX1_2	MUX1_1 	MUX1_0
#define ADS1292_CH2SET		0x05// CH2SET 		00 		PD2	   GAIN2_2 GAIN2_1 GAIN2_0	MUX2_3	MUX2_2 	MUX2_1	MUX2_0
#define ADS1292_RLD_SENS 	0x06// RLD_SENS 	00		CHOP1 	CHOP0  PDB_RLD RLD_LOFF_SENS RLD2N RLD2P RLD1N RLD1P
#define ADS1292_LOFF_SENS	0x07// LOFF_SENS 	00 		   0	  0 	FLIP2	 FLIP1 	LOFF2N 	LOFF2P 	LOFF1N LOFF1P
#define ADS1292_LOFF_STAT	0x08// LOFF_STAT 	00 		   0   CLK_DIV	  0	    RLD_STAT IN2N_OFF IN2P_OFF IN1N_OFF IN1P_OFF
//GPIO and Other Registers
#define ADS1292_RESP1		0x09// RESP1 		00 RESP_DEMOD_EN1 RESP_MOD_EN RESP_PH3 RESP_PH2 RESP_PH1 RESP_PH0 1 RESP_CTRL
#define ADS1292_RESP2		0x0A// RESP2 		02 CALIB_ON 0 0 0 0 RESP_FREQ RLDREF_INT 1
#define ADS1292_GPIO		0x0B// GPIO 		0C 0 0 0 0 GPIOC2 GPIOC1 GPIOD2 GPIOD1

//ADS1292 System Commands
#define ADS1292_CMD_WAKEUP 		0x02	//Wake-up from standby mode
#define ADS1292_CMD_STANDBY 	0x04	//Enter standby mode
#define ADS1292_CMD_RESET 		0x06	//Reset the device
#define ADS1292_CMD_START 		0x08	//Start or restart (synchronize) conversions
#define ADS1292_CMD_STOP 		0x0A	//Stop conversion
#define ADS1292_CMD_OFFSETCAL 	0x1A	//Channel offset calibration
//Data Read Commands
#define ADS1292_CMD_RDATAC 		0x10	//Enable Read Data Continuous mode.This mode is the default mode at power-up.
#define ADS1292_CMD_SDATAC 		0x11	//Stop Read Data Continuously mode
#define ADS1292_CMD_RDATA 		0x12	//Read data by command; supports multiple read back.
//Register Read Commands
#define ADS1292_CMD_RREG		0x20	//Read  n nnnn registers starting at address r rrrr 001r rrrr
#define ADS1292_CMD_WREG		0x40	//Write n nnnn registers starting at address r rrrr 010r rrrr
#define Sampling_rate 250

typedef struct
{
	uint8_t updata;//状态，=1更新数据 =0数据已经读取
	uint8_t ID;// ADS1292 ID
	uint32_t CH1_U_ADC;// CH1无符号ADC数据
	uint32_t CH2_U_ADC;// CH2无符号ADC数据
	int32_t CH1_ADC;// CH1有符号ADC数据
	int32_t CH2_ADC;// CH2有符号ADC数据
	int32_t CH1_Voltage;// CH1电压，单位uV
	int32_t CH2_Voltage;// CH2电压，单位uV
	int32_t CH1_FilterVoltage;// CH1电压，单位uV
	int32_t CH2_FilterVoltage;// CH2电压，单位uV
	uint8_t readbuf[32];// 32 bytes data read from ADS1292
	uint8_t LeadoffStatus[5];//5 bytes byte0=RLD_STAT, byte1=IN2N_OFF, byte2=IN2P_OFF, byte3=IN1N_OFF, byte4=IN1P_OFF
}ADS1292DataTypeDef;


extern ADS1292DataTypeDef ADS1292Data;
void ADS1292_Init(void);

extern void ADS1292_Read(uint8_t *buf);
extern void ADS1292_ECGData_Process(uint8_t *inputbuf);
extern void ADS1292_WriteReg(uint8_t reg,uint8_t val);
extern void ADS1292_DefaultSet(uint8_t *buf);
extern void ADS1292_DefaultRead(uint8_t *buf);


#endif //__ADS1292R_H

