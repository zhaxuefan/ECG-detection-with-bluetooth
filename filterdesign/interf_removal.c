
/****** 该算法出自文献---自适应相干模板法在心电图机中应用的一种改进算法.pdf *****/

/************************************** Head Files 头文件 **********************************/
#include <reg52.h>
#include <absacc.h> 
#include <stdio.h>
#include <STC90.h>
#include <I2C.H>

/****************************************** 头函数声明 *********************************/
void Coherent_Model_Proc(void);                     //相干模板法处理---------------实现
//char ReadRamData(int data_addr);                  //读取RAM中8位的ADC数据--------实现
//int ReadRamData(int data_addr);                   //读取RAM中32位的ADC数据
//void WriteRamData(int data_addr,char data);       //将8位的ADC数据写入RAM中------实现
//void writeRamData(int data_addr,int data);        //将32位的ADC数据写入RAM中
//void send_data_to_DAC(int data);                  //将32位数据发送到DAC
//void initADC(void);                               //初始化ADC--------------------实现
//void start_ADC(void);                             //启动一次ADC转换--------------实现
//void set_Time(void);                              //设置定时器-------------------实现
//void InitDAC(void);                               //初始化DAC转换器--------------实现
//void Start_DAC(void);                             //启动DAC转换器----------------实现
//void send_data_to_DAC(char data);                 //将8位数据发送到DAC-----------实现
void Start_Timer(void);                                                                 //设置定时器--------------------实现
unsigned char ADC_Input_RcvByteData(unsigned char addr);                                //接收ADC数据-------------------实现
bit ADC_Input_set_addr_contr(unsigned char addr,unsigned char contr);                   //准备接收ADC数据---------------实现
bit Send_data_to_DAC(unsigned char addr,unsigned char contr,  unsigned char data);      //将8位数据发送到DAC------------实现


/***************************************** 参数设置 ************************************/
const unsigned char k=4;             //设定值，根据信号的采样频率进行设定，k为正整数
const unsigned char fs=50*k;         //信号的采样频率(Hz),fs=50*k
//T=1/fs;                            //信号的采样周期(s)
const unsigned char fg=50;           //工频干扰的频率(Hz)
//FT=1/50;                           //工频干扰的周期(s),FT=k*T
//const unsigned int K=n>>2;         //K=floor(n/k)
const unsigned int M=64;             //模板长度，根据文献可以设定。M越大，阻带越窄，效果越好，但工频干扰的频率也有一定范围，根据文献M可设为..64.128.256...
//if(M>K) M=K;                       //模板长度不能超过干扰的周期个数
const unsigned int len=k*(2*M-1);    //len=k*(2*M-1)----序列的长度
const unsigned int half=k*(M-1);     //half=k*(M-1)-----序列的中间
const unsigned int Fosc=12000000;            //晶振频率(Hz),12MHz
const unsigned int time_set=Fosc/(fs*12);    //定时器预置数

/******************************* 变量定义 ******************************/
//char ADC_data;                       //ADC数据
//unsigned int ADC_data_to_RAM_addr;   //从ADC获取数据存到RAM中的地址
unsigned int data_addr;                //数据序列的地址
bit start_proc_flag;                   //开始进行相干模板处理的标志
int i,temp,index;                      //中间变量
//int index;
static unsigned int xdata Xdata[len]={0};           //滤波前数据

/************** 端口设置 ****************/      ///待定
//sbit ST=P3^0; 
//sbit OE=P3^1; 
//sbit EOC=P3^2; 
//sbit CLK=P3^3;
//#define CS1 P0_3
//#define WR	P0_4
//#define XFER P0_7
//#define CS2 P2_6
//#define DOUT P1            //DAC output data port
//#define RAM_ADDR 0xff00    //定义外部RAM的初始地址

#define  PCF8591 0x90        //8bit-AD-DA集成芯片PCF8591的地址; PCF8591地址=1001-A2-A1-A0-R/W, R=1,W=0; 
                             //I2C写数据:发送地址-发送控制字-发送数据, I2C读数据:发送地址-读数据


/******************************** 中断函数设置 **********************/
/*------中断函数命名方式：
        void functionname(void) interrupt num  
其中，functionname是自己定义的函数名字，num对应实际的中断向量号
*/
void Timer0_interrupt(void) interrupt 1         //中断服务程序1
{ 
	TH0 = (65535 - time_set) >> 8;      //置数高位
	TL0 = (65535 - time_set) & 0xff;    //置数低位
	//start_ADC();
	
	ADC_Input_set_addr_contr(PCF8591,0x40);           //选择ADC模拟输入通道0
	Xdata[data_addr]=ADC_Input_RcvByteData(PCF8591);
	Coherent_Model_Proc();                            //t~=6*M/10^6, If M<150, t<1ms, k<20，可实时处理fs<1kHz的数据(由M确定，M越大实时性能下降)
	
}

/*
void time1_interrupt(void) interrupt 0     //中断服务程序2,中断源INT0。如果所选单片机有ADC中断源可以设为相应的中断函数：interrupt ADC中断向量号
{ 
	OE=1;
	ADC_data=P0;
	OE=0;
	WriteRamData(ADC_data_to_RAM_addr,ADC_data);    //将数据写入RAM中
	Coherent_Model_Proc();                          //相干模板处理
}
*/


/******************************************** 主程序 *********************************/
void main(void)
{
	//P0=oxff;
	start_proc_flag=0;
	//ADC_data_to_RAM_addr=0;
	data_addr=0;
	
	//initADC();           //初始化ADC
	//InitDAC();           //初始化DAC
	//set_Time();          //启动定时器
	Start_Timer();
	
	while(1);
	
	//X=sin([1:10000]*T*2*pi);      //一维数据向量
	//interf=zeros(1,10000);
	//interf(1:k:10000)=0.2;        //周期脉冲噪声信号,幅度0.2
	//Xdata=X+interf;               //采样得到需要处理的数据(混入噪声),一维向量

	//n=numel(Xdata);     //数据的个数
	
	//t=n*T;              //总采样时间（间隔T采样一个数据）
	//K=floor(t/FT);      //下取整，包含K个干扰周期的信号，K=floor(n/k)
	
	//unsigned int K=n>>2;  //K=floor(n/k)
	
	//Xdata=Xdata(1:K*FT/T);    //修剪掉不够一个FT周期的多余的那部分
	//n=numel(Xdata);

	//unsigned int M=64;       //模板长度，根据文献可以设定。M越大，阻带越窄，效果越好，但工频干扰的频率也有一定范围，根据文献M可设为..64.128.256...
	//if(M>K) M=K;     //模板长度不能超过干扰的周期个数

	//Tn=zeros(1,n);        //模板信号,T(n)=N(n)

	//int temp=0;
	/*
	for(i=M*k;i<n;i++)       //for(i=M*k+1;i<=n;i++) ???
	{
		int temp=0;
		for(j=0;j<M;j++)     //for(j=1;j<=M;j++) ???
		{
			int data_addr=i-k*j;
			//temp=temp+Xdata(i-k*l);
			temp+=ReadRamData(data_addr);
		}
		//Tn(i)=temp>>6;                //M=64=2^6,Tn(i)=temp/M,即右移6位
		//Sdata(i)=Xdata(i)-Tn(i);      //滤波后的信号
		
		//char Sdata=ReadRamData(i)-temp>>6;       //要发送到DAC的数据
		//int Sdata=ReadRamData(i)-temp>>6;       //要发送到DAC的数据
		//send_data_to_DAC(data);
		send_data_to_DAC(ReadRamData(i)-temp>>6);     //将滤波后数据发送到DAC
	}

	for(i=0;i<M*k;i++)             //for(i=1;i<=M*k;i++) ???
	{
		int temp=0;
		for(j=0;j<M;j++)           //for(j=1;j<=M;j++) ???
		{
			int data_addr=i+k*j;
			//temp=temp+Xdata(i+index);
			temp+=ReadRamData(data_addr);
		}
		//Tn(i)=temp/M;
		send_data_to_DAC(ReadRamData(i)-temp>>6);     //将滤波后数据发送到DAC
	}
	*/
	//Sdata=Xdata-Tn;        /*最终的信号*/
}



/************************************** 头函数实现 ***********************************/

/*-------------------- Coherent Model Process -----------------*/
/*
void Coherent_Model_Proc(void)        //相干模板法处理
{
	if(ADC_data_to_RAM_addr==half)
	{
		start_proc_flag=1;
	}
	
	if(start_proc_flag)
	{
		temp=0;                   //temp=sum(Xdata(n-k*i)),so T(n)=temp/M
		for(i=0;i<M;i++)
		{
			int data_ram_addr=ADC_data_to_RAM_addr-k*i;      //Xdata(index),index=n-k*i
			temp+=ReadRamData(data_ram_addr);                //sum of Xdata(index) by index=n-k*i for i=0,1,...,M-1
		}
		if(ADC_data_to_RAM_addr>=half)
		{
			Xdata_addr=ADC_data_to_RAM_addr-half;
		}
		else
		{
			Xdata_addr=ADC_data_to_RAM_addr+half+k;
		}
		send_data_to_DAC(ReadRamData(Xdata_addr)-temp/M);     //将滤波后数据(one data)发送到DAC,  Sdata(n)=Xdata(n)-T(n),send Sdata to DAC
	}
	
	ADC_data_to_RAM_addr++;
	ADC_data_to_RAM_addr%=len;
}
*/
void Coherent_Model_Proc(void)        //相干模板法处理
{
	if(data_addr==half)
	{
		start_proc_flag=1;
	}
	
	if(start_proc_flag)
	{
		temp=0;                 //temp=sum(Xdata(n-k*i)),so T(n)=temp/M
		//int index;
		for(i=0;i<M;i++)
		{
			index=data_addr-k*i;      //Xdata(index),index=n-k*i
			temp+=Xdata[index];                //sum of Xdata(index) by index=n-k*i for i=0,1,...,M-1
		}
		if(data_addr>=half)
		{
			index=data_addr-half;
		}
		else
		{
			index=data_addr+half+k;
		}
		Send_data_to_DAC(PCF8591,0x40,Xdata(index)-temp/M);     //将滤波后数据(one data)发送到DAC,  Sdata(n)=Xdata(n)-T(n),send Sdata to DAC
	
	}
	
	data_addr++;
	data_addr%=len;
}

/*------------------------- DAC ------------------------*/
bit Send_data_to_DAC(unsigned char addr,unsigned char contr,  unsigned char data)
{
	Start_I2c();               //启动总线,准备发送数据给DAC
	SendByte(addr);            //发送器件地址 addr=0x90,写数据
	if(ack==0)return(0);       //失败!
	SendByte(contr);           //发送控制字节
	if(ack==0)return(0);       //失败!
	SendByte(data);            //发送DAC的数据  
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


/*-------------------- Timer ----------------*/
/*
void set_Time(void)
{
	TMOD = 0x01;                      //定时器0，工作方式1
	TH0 = (65535 - time_set) >> 8;    //置数高位
	TL0 = (65535 - time_set) & 0xff;  //置数低位
	ET0 = 1;                          //定时器0中断允许
	EA = 1;                           //全局中断允许
	TR0 = 1;                          //启动定时器0
}
*/
/*------------------- ADC ----------------*/
/*
void initADC(void)
{
	ST=0;
	OE=0;
}

void start_ADC(void)
{
	ST=1; 
	ST=0;
}
*/
/*-------------------- DAC -----------------*/
/*
void InitDAC(void)
{
	CS1 = 1;	  				// 置高Cs1
	CS2 = 1;	  				// 置高Cs2
	WR = 0;						// WR有效
}

void Start_DAC(void)
{
	XFER = 0;						// XFER有效启动DA转换
	_nop_();						// _nop_
	XFER = 1;						// XFER恢复高电平
}

void send_data_to_DAC(char data)                 //将8位数据发送到DAC
{
	CS1 = 0;					// CS1有效；
	_nop_();					// nop
	DOUT = DAC;					// 写DAC寄存器
	_nop_();					// nop
	CS1 = 1;					// CS1无效
	Start_DAC();
}
*/
/*--------------------- RAM --------------------*/
/*
char ReadRamData(int data_addr)                  //读取RAM中8位的ADC数据
{
	char data=XBYTE[RAM_ADDR+data_addr];
	return data;
}

void WriteRamData(int data_addr,char data)       //将8位的ADC数据写入RAM中
{
	XBYTE[RAM_ADDR+data_addr]=data;
}
*/
