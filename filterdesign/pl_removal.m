
close all,clear all

%%%%%%%%%%% 该算法出自文�?--自�?应相干模板法在心电图机中应用的一种改进算�?pdf %%%%%%%%%%%

k=8;        %设定值，根据信号的采样频率进行设定，k为正整数

fg=50;       %工频干扰的频�?Hz)
FT=1/fg;     %工频干扰的周�?s),FT=k*T

fs=fg*k;     %信号的采样频�?Hz)
T=1/fs;      %信号的采样周�?s)



%t=[1:10000];
X=sin([1:100000]*T*2*pi);      %�?��数据向量

subplot(4,2,1),plot([1:100000]*T,X,'b'),title(strcat('Original Sequence(Sample rate:',num2str(fs),'Hz)')),xlabel('Time(s)'),ylabel('Amplitude'),axis([0 10000*T -2 2])

%interf=0.2*sin(2*pi*FT*[1:100000]);
interf=zeros(1,100000);
interf(1:k:100000)=0.2;        %周期脉冲噪声信号，幅�?.2
subplot(4,2,2),plot([1:100000]*T,interf,'b'),title(strcat('Interfrence of Noise Sequence(Frequency:',num2str(fg),'Hz)')),xlabel('Time(s)'),ylabel('Amplitude'),axis([0 10000*T -2 2])

Xdata=X+interf;               %采样得到�?��处理的数�?混入噪声),�?��向量

subplot(4,2,3),plot([1:100000]*T,Xdata,'b'),title('With noise Sequence'),xlabel('Time(s)'),ylabel('Amplitude'),axis([0 10000*T -2 2])

n=numel(Xdata);     %数据的个�?
t=n*T;              %总采样时间（间隔T采样�?��数据�?
K=floor(t/FT);      %下取整，包含K个干扰周期的信号，K=floor(n/k)
%Xdata=Xdata(1:K*FT/T);    %修剪掉不够一个FT周期的多余的那部�?
n=numel(Xdata);

M=20;       %模板长度，根据文献可以设定�?M越大，阻带越窄，效果越好，但工频干扰的频率也有一定范围，根据文献M设为256较好
if M>K
	M=K;   %模板长度不能超过干扰的周期个�?
end

Tn=zeros(1,n);        %模板信号,T(n)=N(n)

%{
temp=0.0;
%k=50;
for i=M*k+1:n
	temp=0;
	for l=1:M
		temp=temp+Xdata(i-k*l);
	end
	Tn(i)=temp/M;
end

for i=1:M*k
	temp=0;
	for l=1:M
		temp=temp+Xdata(i+k*(l-1));
	end
	Tn(i)=temp/M;
end
%}
%单片机处�?
%
len=k*(M-1)+1;                    %模板长度
xdata=zeros(1,len);               %单片机数据缓�?
data_addr=1;                      %地址计数指针
temp=0.0;
for i=2:n                         %中断程序

	xdata(data_addr)=Xdata(i);    %读取ADC转换数据存入缓存
	
	temp=0;
	
	%相干模板法处�?
	for j=1:M
		index=data_addr+(j-1)*k;      %xdata(index),index=n+k*i
		%index=index%len;
		if index<=len
			temp=temp+xdata(index);
		else
			temp=temp+xdata(index-len);
		end
	end
	
	Tn(i-1)=temp/M;
	data_addr=data_addr+1;
	%data_addr=data_addr%len;
	if data_addr>len
        data_addr=1;
    end
end
%}


subplot(4,2,4),plot([1:100000]*T,Tn,'b'),title('Noise from Sequence'),xlabel('Time(s)'),ylabel('Amplitude'),axis([0 10000*T -2 2])

Sdata=Xdata-Tn;        %�?��的信�?
subplot(4,2,5),plot([1:100000]*T,Sdata,'b'),title('After Filter Sequence'),xlabel('Time(s)'),ylabel('Amplitude'),axis([0 10000*T -2 2])

b=zeros(1,k*M+1);
a=zeros(1,k+1);

b(1)=(M-1)/M;
b(k*M+1)=1/M;
b(k+1)=-1;

a(1)=1;
a(k+1)=-1;

[H,W]=freqz(b,a,fs,'whole');

Hf=abs(H);
nans=isnan(Hf);
Hf(nans)=0;

Hx=angle(H);
nans=isnan(Hx);
Hx(nans)=0;

subplot(4,2,6),plot([1:fs],Hf),title('Magnitude-Frequency Characteristics'),xlabel('Frequency(Hz)'),ylabel('Magnitude'),axis([0,fs,-1.5 1.5])

subplot(4,2,7),plot([1:fs],Hx),title('Phase-Frequency Characteristic'),xlabel('Frequency(Hz)'),ylabel('Phase'),axis([0,fs,1.5*min(Hx) 1.5*max(Hx)])





