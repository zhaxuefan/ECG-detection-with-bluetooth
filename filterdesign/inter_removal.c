			  
/****** ���㷨��������---����Ӧ���ģ�巨���ĵ�ͼ����Ӧ�õ�һ�ָĽ��㷨.pdf *****/

/************************************** Head Files ͷ�ļ� **********************************/
#include <reg52.h>
#include <absacc.h> 
#include <stdio.h>
#include <I2C.H>

/****************************************** ͷ�������� *********************************/
void Coherent_Model_Proc(void);                     //���ģ�巨����---------------ʵ��
void Start_Timer(void);                                                                 //���ö�ʱ��--------------------ʵ��
unsigned char ADC_Input_RcvByteData(unsigned char addr);                                //����ADC����-------------------ʵ��
bit ADC_Input_set_addr_contr(unsigned char addr,unsigned char contr);                   //׼������ADC����---------------ʵ��
bit Send_data_to_DAC(unsigned char addr,unsigned char contr,unsigned char s_data);      //��8λ���ݷ��͵�DAC------------ʵ��


/***************************************** �������� ************************************/
unsigned char k=8;             //�趨ֵ�������źŵĲ���Ƶ�ʽ����趨��kΪ������
//unsigned char fs=50*8;         //�źŵĲ���Ƶ��(Hz),fs=50*k
//unsigned char fg=50;           //��Ƶ���ŵ�Ƶ��(Hz)
unsigned int M=64;             //ģ�峤�ȣ��������׿����趨��MԽ�����Խխ��Ч��Խ�ã�����Ƶ���ŵ�Ƶ��Ҳ��һ����Χ����������M����Ϊ..64.128.256...  
int len=8*(64-1)+1;						//len=k*(2*M-1)----���еĳ���
///const unsigned int Fosc=12000000;            //����Ƶ��(Hz),12MHz
unsigned int time_set=12000000/400/12;    //��ʱ��Ԥ����

/******************************* �������� ******************************/
unsigned int data_addr;                //�������еĵ�ַ
int i,temp,index;                      //�м����          
unsigned char xdata Xdata[8*(64-1)+1]={0};			 //�˲�ǰ����

/************** �˿����� ****************/      ///����
#define  PCF8591 0x90        //8bit-AD-DA����оƬPCF8591�ĵ�ַ; PCF8591��ַ=1001-A2-A1-A0-R/W, R=1,W=0; 
                             //I2Cд����:���͵�ַ-���Ϳ�����-��������, I2C������:���͵�ַ-������
 sbit clock=P2^7;

/******************************** �жϺ������� **********************/
/*------�жϺ���������ʽ��
        void functionname(void) interrupt num  
���У�functionname���Լ�����ĺ������֣�num��Ӧʵ�ʵ��ж�������
*/
void Timer0_interrupt(void) interrupt 1         //�жϷ������1
{ 
	TH0 = (65535 - time_set) >> 8;      //������λ
	TL0 = (65535 - time_set) & 0xff;    //������λ
	ADC_Input_set_addr_contr(PCF8591,0x40);           //ѡ��ADCģ������ͨ��0
	Xdata[data_addr]=ADC_Input_RcvByteData(PCF8591);
	Coherent_Model_Proc();                            //t~=6*M/10^6, If M<150, t<1ms, k<20����ʵʱ����fs<1kHz������(��Mȷ����MԽ��ʵʱ�����½�)
}

/******************************************** ������ *********************************/
void main(void)
{
	data_addr=0;       
	Start_Timer();		  //������ʱ��

	while(1);
}

void Coherent_Model_Proc(void)        //���ģ�巨����
{
	temp=0;
	for(i=0;i<M;i++)
	{
		index=data_addr+i*k;      //Xdata(index),index=n+k*i
		//index=index%len;
		if(index<len)temp+=Xdata[index];
		else
		{
			 temp+=Xdata[index-len];
		}	
	}
	 
	Send_data_to_DAC(PCF8591,0x40,Xdata[data_addr]-temp/M+102);
	data_addr++;
	data_addr=data_addr%len;
	//if(data_addr<len);
	//else data_addr=0;
}

/*------------------------- DAC ------------------------*/
bit Send_data_to_DAC(unsigned char addr,unsigned char contr,  unsigned char s_data)
{
	Start_I2c();               //��������,׼���������ݸ�DAC
	SendByte(addr);            //����������ַ addr=0x90,д����
	if(ack==0)return(0);       //ʧ��!
	SendByte(contr);           //���Ϳ����ֽ�
	if(ack==0)return(0);       //ʧ��!
	SendByte(s_data);            //����DAC������  
	if(ack==0)return(0);       //ʧ��!
	Stop_I2c();                //��������,������DAC�ķ�������
	return(1);                 //�ɹ���done!
}

/*------------------------ Timer ----------------------*/
void Start_Timer(void)
{
	TMOD = 0x01;                      //��ʱ��0��������ʽ1
	TH0 = (65535 - time_set) >> 8;    //������λ
	TL0 = (65535 - time_set) & 0xff;  //������λ
	ET0 = 1;                          //��ʱ��0�ж�����
	EA = 1;                           //ȫ���ж�����
	TR0 = 1;                          //������ʱ��0
}

/*------------------------- ADC -----------------------*/
bit ADC_Input_set_addr_contr(unsigned char addr,unsigned char contr)
{
	Start_I2c();              //��������
	SendByte(addr);            //����������ַ addr=0x90, д����
	if(ack==0)return(0);
	SendByte(contr);              //��������
	if(ack==0)return(0);
	Stop_I2c();               //��������
	return(1);
}

/******* ADC���ֽ����ݺ��� ******/
unsigned char ADC_Input_RcvByteData(unsigned char addr)
{  
	unsigned char c;

	Start_I2c();           //��������
	SendByte(addr+1);      //����������ַ addr=0x91, ������
	if(ack==0)return(0);
	c=RcvByte();           //��ȡ����0

	Ack_I2c(1);            //���ͷǾʹ�λ
	Stop_I2c();            //��������
	return(c);
}