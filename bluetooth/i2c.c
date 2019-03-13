#include "i2c.h"
#include "delay.h"

#define  _Nop()  _nop_()  //定义空指令
                         
bit ack;	              //应答标志位

sbit SDA=P2^1;
sbit SCL=P2^0;

/*------------------------------------------------
                    启动总线
------------------------------------------------*/
void Start_I2c()
{
  SDA=1;   //发送起始条件的数据信号
  _Nop();
  SCL=1;
  _Nop();    //起始条件建立时间大于4.7us,延时
  _Nop();
  _Nop();
  _Nop();
  _Nop();    
  SDA=0;     //发送起始信号
  _Nop();    //起始条件锁定时间大于4μ
  _Nop();
  _Nop();
  _Nop();
  _Nop();       
  SCL=0;    //钳住I2C总线，准备发送或接收数据
  _Nop();
  _Nop();
}
/*------------------------------------------------
                    结束总线
------------------------------------------------*/
void Stop_I2c()
{
  SDA=0;    //发送结束条件的数据信号
  _Nop();   //发送结束条件的时钟信号
  SCL=1;    //结束条件建立时间大于4μ
  _Nop();
  _Nop();
  _Nop();
  _Nop();
  _Nop();
  SDA=1;    //发送I2C总线结束信号
  _Nop();
  _Nop();
  _Nop();
  _Nop();
}
/*----------------------------------------------------------------
                 字节数据传送函数               
函数原型: void  SendByte(unsigned char c);
功能:  将数据c发送出去,可以是地址,也可以是数据,发完后等待应答,并对
     此状态位进行操作.(不应答或非应答都使ack=0 假)     
     发送数据正常，ack=1; ack=0表示被控器无应答或损坏。
------------------------------------------------------------------*/
void  SendByte(unsigned char c)
{
 unsigned char BitCnt;
 
 for(BitCnt=0;BitCnt<8;BitCnt++)  //要传送的数据长度为8位
    {
     if((c<<BitCnt)&0x80)SDA=1;   //判断发送位
       else  SDA=0;                
     _Nop();
     SCL=1;               //置时钟线为高，通知被控器开始接收数据位
      _Nop(); 
      _Nop();             //保证时钟高电平周期大于4μ
      _Nop();
      _Nop();
      _Nop();         
     SCL=0; 
    }
    
    _Nop();
    _Nop();
    SDA=1;               //8位发送完后释放数据线，准备接收应答位
    _Nop();
    _Nop();   
    SCL=1;
    _Nop();
    _Nop();
    _Nop();
    if(SDA==1)ack=0;     
       else ack=1;        //判断是否接收到应答信号
    SCL=0;
    _Nop();
    _Nop();
}

