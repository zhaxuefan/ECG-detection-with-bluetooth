#ifndef DENOISING_H
#define DENOISING_H
/*variables declaration*/
static char beats;
static int Dataout_pulse[32];
static int pulseperiod,counter,heartrate;
//17�׵�ͨ�˲�����30Hz����Ƶ��
static const int coeffslp[9]={5225,5175,7255,9453,11593,13507,15016,15983,16315};
//17�׸�ͨ�˲�����2Hz����Ƶ��
static const int coeffshp[9]={-763,-1267,-1091,-1867,-1969,-2507,-2619,-2911,29908};
//��ͨ�˲���
int filterlp(int samplelp);
//��ͨ�˲���
int filterhp(int samplehp);
//�����˲���
void denoising(unsigned char *sample);
// ��16������תΪBCD��
int itobcd(int i);
//16λ�˷���
long mul16(register int x,register int y);
#endif
