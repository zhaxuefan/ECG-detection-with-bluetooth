
%%%%%%%%%%%%%%%%%%%%%%% 本算法出自文献：基于MATLAB 的工频干扰陷波器设计_戚仕涛.pdf %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


clear all

h=ideal_pl(pi,801)-ideal_pl(0.4080*pi,801)+ideal_pl(0.3920*pi,801);%+ideal_pl(0.495*pi,801);  

w=kaiser(801,7.856);

notch=h.*rot90(w);                    %滤波器

%sss=notch*200;
%notch(find(notch<0.001))=0;

[H,F]=freqz(notch,1,250,'whole');
Hf=abs(H);
nans=isnan(Hf);
Hf(nans)=0;

Hx=angle(H);
nans=isnan(Hx);
Hx(nans)=0;

subplot(3,2,1),plot([1:250],Hf),title('频谱图')   %频谱图?
subplot(3,2,2),plot(F/pi,Hx), title('相频特性')   %相频特性


load ecgdata1.mat             %加载数据
data=mydata';
data=-data/100;
t = (1:numel(data))/250;

subplot(3,2,3),plot(t,data),title('原始数据')       % 原始数据
%t=[1:numel(data)];

%noise=0.1*sin();
%noise=0.1*sin(2*t);

%Y=fft(data,250);
%pyy=Y.*conj(Y)/250;



drift=0.1*sin(2*pi*50*t);      %噪声
%noise=0.2*sin(2*pi*t);
%noise(find(noise<0.2))=0;
noise=zeros(1,numel(data));
noise(1:250:numel(data))=0.2;
subplot(3,2,4),plot(t(1:200),drift(1:200)),title('噪声') 


noise_data=data+drift+noise;                 %添加噪声
subplot(3,2,5),plot(t,noise_data),title('添加噪声') 
%Y=fft(noise_data,250);
%pyy=Y.*conj(Y)/250;

ecg=filter(notch,1,noise_data);        %滤波结果
subplot(3,2,6),plot(t,ecg),title('滤波结果') 

Y=fft(ecg,250);
pyy=Y.*conj(Y)/250;


