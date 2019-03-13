
%%%%%%%%%%%%%%%%%%%%%%%%%%%% 本算法出自文献：基于MATLAB的FIR带阻数字滤波器的设计_邢国泉.pdf %%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear all,close all

k=4;
fsamp = 300;
T=1/fsamp;      %信号的采样周期(s)
fg=50;
FT=1/fg;     %工频干扰的周期(s),FT=k*T
t=linspace(0,1,10000);


fcuts = [48 49 51 52];
mags = [0 1 0];
devs = [0.01 0.01 0.01];
[n,Wn,beta,ftype] = kaiserord(fcuts,mags,devs,fsamp);
n = n + rem(n,2);
hh = fir1(n,Wn,'stop',kaiser(n+1,beta));

hh(find(hh<0.001))=0;
%t=[0:fg]/fg;
s1=sin(pi*fg*t);
s2=zeros(1,10000);
s2(1:50:10000)=0.2;%sin(2*pi*fsamp*t);

s3=s2+s1;
s4=filter(hh,1,s3);


X=sin([0:0.0001:1]*200*2*pi);      %一维数据向量
subplot(3,2,1),plot([0:0.0001:1]*T,X,'b'),title('Original')%,axis([0 10000*T -1.5 1.5])

%interf=zeros(1,10000);
interf=0.2*sin([0:0.0001:1]*50*2*pi);        %周期脉冲噪声信号，幅度0.2
subplot(3,2,2),plot([0:0.0001:1]*T,interf,'b'),title('Noise')%,axis([0 10000*T -1.5 1.5])

Xdata=X+interf;               %采样得到需要处理的数据(混入噪声),一维向量
subplot(3,2,3),plot([0:0.0001:1]*T,Xdata,'b'),title('Add Noise')%,axis([0 10000*T -1.5 1.5])

data=filter(hh,1,Xdata);

[H,f] = freqz(hh,1,fsamp,'whole');
Hf=abs(H);
nans=isnan(Hf);
Hf(nans)=0;

Hx=angle(H);
nans=isnan(Hx);
Hx(nans)=0;

subplot(3,2,4),plot([0:0.0001:1]*T,data,'b'),title('Process')%,axis([0 10000*T -1.5 1.5])

noise=data-X;
subplot(3,2,5),plot([0:0.0001:1]*T,noise,'b'),title('Get Noise')%,axis([0 10000*T -1.5 1.5])
