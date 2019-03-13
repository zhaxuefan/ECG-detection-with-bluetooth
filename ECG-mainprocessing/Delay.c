
#include <msp430x14x.h>
/////////////////////////////延时函数///////////////////////////////////////////

/*******************************************
函数名称：Delay_1ms
功    能：延时约1ms的时间
参    数：无
返回值  ：无
********************************************/
void Delay_1ms(void)
{
	unsigned char i;
    
	for(i = 150;i > 0;i--)  _NOP();
} 
/*******************************************
函数名称：Delay_Nms
功    能：延时N个1ms的时间
参    数：n--延时长度
返回值  ：无
********************************************/
void Delay_ms(unsigned int n)
{
    unsigned int i;
    
    for(i = n;i > 0;i--)    Delay_1ms();
}



/*************ms级别延迟函数*****************/
void ADS1292_Delay(unsigned int n)
{
    unsigned int i;
    
    for(i = n;i > 0;i--)    Delay_1ms();
}
/**************us级别延迟函数****************/

void Delay_us(unsigned int n)
{
  unsigned int m;
    for(m=0;m<n;m++);
}

