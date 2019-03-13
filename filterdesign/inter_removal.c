			  
/****** 该算法出自文献---自适应相干模板法在心电图机中应用的一种改进算法.pdf *****/

/************************************** Head Files 头文件 **********************************/
#include <reg52.h>
#include <absacc.h> 
#include <stdio.h>
#include <I2C.H>

/****************************************** 头函数声明 *********************************/
void Coherent_Model_Proc(void);                     //相干模板法处理---------------实现
void Start_Timer(void);                                                                 //设置定时器--------------------实现
unsigned char ADC_Input_RcvByteData(unsigned char addr);                                //接收ADC数据-------------------实现
bit ADC_Input_set_addr_contr(unsigned char addr,unsigned char contr);                   //准备接收ADC数据---------------实现
bit Send_data_to_DAC(unsigned char addr,unsigned char contr,unsigned char s_data);      //将8位数据发送到DAC------------实现


/***************************************** 参数设置 ************************************/
unsigned char k=8;             //设定值，根据信号的采样频率进行设定，k为正整数
//unsigned char fs=50*8;         //信号的采样频率(Hz),fs=50*k
//unsigned char fg=50;           //工频干扰的频率(Hz)
unsigned int M=64;             //模板长度，根据文献可以设定。M越大，阻带越窄，效果越好，但工频干扰的频率也有一定范围，根据文献M可设为..64.128.256...  
int len=8*(64-1)+1;						//len=k*(2*M-1)----序列的长度
///const unsigned int Fosc=12000000;            //晶振频率(Hz),12MHz
unsigned int time_set=12000000/400/12;    //定时器预置数

/******************************* 变量定义 ******************************/
unsigned int data_addr;                //数据序列的地址
int i,temp,index;                      //中间变量          
unsigned char xdata Xdata[8*(64-1)+1]={0};			 //滤波前数据

/************** 端口设置 ****************/      ///待定
#define  PCF8591 0x90        //8bit-AD-DA集成芯片PCF8591的地址; PCF8591地址=1001-A2-A1-A0-R/W, R=1,W=0; 
                             //I2C写数据:发送地址-发送控制字-发送数据, I2C读数据:发送地址-读数据
 sbit clock=P2^7;

/******************************** 中断函数设置 **********************/
/*------中断函数命名方式：
        void functionname(void) interrupt num  
其中，functionname是自己定义的函数名字，num对应实际的中断向量号
*/
void Timer0_interrupt(void) interrupt 1         //中断服务程序1
{ 
	TH0 = (65535 - time_set) >> 8;      //置数高位
	TL0 = (65535 - time_set) & 0xff;    //置数低位
	ADC_Input_set_addr_contr(PCF8591,0x40);           //选择ADC模拟输入通道0
	Xdata[data_addr]=ADC_Input_RcvByteData(PCF8591);
	Coherent_Model_Proc();                            //t~=6*M/10^6, If M<150, t<1ms, k<20，可实时处理fs<1kHz的数据(由M确定，M越大实时性能下降)
}

/******************************************** 主程序 *********************************/
void main(void)
{
	data_addr=0;       
	Start_Timer();		  //启动定时器

	while(1);
}

void Coherent_Model_Proc(void)        //相干模板法处理
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
	Start_I2c();               //启动总线,准备发送数据给DAC
	SendByte(addr);            //发送器件地址 addr=0x90,写数据
	if(ack==0)return(0);       //失败!
	SendByte(contr);           //发送控制字节
	if(ack==0)return(0);       //失败!
	SendByte(s_data);            //发送DAC的数据  
	if(ack==0)return(0);       //失败!
	Stop_I2c();                //结束总线,结束对DAC的发送数据
	return(1);                 //成功，done!
}

/*------------------------ Timer ----------------------*/
void Start_Timer(void)
{
	TMOD = 0x01;                      //定时器0，工作方式1
	TH0 = (65535 - time_set) >> 8;    //置数高位
	TL0 = (65535 - time_set) & 0xff;  //置数低位
	ET0 = 1;                          //定时器0中断允许
	EA = 1;                           //全局中断允许
	TR0 = 1;                          //启动定时器0
}

/*------------------------- ADC -----------------------*/
bit ADC_Input_set_addr_contr(unsigned char addr,unsigned char contr)
{
	Start_I2c();              //启动总线
	SendByte(addr);            //发送器件地址 addr=0x90, 写数据
	if(ack==0)return(0);
	SendByte(contr);              //发送数据
	if(ack==0)return(0);
	Stop_I2c();               //结束总线
	return(1);
}

/******* ADC读字节数据函数 ******/
unsigned char ADC_Input_RcvByteData(unsigned char addr)
{  
	unsigned char c;

	Start_I2c();           //启动总线
	SendByte(addr+1);      //发送器件地址 addr=0x91, 读数据
	if(ack==0)return(0);
	c=RcvByte();           //读取数据0

	Ack_I2c(1);            //发送非就答位
	Stop_I2c();            //结束总线
	return(c);
}