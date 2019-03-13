#include <msp430x14x.h>
#include "math.h"
#include "denoising.h"
#include "ADS1292.h"

//低通滤波器
int filterlp(int samplelp){
	static int buflp[32];
	static int offsetlp = 0;
	long z;
	int i;
	buflp[offsetlp] = samplelp;
    z = mul16(coeffslp[8],buflp[(offsetlp-8)&0x1F]);
	for(i = 0;i<8;i++){
		z += mul16(coeffslp[i],buflp[(offsetlp-i)&0x1F]+buflp[(offsetlp-16+i)&0x1F]);
	}
	offsetlp = (offsetlp+1)&0x1F;
	return z>>15;	
}
//高通滤波器
int filterhp(int samplehp){
    static int bufhp[32];
    static int offsethp = 0;
    long z;
    int i;
    bufhp[offsethp] = samplehp;
    z = mul16(coeffshp[8],bufhp[(offsethp-8)&0x1F]);
    for(i = 0;i < 8;i++){
		z += mul16(coeffshp[i],bufhp[(offsethp-i)&0x1F]+bufhp[(offsethp-16+i)&0x1F]);
	}
    offsethp = (offsethp+1)&0x1F;
    return z>>15;
}
//数字滤波器
void denoising(unsigned char *sample){
  int i;
  for(i = 0;i<32;i++){
    sample[i]=filterlp(sample[i]);//低通滤波
       Dataout_pulse[i]=filterhp(sample[i])-128;//高通滤波
       sample[i]=sample[i]>>6;
       if(sample[i]>255)
       sample[i]=255;
       if(sample[i]<0)
       sample[i]=0;
       counter++;
       pulseperiod++;
       if(Dataout_pulse[i]>48)
       {
         counter=0;//采集到有效心电信号
         if(counter==128)
         {
            beats++;
            if(beats==3)
            {
              beats=0;
 //             heartrate=itobcd(92160/pulseperiod);   //将心率16进制数转为bcd码
              pulseperiod = 0;
            }
          }
        }
  }        
}
// 将16进制数转为BCD码
int itobcd(int i) { 
	int bcd = 0;                           
    char j = 0; 
    while (i > 9) 
    {
		bcd |= ((i % 10) << j); 
        i /= 10; 
        j += 4;
	} 
        return (bcd | (i << j)); // Return converted value
}
//16位乘法
long mul16(register int x,register int y){
	long result=0;
	MPY = x;
	OP2 = y;
	result += RESHI;
	result <<= 16;
	result += RESLO;
	return result;
}
    
