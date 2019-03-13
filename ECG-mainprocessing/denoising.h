#ifndef DENOISING_H
#define DENOISING_H
/*variables declaration*/
static char beats;
static int Dataout_pulse[32];
static int pulseperiod,counter,heartrate;
//17阶低通滤波器除30Hz以上频率
static const int coeffslp[9]={5225,5175,7255,9453,11593,13507,15016,15983,16315};
//17阶高通滤波器除2Hz以下频率
static const int coeffshp[9]={-763,-1267,-1091,-1867,-1969,-2507,-2619,-2911,29908};
//低通滤波器
int filterlp(int samplelp);
//高通滤波器
int filterhp(int samplehp);
//数字滤波器
void denoising(unsigned char *sample);
// 将16进制数转为BCD码
int itobcd(int i);
//16位乘法器
long mul16(register int x,register int y);
#endif
