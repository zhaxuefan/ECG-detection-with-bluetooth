#include "msp430x14x.h"
#include "string.h"
#include "Delay.h"
#include "ADS1292R.h"
ADS1292DataTypeDef ADS1292Data;
#define ECGGain 6 //����ADC����6
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

//���������ADS1292
void ADS1292_WriteCMD(uint8_t CMD)
{
	unsigned char i;
        SPI_ENABLE;	//Enable ADS1292
	for(i=0;i<10;i++);        
	ADS_SPI(CMD);//ֱ�ӷ��������
	for(i=0;i<10;i++);      
	SPI_DISABLE;	//Disable ADS1292
}


uint8_t ADS1292_ReadReg(uint8_t reg)
{
	uint8_t tmp,i;
	ADS1292_WriteCMD(ADS1292_CMD_STOP);//����ֹͣת������
	ADS1292_WriteCMD(ADS1292_CMD_SDATAC);//����ֹͣ������ȡ����
        SPI_ENABLE;
        for(i=0;i<10;i++);
	ADS_SPI(ADS1292_CMD_RREG | reg);//First opcode byte: 001r rrrr, where r rrrr is the starting register address.
	for(i=0;i<10;i++);
	ADS_SPI(0x00);//Second opcode byte: 000n nnnn, where n nnnn is the number of registers to read �C 1.
	for(i=0;i<10;i++);
	tmp = ADS_SPI(0x00);//read reg
	for(i=0;i<10;i++);
        SPI_DISABLE;             //Disable ADS1292
	ADS1292_WriteCMD(ADS1292_CMD_START);//���Ϳ�ʼת������
	ADS1292_WriteCMD(ADS1292_CMD_RDATAC);//����������ȡ���
	return tmp;	
}


void ADS1292_WriteReg(uint8_t reg,uint8_t val)
{
	unsigned char i;
	ADS1292_WriteCMD(ADS1292_CMD_STOP);//����ֹͣת������
        SPI_ENABLE;
        for(i=0;i<10;i++);
	ADS_SPI(ADS1292_CMD_WREG | reg);//First opcode byte: 010r rrrr, where r rrrr is the starting register address.
        for(i=0;i<10;i++);
	ADS_SPI(0x00);//Second opcode byte: 000n nnnn, where n nnnn is the number of registers to write �C 1.
	for(i=0;i<10;i++);
	ADS_SPI(val);//write reg
	for(i=0;i<10;i++); 
        SPI_DISABLE;
	ADS1292_WriteCMD(ADS1292_CMD_START);//���Ϳ�ʼת������
}



//ʵ��һ���Խ�Ĭ�ϵ���������д��оƬ�У���ID��
void ADS1292_DefaultSet(uint8_t *buf)
{
	unsigned char i,j;
	ADS1292_WriteCMD(ADS1292_CMD_STOP);//����ֹͣת������
	ADS1292_WriteCMD(ADS1292_CMD_SDATAC);//����ֹͣ������ȡ����
	SPI_ENABLE;
        for(i=0;i<10;i++);
	ADS_SPI(ADS1292_CMD_WREG | ADS1292_CONFIG1);//First opcode byte: 010r rrrr, where r rrrr is the starting register address.
	for(i=0;i<10;i++);
	ADS_SPI(10);//Second opcode byte: 000n nnnn, where n nnnn is the number of registers to write �C 1.	0x01 - 0x0B
	for(i=0;i<10;i++);
	buf++;//ignore ID ����ID��дʣ�µļĴ���
	for(j=1;j<12;j++)
	{
		ADS_SPI(*buf);
		buf++;
                for(i=0;i<10;i++);
	}
        for(i=0;i<10;i++);
        SPI_DISABLE;
	ADS1292_WriteCMD(ADS1292_CMD_START);//���Ϳ�ʼת������
	ADS1292_WriteCMD(ADS1292_CMD_RDATAC);//����������ȡ���
}

//ʵ��һ���Խ�д��оƬ�������������
void ADS1292_DefaultRead(uint8_t *buf)
{
	unsigned char i,j;
	ADS1292_WriteCMD(ADS1292_CMD_STOP);//����ֹͣת������
	ADS1292_WriteCMD(ADS1292_CMD_SDATAC);//����ֹͣ������ȡ����
	SPI_ENABLE;
	for(i=0;i<10;i++);
        ADS_SPI(ADS1292_CMD_RREG | ADS1292_ID);//First opcode byte: 010r rrrr, where r rrrr is the starting register address.        
        for(i=0;i<10;i++);
	ADS_SPI(10);//Second opcode byte: 000n nnnn, where n nnnn is the number of registers to write �C 1.	0x01 - 0x0B
	for(i=0;i<10;i++);
	for(j=0;j<12;j++)
	{
		*buf=ADS_SPI(0x00);
		buf++;
                for(i=0;i<10;i++);			
	}
        for(i=0;i<10;i++);
        SPI_DISABLE;
	ADS1292_WriteCMD(ADS1292_CMD_RDATAC);//����������ȡ���
}


void ADS1292_Read(uint8_t *buf)
{ 
	unsigned char i;
	unsigned char j;
        SPI_ENABLE;
        for(j=0;j<10;j++);
	for(i=0;i<9;i++)
	{
		*buf=ADS_SPI(0x00);//Second opcode byte: 000n nnnn, where n nnnn is the number of registers to write �C 1.
                for(j=0;j<10;j++);
		buf++;		
	}
                
        for(j=0;j<10;j++);
        SPI_DISABLE;	
}

//ADS1292�Լ캯��
//��������ȡ��SelfTestQuantity����ֵ��Ϊ0ʱ��
//��ΪADS1291û�����óɹ�����һֱ���ã�ֱ���ɹ�Ϊֹ��
 

void ADS1292_Init()
{
/////////////////////////////////my/////////////////////////////////////////////
        START_H;
	SPI_DISABLE;
        SCLK_SEL_H;
	REST_L;                     //�������ģʽ
	ADS1292_Delay(1000);
	REST_H;                     //�˳�����ģʽ
	ADS1292_Delay(1000);                 //�ȴ��ȶ�
	REST_L;                     //������λ���� 
	Delay_us(500);
	REST_H;
	ADS1292_Delay(1000);                 //�ȴ��ȶ������Կ�ʼʹ��ADS1292R
	START_L;
	SPI_ENABLE;
/////////////////////////////////my///////////////////////////////////////////// 
	ADS1292_DefaultSet(ADS1292R_Default_Register_Settings);//����Ĭ������

}



//���ɼ����ĵ����ݣ�ת��Ϊ������ֵ
void ADS1292ECGData_ConvertToInt(uint8_t *inputbuf, int32_t *ECG1Out, int32_t *ECG2Out)
{
	int32_t tmp;
	tmp = inputbuf[3];
	tmp = (tmp<<8) + inputbuf[4];
	tmp = (tmp<<8) + inputbuf[5];
	if(tmp >=  0x800000) //>=0x8000000 Ϊ��������-0xFFFFFF,����Ӧ��������ɸ���
	{
		tmp = tmp - 0xFFFFFF;
	}
	else//<0x800000Ϊ���������ô���
	{
	}

	*ECG1Out = tmp;

	tmp = inputbuf[6];
	tmp = (tmp<<8) + inputbuf[7];
	tmp = (tmp<<8) + inputbuf[8];

	if(tmp >=  0x800000) //>=0x8000000 Ϊ��������-0xFFFFFF,����Ӧ��������ɸ���
	{
		tmp = tmp - 0xFFFFFF;
	}
	else//<0x800000Ϊ���������ô���
	{
	}
	*ECG2Out = tmp;
}

//���ɼ����ĵ����ݣ���������ת��Ϊ����
//���Ϊ0x00000000 - 0x00FFFFFF
void ADS1292ECGData_ConvertToUint(uint8_t *inputbuf, uint32_t *ECG1Out, uint32_t *ECG2Out)
{
	uint32_t tmp;
	tmp = inputbuf[3];
	tmp = (tmp<<8) + inputbuf[4];
	tmp = (tmp<<8) + inputbuf[5];
	if(tmp >=  0x800000) //����ԭʼ��Χ0x800000-0xFFFFFF,
	{
		tmp = tmp - 0x800000;//����Ҫ��0x800000����������Χ���0x000000-0x7FFFFF
	}
	else				//����ԭʼ��Χ0x000000-0x7FFFFF,
	{
		tmp = tmp + 0x800000;//����Ҫ��0x800000����������Χ���0x800000-0xFFFFFF
	}
	*ECG1Out = tmp;

	tmp = inputbuf[6];
	tmp = (tmp<<8) + inputbuf[7];
	tmp = (tmp<<8) + inputbuf[8];
	if(tmp >=  0x800000) //����ԭʼ��Χ0x800000-0xFFFFFF,
	{
		tmp = tmp - 0x800000;//����Ҫ��0x800000����������Χ���0x000000-0x7FFFFF
	}
	else				//����ԭʼ��Χ0x000000-0x7FFFFF,
	{
		tmp = tmp + 0x800000;//����Ҫ��0x800000����������Χ���0x800000-0xFFFFFF
	}
	*ECG2Out = tmp;
}

//�������ŵ�ADCֵ�ĳɴ����ŵĵ�ѹֵ
//��ѹֵ�ĵ�λΪuV
void ADS1292ECGData_ConvertToVoltage(int32_t ECG1ADCIn, int32_t ECG2ADCIn, int32_t *ECG1Out, int32_t *ECG2Out)
{
	float ratio;

	ratio = (float)(ECG1ADCIn/ECGGain)/(float)(0x7FFFFF);//�����ֵ
	*ECG1Out = (int32_t)(ratio*2420000);//ADC��׼��ѹ2.42V�������ѹ��λΪuV

	ratio = (float)(ECG2ADCIn/ECGGain)/(float)(0x7FFFFF);//�����ֵ
	*ECG2Out = (int32_t)(ratio*2420000);//ADC��׼��ѹ2.42V�������ѹ��λΪuV
}

//�ӻ�ȡ��72bit(9�ֽ�)�ĵ������У���ȡ�ĵ絼��״̬
//�ĵ����ݸ�ʽ��3�ֽ�״̬+3�ֽ�CH1����+3�ֽ�CH2����
//3�ֽ�״̬���ݸ�ʽ��the 24 status bits is: (1100 + LOFF_STAT[4:0] + GPIO[1:0] + 13 '0's).
//LOFF_STAT[4:0],bit4 = RLD_STAT, bit3 = IN2N_OFF, bit2 = IN2P_OFF, bit1 = IN1N_OFF, bit0 = IN1P_OFF
//��������*inputbuf 9�ֽ�,���*outputbuf 5�ֽ�
void ADS1292_GetStatus(uint8_t *inputbuf,uint8_t *outputbuf)
{
	uint8_t tmp1,tmp2;
	tmp1 = *inputbuf;//��ȡ��1�ֽ�����
	inputbuf++;
	tmp2 = *inputbuf;//��ȡ��2�ֽ�����
 
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


//����ĵ�����������ݴ��� 
void ADS1292_ECGData_Process(uint8_t *inputbuf)
{
	ADS1292Data.updata  = 1;//�����ݸ���
	ADS1292Data.ID = *inputbuf;
	ADS1292ECGData_ConvertToUint(inputbuf,&ADS1292Data.CH1_U_ADC,&ADS1292Data.CH2_U_ADC);//ת��Ϊ�޷���ADCֵ
	ADS1292ECGData_ConvertToInt(inputbuf,&ADS1292Data.CH1_ADC,&ADS1292Data.CH2_ADC);	 //ת��Ϊ������ADCֵ
	ADS1292ECGData_ConvertToVoltage(ADS1292Data.CH1_ADC,ADS1292Data.CH2_ADC,&ADS1292Data.CH1_Voltage,&ADS1292Data.CH2_Voltage);//ת��Ϊ�����ŵ�ѹֵ
	ADS1292_GetStatus(inputbuf,ADS1292Data.LeadoffStatus);								 //��ȡ����״̬
	memcpy(&ADS1292Data.readbuf,inputbuf,32);//ÿ��ADS1292��������Ϊ32�ֽ�		//����ǰ��ֵ���Ƶ��ṹ�� buf��
}

