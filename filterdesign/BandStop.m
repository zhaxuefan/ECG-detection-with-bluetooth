
%%%%%%%%%%%%%%%%%%%%%%%% 该算法出自文献：用于去除心电信号中工频干扰数字滤波技术.pdf(王镇) %%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%% FIR filter %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%% Band-Pass Transfer function: H1(z)=1/Q^n*(1-z^(-R))^n/(1-z^(-P))^n,
%%%%%%% Band-Pass Frequence Respose: H1(w)=1/Q^n*sin(R/2*w)^n/sin(P/2*w)^n*exp(-j*(R-P)/2*n*w)
%%%%%%% Band-Stop Transfer function: H(z)=z^(n*(R-P)/2)-1/Q^n*(1-z^(-R))^n/(1-z^(-P))^n,
%%%%%%% Q=R/P
%%%%%%% If SampleRate fs=500Hz and Interference Frequency fg=50Hz,then P=fs/fg=10,
%%%%%%% For a good band-stop and analysis from our tests, we set n=2 and R=160,then Q=16
%%%%%%% so H(z)=z^(-150)-(1/16)^2*((1-z^(-160))/(1-z(-10)))^2
%%%%%%% The next step is designing a self-adaptive band-stop filter ???

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear all

%%%%%%%%%%%%%%%%%%%%%%%%%% Variable List %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%% fs必须是fg的正整数倍
fs=400;         %sample rate              (can change)
fg=50;          %interference frequency   (can change)
n=2;            %const                    (don't change)
R=240;          %what meaning ?           (can change)
P=fs/fg;        %what meaning ?  (const)  (decided by fs and fg)
Q=R/P;          %what meaning ?  (const)  (decided by R and P)


%%%%%%%%%%%%% Band-Pass %%%%%%%%%%%%%
%{    
b=zeros(1,321);
a=zeros(1,21);
b(1)=1/256;
b(161)=-2/256;
b(321)=1/256;

a(1)=1;
a(11)=-2;
a(21)=1;
%}

%%%%%%%%%%%%% Band-Stop %%%%%%%%%%%%%
b=zeros(1,2*R+1);
a=zeros(1,2*P+1);
%b(1)=-1/Q^2;
%b(R-P+1)=1;
%b(R+1)=-2*(Q^2-1)/Q^2;
%b(R+P+1)=1;
%b(2*R+1)=-1/Q^2;

b(1)=-1/Q^2;
b(R-P+1)=1;
b(R+1)=-2*(Q^2-1)/Q^2;
b(R+P+1)=1;
b(2*R+1)=-1/Q^2;

%b(1)=-1;
%b(R-P+1)=1;

%b(R+P+1)=1;
%b(2*R+1)=-1;

a(1)=1;
a(P+1)=-2;
a(2*P+1)=1;
%{
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
%}

%[H,F]=freqz(b,a,fs,'whole');         %幅频响应
[H,F]=freqz(b,a);         %幅频响应
hh=abs(H);
%subplot(3,2,1),plot(F*fs/(2*pi),hh),title('Magnitude-Frequency Characteristics'),xlabel('Frequency(Hz)'),ylabel('Magnitude')
figure(1),plot(F*fs/(2*pi),20*log(hh)),title('Magnitude-Frequency Characteristics'),xlabel('Frequency(Hz)'),ylabel('Magnitude/dB')
Hx=angle(H);
figure(6),plot(F*fs/(2*pi),hh),title('Magnitude-Frequency Characteristics'),xlabel('Frequency(Hz)'),ylabel('Magnitude')
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
t=[1:1000]/fs;
%data=2*sin(2*pi*10*t);
data=int32(51*(sin(2*pi*t)+2));

%load ecgdata.mat               %加载数据
%data=mydata';                  %采样的数据
%t = (1:numel(data))/fs;
%subplot(3,2,2),plot(t,data),title(strcat('Original Sequence(Sample rate:',num2str(fs),'Hz)')),xlabel('Time(s)'),ylabel('Amplitude'),axis([0 numel(data)/fs -2 2])       % 原始数据
figure(2),plot(t,data),title(strcat('Original Sequence(Sample rate:',num2str(fs),'Hz)')),xlabel('Time(s)'),ylabel('Amplitude')%,axis([0 numel(data)/fs min(data) max(data)])       % 原始数据

%noise=0.5*sin(2*pi*50*t);      %噪声数据
noise=int32(10.2*sin(2*pi*50*t));      %噪声数据

%subplot(3,2,3),plot(t,noise),title(strcat('Interfrence of Noise Sequence(Frequency:',num2str(fg),'Hz)')),xlabel('Time(s)'),ylabel('Amplitude'),axis([0 numel(data)/fs -2 2])
figure(3),plot(t,noise),title(strcat('Interfrence of Noise Sequence(Frequency:',num2str(fg),'Hz)')),xlabel('Time(s)'),ylabel('Amplitude')%,axis([0 numel(data)/fs min(noise) max(noise)])
noise_data=data+noise;         %添加噪声
%subplot(3,2,4),plot(t,noise_data),title('With noise Sequence'),xlabel('Time(s)'),ylabel('Amplitude'),axis([0 numel(data)/fs -2 2])
figure(4),plot(t,noise_data),title('With noise Sequence'),xlabel('Time(s)'),ylabel('Amplitude')%,axis([0 numel(data)/fs min(noise_data) max(noise_data)])
%xdata=filter(b,a,noise_data);    %滤波后数据
%subplot(3,2,6),plot(t,ecg),title('After Filter Sequence'),xlabel('Time(s)'),ylabel('Amplitude'),axis([0 numel(data)/fs -2 2])
%figure(5),plot(t,xdata),title('After Filter Sequence'),xlabel('Time(s)'),ylabel('Amplitude'),axis([0 numel(data)/fs -2 2])
%drift=ecg((R-P+1):end)-data(1:end-(R-P));
%subplot(3,2,5),plot(t((R-P+1):end),drift),title(strcat('Difference of before and after filter(delay:',num2str((R-P)/fs),'s)')),xlabel('Time(s)'),ylabel('Amplitude'),axis([0 numel(data)/fs -2 2])

%data=int32(51*(sin(2*pi*t)+2));
%noise=int32(10.2*sin(2*pi*50*t));      %噪声数据
%noise_data=data+noise;         %添加噪声

b_index=[1,R-P+1,R+1,R+P+1,2*R+1];     %index must from small to large
b=[1,Q*Q,2*(Q*Q-1),Q*Q,1];     %Let Q=16, Q^2=2^8, after process signal>>8


a_index=[1,P+1,2*P+1];                 %index must from small to large
a=[Q*Q,2*Q*Q,Q*Q];               %after process signal>>8


xdata=myfilter(b_index,a_index,b,a,noise_data);
figure(5),plot(t,xdata),title('After Filter Sequence'),xlabel('Time(s)'),ylabel('Amplitude')%,axis([0 numel(data)/fs min(xdata) max(xdata)])