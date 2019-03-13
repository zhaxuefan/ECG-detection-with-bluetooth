close all,clear all

%%%%%%%%%%% 该算法出自文�?--自�?应相干模板法在心电图机中应用的一种改进算�?pdf %%%%%%%%%%%

k=8;        %设定值，根据信号的采样频率进行设定，k为正整数

fs=50*k;     %信号的采样频�?Hz)
T=1/fs;      %信号的采样周�?s)

fg=50;       %工频干扰的频�?Hz)
FT=1/50;     %工频干扰的周�?s),FT=k*T

X=sin([1:5000]*T*2*pi);      %�?��数据向量

subplot(4,2,1),plot([1:5000]*T,X,'b'),title(strcat('Original Sequence(Sample rate:',num2str(fs),'Hz)')),xlabel('Time(s)'),ylabel('Amplitude'),axis([0 5000*T -2 2])

interf=zeros(1,5000);
interf(1:k:5000)=0.2;        %周期脉冲噪声信号，幅�?.2
subplot(4,2,2),plot([1:5000]*T,interf,'b'),title(strcat('Interfrence of Noise Sequence(Frequency:',num2str(fg),'Hz)')),xlabel('Time(s)'),ylabel('Amplitude'),axis([0 5000*T -2 2])

Xdata=X+interf;               %采样得到�?��处理的数�?混入噪声),�?��向量

subplot(4,2,3),plot([1:5000]*T,Xdata,'b'),title('With noise Sequence'),xlabel('Time(s)'),ylabel('Amplitude'),axis([0 5000*T -2 2])

n=numel(Xdata);     %数据的个�?
t=n*T;              %总采样时间（间隔T采样�?��数据�?
K=floor(t/FT);      %下取整，包含K个干扰周期的信号，K=floor(n/k)
Xdata=Xdata(1:K*FT/T);    %修剪掉不够一个FT周期的多余的那部�?
n=numel(Xdata);

M=128;       %模板长度，根据文献可以设定�?M越大，阻带越窄，效果越好，但工频干扰的频率也有一定范围，根据文献M设为256较好
if M>K
	M=K;   %模板长度不能超过干扰的周期个�?
end

Tn=zeros(1,n);        %模板信号,T(n)=N(n)

temp=0.0;

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
		temp=temp+Xdata(i+k*l);
	end
	Tn(i)=temp/M;
end

subplot(4,2,4),plot([1:5000]*T,Tn,'b'),title('Noise from Sequence'),xlabel('Time(s)'),ylabel('Amplitude'),axis([0 5000*T -2 2])

Sdata=Xdata-Tn;        %�?��的信�?
subplot(4,2,5),plot([1:5000]*T,Sdata,'b'),title('After Filter Sequence'),xlabel('Time(s)'),ylabel('Amplitude'),axis([0 5000*T -2 2])

b=zeros(1,k*(M+1)+1);
a=zeros(1,k+1);

b(1)=1;
b(k+1)=-1-1/M;
b(k*(M+1)+1)=1/M;

a(1)=1;
a(k+1)=-1;
[H,F]=freqz(b,a,10240,'whole',fs);
figure(4),plot(F,abs(H));

%Hf=abs(H);
%nans=isnan(Hf);
%Hf(nans)=0;

%Hx=angle(H);
%nans=isnan(Hx);
%Hx(nans)=0;

%subplot(4,2,6),plot([1:fs],Hf),title('Magnitude-Frequency Characteristics'),xlabel('Frequency(Hz)'),ylabel('Magnitude'),axis([0,fs,-1.5 1.5])
%figure(2),plot([1:fs],Hf),title('Magnitude-Frequency Characteristics'),xlabel('Frequency(Hz)'),ylabel('Magnitude'),axis([0,100,0 1.5])
%subplot(4,2,7),plot([1:fs],Hx),title('Phase-Frequency Characteristic'),xlabel('Frequency(Hz)'),ylabel('Phase'),axis([0,fs,1.5*min(Hx) 1.5*max(Hx)])
%figure(3),plot([1:fs],Hx),title('Phase-Frequency Characteristic'),xlabel('Frequency(Hz)'),ylabel('Phase'),axis([0,fs,1.5*min(Hx) 1.5*max(Hx)])