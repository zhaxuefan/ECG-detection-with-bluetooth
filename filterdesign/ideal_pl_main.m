
%%%%%%%%%%%%%%%%%%%%%%% ���㷨�������ף�����MATLAB �Ĺ�Ƶ�����ݲ������_������.pdf %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


clear all

h=ideal_pl(pi,801)-ideal_pl(0.4080*pi,801)+ideal_pl(0.3920*pi,801);%+ideal_pl(0.495*pi,801);  

w=kaiser(801,7.856);

notch=h.*rot90(w);                    %�˲���

%sss=notch*200;
%notch(find(notch<0.001))=0;

[H,F]=freqz(notch,1,250,'whole');
Hf=abs(H);
nans=isnan(Hf);
Hf(nans)=0;

Hx=angle(H);
nans=isnan(Hx);
Hx(nans)=0;

subplot(3,2,1),plot([1:250],Hf),title('Ƶ��ͼ')   %Ƶ��ͼ?
subplot(3,2,2),plot(F/pi,Hx), title('��Ƶ����')   %��Ƶ����


load ecgdata1.mat             %��������
data=mydata';
data=-data/100;
t = (1:numel(data))/250;

subplot(3,2,3),plot(t,data),title('ԭʼ����')       % ԭʼ����
%t=[1:numel(data)];

%noise=0.1*sin();
%noise=0.1*sin(2*t);

%Y=fft(data,250);
%pyy=Y.*conj(Y)/250;



drift=0.1*sin(2*pi*50*t);      %����
%noise=0.2*sin(2*pi*t);
%noise(find(noise<0.2))=0;
noise=zeros(1,numel(data));
noise(1:250:numel(data))=0.2;
subplot(3,2,4),plot(t(1:200),drift(1:200)),title('����') 


noise_data=data+drift+noise;                 %�������
subplot(3,2,5),plot(t,noise_data),title('�������') 
%Y=fft(noise_data,250);
%pyy=Y.*conj(Y)/250;

ecg=filter(notch,1,noise_data);        %�˲����
subplot(3,2,6),plot(t,ecg),title('�˲����') 

Y=fft(ecg,250);
pyy=Y.*conj(Y)/250;


