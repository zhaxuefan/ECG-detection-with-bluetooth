
////////////////////////// 该算法出自文献：用于去除心电信号中工频干扰数字滤波技术.pdf(王镇) //////////////////////////

/////////////////////////////// FIR filter ////////////////////////////////

/////// Band-Pass Transfer function: H1(z)=1/Q^n*(1-z^(-R))^n/(1-z^(-P))^n,
/////// Band-Pass Frequence Respose: H1(w)=1/Q^n*sin(R/2*w)^n/sin(P/2*w)^n*exp(-j*(R-P)/2*n*w)
/////// Band-Stop Transfer function: H(z)=z^(n*(R-P)/2)-1/Q^n*(1-z^(-R))^n/(1-z^(-P))^n,
/////// Q=R/P
/////// If SampleRate fs=500Hz and Interference Frequency fg=50Hz,then P=fs/fg=10,
/////// For a good band-stop and analysis from our tests, we set n=2 and R=160,then Q=16
/////// so H(z)=z^(-150)-(1/16)^2*((1-z^(-160))/(1-z(-10)))^2
/////// The next step is designing a self-adaptive band-stop filter ???

//////////////////////////////////////////////////////////////////////////

/************************************** Head Files 头文件 **********************************/
#include <reg52.h>
#include <absacc.h> 
#include <stdio.h>
#include <STC90.h>
#include <I2C.H>

/************************************** 端口及器件地址设置 *********************************/
#define  PCF8591 0x90  //8bit-AD-DA集成芯片PCF8591的地址; PCF8591地址=1001-A2-A1-A0-R/W, R=1,W=0; 
                       //I2C写数据:发送地址-发送控制字-发送数据, I2C读数据:发送地址-读数据


/************************************** 头函数的声明 ***************************************/
bit ADC_Input_set_addr_contr(unsigned char addr,unsigned char contr);                   //准接收ADC数据-----------------实现
unsigned char ADC_Input_RcvByteData(unsigned char addr);                                //接收ADC数据-------------------实现
void Start_Timer(void);                                                                 //设置定时器--------------------实现
bit Send_data_to_DAC(unsigned char addr,unsigned char contr,  unsigned char data);      //将8位数据发送到DAC------------实现
void Band_Stop_Proc(void);                                                              //带阻滤波处理------------------实现

/************************************* Variable List 参数设置 ******************************/
//////// fs必须是fg的正整数倍
//fs=250;         %sample rate              (can change)
//fg=50;          %interference frequency   (can change)
//n=2;            %const                    (don't change)
//R=160;          %what meaning ?           (can change)
//P=fs/fg;        %what meaning ?  (const)  (decided by fs and fg)
//Q=R/P;          %what meaning ?  (const)  (decided by R and P)
const unsigned int fs=500;           //为了达到实时性要求，fs<5kHz, 为了不改变系数，fs<2500Hz, 要求fs是fg的正整数倍
const unsigned int fg=50;
const unsigned int n=2;
const unsigned int R=160;
const unsigned int P=fs/fg;
const unsigned int Q=R/P;

/************************************ 变量定义 **********************************************/
static unsigned int data_addr;                    //数据序列的地址
static unsigned int xdata y[nfilt]={0};           //滤波后数据
static unsigned int xdata x[nfilt]={0};           //滤波前数据
bit DataAddrLargerThan_nfilt_flag;       //前nfilt个数据处理不同于后面的数据处理，这里设置转变标志  

///////////// Band-Pass /////////////
/*    
b=zeros(1,321);
a=zeros(1,21);
b(1)=1/256;
b(161)=-2/256;
b(321)=1/256;

a(1)=1;
a(11)=-2;
a(21)=1;
*/

//////////// Band-Stop /////////////
//b=zeros(1,2*R+1);
//a=zeros(1,2*P+1);
//b(1)=-1/Q^2;
//b(R-P+1)=1;
//b(R+1)=-2*(Q^2-1)/Q^2;
//b(R+P+1)=1;
//b(2*R+1)=-1/Q^2;

//a(1)=1;
//a(P+1)=-2;
//a(2*P+1)=1;

/*************** 传递函数的各个参数 ***************/
/////////// R=160,P=10, R>P, so nfilt=2*R+1;
const int[] b_index={0,R-P,R,R+P,2*R};     //index must from small to large
const int[] b={-1,Q*Q,-2*(Q*Q-1),Q*Q,-1};     //Let Q=16, Q^2=2^8, after process signal>>8

const int[] a_index={0,P,2*P};                 //index must from small to large
const int[] a={Q*Q,-2*Q*Q,Q*Q};               //after process signal>>8

const int nfilt=2*R+1;
//const int na=2*P+1;
//const int nb=2*R+1;           //in here nb=nfilt, so we don't need to declare nb
const unsigned int Fosc=12000000;            //晶振频率(Hz),12MHz
const unsigned int time_set=Fosc/(fs*12);    //定时器预置数


/**************************************** 中断函数设置 **********************************************/
void timer0_interrupt(void) interrupt 1         //中断服务程序1
{ 
	TH0 = (65535 - time_set) >> 8;      //置数高位
	TL0 = (65535 - time_set) & 0xff;    //置数低位
	
	ADC_Input_set_addr_contr(PCF8591,0x40);         //选择ADC模拟输入通道0
	x[data_addr]=ADC_Input_RcvByteData(PCF8591);
	Band_Stop_Proc();                               //13次乘法,21次加法,每次乘法4个机器周期，每次加法1个机器周期，
	                                                //有73个机器周期，算上其他,在100个机器周期之内，每个机器周期12个时钟晶振,
													//t<100*12/Fosc=0.1ms, t<0.1ms, 故可以实时处理采样率fs<10kHz的噪声数据
}


/************************************************** 主程序 ******************************************/
void main(void)
{
	//////////////////// 初始化 //////////////////////////
	data_addr=0;
	DataAddrLargerThan_nfilt_flag=0;
	
	Start_Timer();
	
	while(1);
	/*
	while(1)
	{
		y[i]=0;
		if(DataAddrLargerThan_nfilt_flag==0)
		{
			if(i<a_index[1])                             //na=2*P+1  only j=0,b(0)=-1,a(0)=Q*Q
				y[i]+=b[0]*x[i]-a[0]*y[i];
			else if(i<a_index[2])                        //only j=0,j=P+1
				y[i]+=b[0]*x[i]-a[0]*y[i];
				y[i]-=a[1]*y[i-a_index[1]];
			else if(i<b_index[2])                        //j=R-P+1,j=R+1,j=R+P+1,j=2*R+1   
				y[i]+=b[0]*x[i]-a[0]*y[i];
				y[i]-=a[1]*y[i-a_index[1]];
				y[i]+=b[1]*x(i-b_index[1]);
			else if(i<b_index[3])
				y[i]+=b[0]*x[i]-a[0]*y[i];
				y[i]-=a[1]*y[i-a_index[1]];
				y[i]+=b[1]*x[i-b_index[1]];
				y[i]+=b[2]*x[i-b_index[2]];
			else if(i<b_index[4])
				y[i]+=b[0]*x[i]-a[0]*y[i];
				y[i]-=a[1]*y[i-a_index[1]];
				y[i]+=b[1]*x[i-b_index[1]];
				y[i]+=b[2]*x[i-b_index[2]];
				y[i]+=b[3]*x[i-b_index[3]];
			else
				y[i]+=b[0]*x[i]-a[0]*y[i];
				y[i]-=a[1]*y[i-a_index[1]];
				y[i]+=b[1]*x[i-b_index[1]];
				y[i]+=b[2]*x[i-b_index[2]];
				y[i]+=b[3]*x[i-b_index[3]];
				y[i]+=b[4]*x[i-b_index[4]];
		}
		
		else
		{
			y[i]+=b[0]*x[i]-a[0]*y[i];
			index=(i-a_index[1]+nfilt)%nfilt;
			y[i]-=a[1]*y[index];
			y[i]+=b[1]*x[index];
			index=(i-a_index[2]+nfilt)%nfilt;
			y[i]+=b[2]*x[index];
			index=(i-a_index[3]+nfilt)%nfilt;
			y[i]+=b[3]*x[index];
			index=(i-a_index[4]+nfilt)%nfilt;
			y[i]+=b[4]*x[index];
		}
		y[i]/=a[0];
		
		i++;
		if(i==nfilt)DataAddrLargerThan_nfilt_flag=1;
		i%=nfilt;
	}
	*/
}

/************************************************ 头函数实现 **************************************/

/*------------------------- Band-Stop-Proc ------------*/
void Band_Stop_Proc(void)
{
	y[data_addr]=0;
	if(!DataAddrLargerThan_nfilt_flag)                   //DataAddrLargerThan_nfilt_flag==0
	{
		if(data_addr<a_index[1])                             //na=2*P+1  only j=0,b(0)=-1,a(0)=Q*Q
			y[data_addr]+=b[0]*x[data_addr]-a[0]*y[data_addr];
		else if(data_addr<a_index[2])                        //only j=0,j=P+1
			y[data_addr]+=b[0]*x[data_addr]-a[0]*y[data_addr];
			y[data_addr]-=a[1]*y[data_addr-a_index[1]];
		else if(data_addr<b_index[2])                        //j=R-P+1,j=R+1,j=R+P+1,j=2*R+1   
			y[data_addr]+=b[0]*x[data_addr]-a[0]*y[data_addr];
			y[data_addr]-=a[1]*y[data_addr-a_index[1]];
			y[data_addr]+=b[1]*x(data_addr-b_index[1]);
		else if(data_addr<b_index[3])
			y[data_addr]+=b[0]*x[data_addr]-a[0]*y[data_addr];
			y[data_addr]-=a[1]*y[data_addr-a_index[1]];
			y[data_addr]+=b[1]*x[data_addr-b_index[1]];
			y[data_addr]+=b[2]*x[data_addr-b_index[2]];
		else if(data_addr<b_index[4])
			y[data_addr]+=b[0]*x[data_addr]-a[0]*y[data_addr];
			y[data_addr]-=a[1]*y[data_addr-a_index[1]];
			y[data_addr]+=b[1]*x[data_addr-b_index[1]];
			y[data_addr]+=b[2]*x[data_addr-b_index[2]];
			y[data_addr]+=b[3]*x[data_addr-b_index[3]];
		else
			y[data_addr]+=b[0]*x[data_addr]-a[0]*y[data_addr];
			y[data_addr]-=a[1]*y[data_addr-a_index[1]];
			y[data_addr]+=b[1]*x[data_addr-b_index[1]];
			y[data_addr]+=b[2]*x[data_addr-b_index[2]];
			y[data_addr]+=b[3]*x[data_addr-b_index[3]];
			y[data_addr]+=b[4]*x[data_addr-b_index[4]];
	}
	
	else
	{
		y[data_addr]+=b[0]*x[data_addr]-a[0]*y[data_addr];
		index=(data_addr-a_index[1]+nfilt)%nfilt;
		y[data_addr]-=a[1]*y[index];
		y[data_addr]+=b[1]*x[index];
		index=(data_addr-a_index[2]+nfilt)%nfilt;
		y[data_addr]+=b[2]*x[index];
		index=(data_addr-a_index[3]+nfilt)%nfilt;
		y[data_addr]+=b[3]*x[index];
		index=(data_addr-a_index[4]+nfilt)%nfilt;
		y[data_addr]+=b[4]*x[index];
	}
	y[data_addr]/=a[0];
	Send_data_to_DAC(PCF8591,0x40,y[data_addr]);
	
	data_addr++;
	if(data_addr==nfilt)DataAddrLargerThan_nfilt_flag=1;
	data_addr%=nfilt;
} 


/*------------------------- DAC------------------------*/
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

	Start_I2c();          //启动总线
	SendByte(addr+1);      //发送器件地址 addr=0x91, 读数据
	if(ack==0)return(0);
	c=RcvByte();          //读取数据0

	Ack_I2c(1);           //发送非就答位
	Stop_I2c();           //结束总线
	return(c);
}

/*
b=zeros(1,321);
a=zeros(1,21);
b(1)=-1/256;
b(151)=1;
b(161)=-510/256;
b(171)=1;
b(321)=-1/256;

a(1)=1;
a(11)=-2;
a(21)=1;
*/

/*
[H,F]=freqz(b,a,fs,'whole');         %幅频响应

hh=abs(H);
subplot(3,2,1),plot([1:fs],hh),title('Magnitude-Frequency Characteristics'),xlabel('Frequency(Hz)'),ylabel('Magnitude')

load ecgdata.mat               %加载数据
data=mydata';                  %采样的数据
t = (1:numel(data))/fs;
subplot(3,2,2),plot(t,data),title(strcat('Original Sequence(Sample rate:',num2str(fs),'Hz)')),xlabel('Time(s)'),ylabel('Amplitude'),axis([0 numel(data)/fs -2 2])       % 原始数据

noise=0.1*sin(2*pi*fg*t);      %噪声数据
subplot(3,2,3),plot(t,noise),title(strcat('Interfrence of Noise Sequence(Frequency:',num2str(fg),'Hz)')),xlabel('Time(s)'),ylabel('Amplitude'),axis([0 numel(data)/fs -2 2])

noise_data=data+noise;         %添加噪声
subplot(3,2,4),plot(t,noise_data),title('With noise Sequence'),xlabel('Time(s)'),ylabel('Amplitude'),axis([0 numel(data)/fs -2 2])

ecg=filter(b,a,noise_data);    %滤波后数据
subplot(3,2,6),plot(t,ecg),title('After Filter Sequence'),xlabel('Time(s)'),ylabel('Amplitude'),axis([0 numel(data)/fs -2 2])

drift=ecg((R-P+1):end)-data(1:end-(R-P));
subplot(3,2,5),plot(t((R-P+1):end),drift),title(strcat('Difference of before and after filter(delay:',num2str((R-P)/fs),'s)')),xlabel('Time(s)'),ylabel('Amplitude'),axis([0 numel(data)/fs -2 2])


ecg1=myfilter(b,a,noise_data);




//////////////////////////////////////////////
function y=myfilter(b,a,x)

na=numel(a);
nb=numel(b);
nx=numel(x);
nfilt=max(na,nb);

if (a(1)-1.0)>eps||(a(1)-1.0)<-eps
    for i=1:nb
        b(i)=b(i)/a(1);
    end
    for i=1:na
        a(i)=a(i)/a(1);
    end
end

yf=zeros(1,nx+nfilt);
xf=zeros(1,nx+nfilt);
xf(nfilt+1:nx+nfilt)=x(:);

af=zeros(1,nfilt);
bf=zeros(1,nfilt);

af(1:na)=a(:);
bf(1:nb)=b(:);

for i=nfilt+1:nx+nfilt
    for j=1:nfilt
        if j<na&&j<nb
            yf(i)=yf(i)+bf(j)*xf(i-j+1)-af(j)*yf(i-j+1);
        elseif j>na
            yf(i)=yf(i)+bf(j)*xf(i-j+1);
        else
            yf(i)=yf(i)-af(j)*yf(i-j+1);
        end
    end
end

y=yf(nfilt+1:nx+nfilt);

*/