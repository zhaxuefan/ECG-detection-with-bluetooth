#include "msp430x14x.h"
#include "string.h"
#include "Delay.h"
#include "ADS1292R.h"
ADS1292DataTypeDef ADS1292Data;
#define ECGGain 6 //定义ADC增益6
unsigned char ADS1292_Default_Test_Register_Settings[15] = {
        //Device ID read Ony
	0x00,
        //CONFIG1
	0x01,//250sps//	0x00,//125 sps
        //CONFIG2
	0xE3,    //lead-off enable,Vref enable,Vref=2.42V,int_test enable,test wave = square
        //LOFF,lead-off control register
        0xF0,//enable lead-off,at dc lead-off detect
	//CH1SET (PGA gain = 6)
	0x25,//0x25,Gain=2,test signal//0x05,Gain=6,test signal//0x65,Gain=12,test signal
	//CH2SET (PGA gain = 6)
	0x25,//0x25,Gain=2,test signal//0x05,Gain=6,test signal//0x65,Gain=12,test signal
	//RLD_SENS (default)
	0x2F,       //2C,0x2f RLD from IN1+/- and IN2+/-    
	//LOFF_SENS (default)
	0x3F,       //Ch1 and Ch2 Lead-off enable
        //LOFF_STAT
        0x00,
        //RESP1
        0x02,//ADS1291 and ADS1292 must be 0x02
	//RESP2
	0x05,   //0x03 32Khz 0x07 64Khz //ADS1291 and ADS1292 must be 0x05 or 0x07
	//GPIO
        0x0C 
};		


unsigned char 	ADS1292R_Default_Test_Register_Settings[15] = {
	//Device ID read Ony
	0x00,
   	//CONFIG1
	0x01,//250sps//	0x00,//125 sps
        //CONFIG2
	0xE3,    //lead-off enable,Vref enable,Vref=2.42V,int_test enable,test wave = square
        //LOFF,lead-off control register
        0xF0,//enable lead-off,at dc lead-off detect
	//CH1SET (PGA gain = 6)
	0x25,//0x25,Gain=2,test signal//0x05,Gain=6,test signal//0x65,Gain=12,test signal
	//CH2SET (PGA gain = 6)
	0x25,//0x25,Gain=2,test signal//0x05,Gain=6,test signal//0x65,Gain=12,test signal
	//RLD_SENS (default)
	0x2F,       //0x2C RLD from IN2+/-,0x2F RLD from IN1+/- and IN2+/-    
	//LOFF_SENS (default)
	0x3F,       //Ch1 and Ch2 Lead-off enable
        //LOFF_STAT
        0x00,
        //RESP1
	0x02,//0x02,0xDE,0xEA,0xC2
	//RESP2
	0x03,   //0x03,32KHz  0x07,64KHz
	//GPIO
        0x0C 
};		


unsigned char 	ADS1292R_Default_Register_Settings[15] = {
	//Device ID read Ony
	0x00,
   	//CONFIG1
	0x01,//250sps//	0x00,//125 sps
        //CONFIG2
	0xE0,//lead-off enable,Vref enable,Vref=2.42V,int_test disable
        //LOFF,lead-off control register
        0xF0,//enable lead-off,at dc lead-off detect
	//CH1SET PGA gain = 6)
	0x00,//Gain=6,Normal electrode input
	//CH2SET PGA gain = 6)
	0x00,//Gain=6,Normal electrode input
	//RLD_SENS (default)
	0x2F,       //2C,0x2f RLD from IN1+/- and IN2+/-    
	//LOFF_SENS (default)
	0x3F,       //Ch1 and Ch2 Lead-off enable
        //LOFF_STAT
        0x00,
        //RESP1
        0xC2,//disable resp, 0xDE,    //EA DE
	//RESP2
	0x03,   //03 32Khz   07 64Khz
	//GPIO
        0x0C 
};	


uint8_t ADS_SPI(uint8_t byte)
{
        uint8_t buffer ; 
        while (!(IFG2&UTXIFG1));    // Wait for TXBUF ready    
        TXBUF1  = byte;             //Initiate next data RX, meanwhile       
        while (!(IFG2&URXIFG1));    // Wait for TXBUF ready  
        buffer =  RXBUF1;        // Store data from last data RX
        return(buffer);
        
}


void ADS1292_Reset()
{
        REST_L;                 //(ADS_RESET)=0
	ADS1292_Delay(50);	//delay 50ms
	REST_H ;	        //(ADS_RESET)=1  
}

//发送命令给ADS1292
void ADS1292_WriteCMD(uint8_t CMD)
{
	unsigned char i;
        SPI_ENABLE;	//Enable ADS1292
	for(i=0;i<10;i++);        
	ADS_SPI(CMD);//直接发送命令即可
	for(i=0;i<10;i++);      
	SPI_DISABLE;	//Disable ADS1292
}


uint8_t ADS1292_ReadReg(uint8_t reg)
{
	uint8_t tmp,i;
	ADS1292_WriteCMD(ADS1292_CMD_STOP);//发送停止转换命令
	ADS1292_WriteCMD(ADS1292_CMD_SDATAC);//发送停止连续读取命令
        SPI_ENABLE;
        for(i=0;i<10;i++);
	ADS_SPI(ADS1292_CMD_RREG | reg);//First opcode byte: 001r rrrr, where r rrrr is the starting register address.
	for(i=0;i<10;i++);
	ADS_SPI(0x00);//Second opcode byte: 000n nnnn, where n nnnn is the number of registers to read – 1.
	for(i=0;i<10;i++);
	tmp = ADS_SPI(0x00);//read reg
	for(i=0;i<10;i++);
        SPI_DISABLE;             //Disable ADS1292
	ADS1292_WriteCMD(ADS1292_CMD_START);//发送开始转换命令
	ADS1292_WriteCMD(ADS1292_CMD_RDATAC);//发送连续读取命令，
	return tmp;	
}


void ADS1292_WriteReg(uint8_t reg,uint8_t val)
{
	unsigned char i;
	ADS1292_WriteCMD(ADS1292_CMD_STOP);//发送停止转换命令
        SPI_ENABLE;
        for(i=0;i<10;i++);
	ADS_SPI(ADS1292_CMD_WREG | reg);//First opcode byte: 010r rrrr, where r rrrr is the starting register address.
        for(i=0;i<10;i++);
	ADS_SPI(0x00);//Second opcode byte: 000n nnnn, where n nnnn is the number of registers to write – 1.
	for(i=0;i<10;i++);
	ADS_SPI(val);//write reg
	for(i=0;i<10;i++); 
        SPI_DISABLE;
	ADS1292_WriteCMD(ADS1292_CMD_START);//发送开始转换命令
}



//实现一次性将默认的配置数组写到芯片中，除ID外
void ADS1292_DefaultSet(uint8_t *buf)
{
	unsigned char i,j;
	ADS1292_WriteCMD(ADS1292_CMD_STOP);//发送停止转换命令
	ADS1292_WriteCMD(ADS1292_CMD_SDATAC);//发送停止连续读取命令
	SPI_ENABLE;
        for(i=0;i<10;i++);
	ADS_SPI(ADS1292_CMD_WREG | ADS1292_CONFIG1);//First opcode byte: 010r rrrr, where r rrrr is the starting register address.
	for(i=0;i<10;i++);
	ADS_SPI(10);//Second opcode byte: 000n nnnn, where n nnnn is the number of registers to write – 1.	0x01 - 0x0B
	for(i=0;i<10;i++);
	buf++;//ignore ID 跳过ID，写剩下的寄存器
	for(j=1;j<12;j++)
	{
		ADS_SPI(*buf);
		buf++;
                for(i=0;i<10;i++);
	}
        for(i=0;i<10;i++);
        SPI_DISABLE;
	ADS1292_WriteCMD(ADS1292_CMD_START);//发送开始转换命令
	ADS1292_WriteCMD(ADS1292_CMD_RDATAC);//发送连续读取命令，
}

//实现一次性将写到芯片的配置数组读出
void ADS1292_DefaultRead(uint8_t *buf)
{
	unsigned char i,j;
	ADS1292_WriteCMD(ADS1292_CMD_STOP);//发送停止转换命令
	ADS1292_WriteCMD(ADS1292_CMD_SDATAC);//发送停止连续读取命令
	SPI_ENABLE;
	for(i=0;i<10;i++);
        ADS_SPI(ADS1292_CMD_RREG | ADS1292_ID);//First opcode byte: 010r rrrr, where r rrrr is the starting register address.        
        for(i=0;i<10;i++);
	ADS_SPI(10);//Second opcode byte: 000n nnnn, where n nnnn is the number of registers to write – 1.	0x01 - 0x0B
	for(i=0;i<10;i++);
	for(j=0;j<12;j++)
	{
		*buf=ADS_SPI(0x00);
		buf++;
                for(i=0;i<10;i++);			
	}
        for(i=0;i<10;i++);
        SPI_DISABLE;
	ADS1292_WriteCMD(ADS1292_CMD_RDATAC);//发送连续读取命令，
}


void ADS1292_Read(uint8_t *buf)
{ 
	unsigned char i;
	unsigned char j;
        SPI_ENABLE;
        for(j=0;j<10;j++);
	for(i=0;i<9;i++)
	{
		*buf=ADS_SPI(0x00);//Second opcode byte: 000n nnnn, where n nnnn is the number of registers to write – 1.
                for(j=0;j<10;j++);
		buf++;		
	}
                
        for(j=0;j<10;j++);
        SPI_DISABLE;	
}

//ADS1292自检函数
//当连续读取的SelfTestQuantity的数值均为0时，
//认为ADS1291没有配置成功，则一直配置，直到成功为止。
 

void ADS1292_Init()
{
/////////////////////////////////my/////////////////////////////////////////////
        START_H;
	SPI_DISABLE;
        SCLK_SEL_H;
	REST_L;                     //进入掉电模式
	ADS1292_Delay(1000);
	REST_H;                     //退出掉电模式
	ADS1292_Delay(1000);                 //等待稳定
	REST_L;                     //发出复位脉冲 
	Delay_us(500);
	REST_H;
	ADS1292_Delay(1000);                 //等待稳定，可以开始使用ADS1292R
	START_L;
	SPI_ENABLE;
/////////////////////////////////my///////////////////////////////////////////// 
	ADS1292_DefaultSet(ADS1292R_Default_Register_Settings);//启用默认配置

}



//将采集的心电数据，转化为正负数值
void ADS1292ECGData_ConvertToInt(uint8_t *inputbuf, int32_t *ECG1Out, int32_t *ECG2Out)
{
	int32_t tmp;
	tmp = inputbuf[3];
	tmp = (tmp<<8) + inputbuf[4];
	tmp = (tmp<<8) + inputbuf[5];
	if(tmp >=  0x800000) //>=0x8000000 为负数，则-0xFFFFFF,将对应的正数变成负数
	{
		tmp = tmp - 0xFFFFFF;
	}
	else//<0x800000为正数，不用处理
	{
	}

	*ECG1Out = tmp;

	tmp = inputbuf[6];
	tmp = (tmp<<8) + inputbuf[7];
	tmp = (tmp<<8) + inputbuf[8];

	if(tmp >=  0x800000) //>=0x8000000 为负数，则-0xFFFFFF,将对应的正数变成负数
	{
		tmp = tmp - 0xFFFFFF;
	}
	else//<0x800000为正数，不用处理
	{
	}
	*ECG2Out = tmp;
}

//将采集的心电数据，所有数均转化为正数
//输出为0x00000000 - 0x00FFFFFF
void ADS1292ECGData_ConvertToUint(uint8_t *inputbuf, uint32_t *ECG1Out, uint32_t *ECG2Out)
{
	uint32_t tmp;
	tmp = inputbuf[3];
	tmp = (tmp<<8) + inputbuf[4];
	tmp = (tmp<<8) + inputbuf[5];
	if(tmp >=  0x800000) //负数原始范围0x800000-0xFFFFFF,
	{
		tmp = tmp - 0x800000;//故需要减0x800000，将负数范围变成0x000000-0x7FFFFF
	}
	else				//正数原始范围0x000000-0x7FFFFF,
	{
		tmp = tmp + 0x800000;//故需要加0x800000，将正数范围变成0x800000-0xFFFFFF
	}
	*ECG1Out = tmp;

	tmp = inputbuf[6];
	tmp = (tmp<<8) + inputbuf[7];
	tmp = (tmp<<8) + inputbuf[8];
	if(tmp >=  0x800000) //负数原始范围0x800000-0xFFFFFF,
	{
		tmp = tmp - 0x800000;//故需要减0x800000，将负数范围变成0x000000-0x7FFFFF
	}
	else				//正数原始范围0x000000-0x7FFFFF,
	{
		tmp = tmp + 0x800000;//故需要加0x800000，将正数范围变成0x800000-0xFFFFFF
	}
	*ECG2Out = tmp;
}

//将带符号的ADC值改成带符号的电压值
//电压值的单位为uV
void ADS1292ECGData_ConvertToVoltage(int32_t ECG1ADCIn, int32_t ECG2ADCIn, int32_t *ECG1Out, int32_t *ECG2Out)
{
	float ratio;

	ratio = (float)(ECG1ADCIn/ECGGain)/(float)(0x7FFFFF);//计算比值
	*ECG1Out = (int32_t)(ratio*2420000);//ADC基准电压2.42V，输出电压单位为uV

	ratio = (float)(ECG2ADCIn/ECGGain)/(float)(0x7FFFFF);//计算比值
	*ECG2Out = (int32_t)(ratio*2420000);//ADC基准电压2.42V，输出电压单位为uV
}

//从获取的72bit(9字节)心电数据中，获取心电导联状态
//心电数据格式：3字节状态+3字节CH1数据+3字节CH2数据
//3字节状态数据格式：the 24 status bits is: (1100 + LOFF_STAT[4:0] + GPIO[1:0] + 13 '0's).
//LOFF_STAT[4:0],bit4 = RLD_STAT, bit3 = IN2N_OFF, bit2 = IN2P_OFF, bit1 = IN1N_OFF, bit0 = IN1P_OFF
//输入数组*inputbuf 9字节,输出*outputbuf 5字节
void ADS1292_GetStatus(uint8_t *inputbuf,uint8_t *outputbuf)
{
	uint8_t tmp1,tmp2;
	tmp1 = *inputbuf;//获取第1字节数据
	inputbuf++;
	tmp2 = *inputbuf;//获取第2字节数据
 
	if(tmp1 && 0x08) *outputbuf = 1;//RLD_STAT
	else *outputbuf = 0;
	outputbuf++;

	if(tmp1 && 0x04) *outputbuf = 1;//IN2N_OFF
	else *outputbuf = 0;
	outputbuf++;

	if(tmp1 && 0x02) *outputbuf = 1;//IN2P_OFF
	else *outputbuf = 0;
	outputbuf++;

	if(tmp1 && 0x01) *outputbuf = 1;//IN1N_OFF
	else *outputbuf = 0;
	outputbuf++;

	if(tmp2 && 0x80) *outputbuf = 1;//IN1P_OFF
	else *outputbuf = 0;
}


//完成心电相关所有数据处理 
void ADS1292_ECGData_Process(uint8_t *inputbuf)
{
	ADS1292Data.updata  = 1;//有数据更新
	ADS1292Data.ID = *inputbuf;
	ADS1292ECGData_ConvertToUint(inputbuf,&ADS1292Data.CH1_U_ADC,&ADS1292Data.CH2_U_ADC);//转化为无符号ADC值
	ADS1292ECGData_ConvertToInt(inputbuf,&ADS1292Data.CH1_ADC,&ADS1292Data.CH2_ADC);	 //转化为带符号ADC值
	ADS1292ECGData_ConvertToVoltage(ADS1292Data.CH1_ADC,ADS1292Data.CH2_ADC,&ADS1292Data.CH1_Voltage,&ADS1292Data.CH2_Voltage);//转化为带符号电压值
	ADS1292_GetStatus(inputbuf,ADS1292Data.LeadoffStatus);								 //获取导联状态
	memcpy(&ADS1292Data.readbuf,inputbuf,32);//每个ADS1292采样数据为32字节		//将当前的值复制到结构体 buf中
}

