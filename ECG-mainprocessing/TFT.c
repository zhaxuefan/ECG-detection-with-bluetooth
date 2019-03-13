
#include  <msp430x14x.h>
#include  "TFT.h"   



//================================================================================================
//	实现功能：	延时
//	输入参数：	count 设置延时时间
//================================================================================================
void delayms(unsigned int count)
{
    int i,j;                                                                                
    for(i=0;i<count;i++)                                                                    
       {
	     for(j=0;j<120;j++);
       }                                                                                     
}

//================================================================================================
//	实现功能：  写命令
//	输入参数：  DH 需要输入16bits命令的高8位
//                  DL 需要输入16bits命令的低8位
//================================================================================================
void Write_Cmd(unsigned char DH,unsigned char DL)
{
	
    CS_0;
    RS_0;
    nRD_1;
    RW_0;

    DataPort_L=DL;            //送低8位命令给TFT
     
	DataPort_H=DH;        //送高8位命令给TFT

        
	RW_1;
	CS_1;
}

//================================================================================================
//	实现功能：  写数据(2*8bits)
//	输入参数：  DH 需要输入16bits数据的高8位
//                  DL 需要输入16bits数据的低8位
//================================================================================================
void Write_Data(unsigned char DH,unsigned char DL)
{
	
	CS_0;
	RS_1;
        RW_0;

	DataPort_L=DL;            //送低8位数据给573待锁存
               
	DataPort_H=DH;            //送高8位数据给TFT

	
	RW_1;
	CS_1;
}

//================================================================================================
//	实现功能：  写数据(16位)
//	输入参数：  y 需要输入16bits数据
//================================================================================================
void  Write_Data_U16(unsigned int y)
{
	unsigned char m,n;
	m=(y>>8);
	n=y;
	Write_Data(m,n);
}

//================================================================================================
//	实现功能：  向x寄存器写入y数据
//	输入参数：  x 需要输入的命令 16位
//                  y 需要输入的数据 16位
//================================================================================================

void  Write_Cmd_Data (unsigned char x,unsigned int y)
{
	unsigned char m,n;
	m=y>>8;
	n=y;
            
	Write_Cmd(0x00,x);
        
	Write_Data(m,n);
}



/*
//================================================================================================
//	实现功能： 设置坐标
//     输入参数：  x0，y0 起始坐标
//                 x1，y1 结束坐标
//================================================================================================
void LCD_SetPos(unsigned int x0,unsigned int x1,unsigned int y0,unsigned int y1)
{

  Write_Cmd_Data(0x50,x0);  // Horizontal GRAM Start Address
  Write_Cmd_Data(0x51,x1);  // Horizontal GRAM End Address
  Write_Cmd_Data(0x52,y0);  // Vertical GRAM Start Address
  Write_Cmd_Data(0x53,y1);  // Vertical GRAM End Address
  
  Write_Cmd_Data(0x20,x0);  // GRAM horizontal Address
  Write_Cmd_Data(0x21,y0);  // GRAM Vertical Address
  
  Write_Cmd (0x00,0x22);    // 0x0022,Start to Write Data to GRAM
  
}
*/


//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标
void LCD_SetPos(u16 Xpos, u16 Ypos)
{
	//竖屏显示					   
	  Write_Cmd_Data(0x20, Xpos);
	  Write_Cmd_Data(0x21, Ypos);
          
	  Write_Cmd (0x00,0x22);    // 0x0022,Start to Write Data to GRAM	
}




/*

//================================================================================================
//	实现功能：	TFT清屏
//	输入参数：	bColor 清屏所使用的背景色
//================================================================================================
void CLR_Screen(unsigned int bColor)
{
 unsigned int i,j;
 LCD_SetPos(0,240,0,320);//320x240

 for (i=0;i<320;i++)
	{
	   for (j=0;j<240;j++)
	       Write_Data_U16(bColor);
	}
}
*/ 

void CLR_Screen(u16 Color)
{
        u16 i=0,j=0; 
	LCD_SetPos(0x0000,0x0000);//320x240 
       
	for(i=0;i<240;i++)
	{
          for(j=0;j<320;j++)
          {
	//	Delay_ms(100);
                Write_Data_U16(Color);
              
          }
                
	}         
} 



//==============================================================================
//             实现功能：	画点
//             输入参数：  x，y 需要画点坐标
//              color 点的颜色
//==============================================================================
void Put_pixel(unsigned int x,unsigned int y,unsigned int color)
{
	LCD_SetPos(x,y);        //设置待画点坐标
	Write_Data_U16(color);      //在指定点写入颜色数据
}


//==============================================================================
//             函数名称: LCD_Scan_Dir                                        
//            功能描述: 设置LCD的自动扫描方向                                
//==============================================================================
void LCD_Scan(void)
{
	u16 regval=0;
//      regval|=U2D_L2R; //从左到右,从上到下
        regval|=D2U_R2L //从下到上,从右到左
	regval|=1<<12;  
	Write_Cmd_Data(0X03,regval);
	
} 




/*
//================================================================================================
//	实现功能：	指定位置显示RGB颜色
//  输入参数：  x0，y0 起始坐标
//              x1，y1 结束坐标
//		        Color  背景颜色
//================================================================================================



void Show_RGB (unsigned int x0,unsigned int x1,unsigned int y0,unsigned int y1,unsigned int Color)

{
	unsigned int i,j;
	LCD_SetPos(x0,x1,y0,y1);      //设置显示位置
	for (i=x0;i<=x1;i++)
	{
	   for (j=y0;j<=y1;j++)
	       Write_Data_U16(Color);
	}
}

*/

void LCD_SetDisplayWindow(u8 Xpos, u16 Ypos, u16 Height, u8 Width)
{
  	Write_Cmd_Data(0x50, Xpos);	   	   	
  	Write_Cmd_Data(0x51, Xpos+Height); 	
  	Write_Cmd_Data(0x52, Ypos);		  	
  	Write_Cmd_Data(0x53, Ypos+Width);  	

  	LCD_SetPos(Xpos, Ypos);			
}


void LCD_WriteBMP(u8 Xpos, u16 Ypos, u16 Height, u8 Width, const u8 *bitmap)
{
  	u32 index;
  	u32 size = Height * Width;
  	u16 *bitmap_ptr = (u16 *)bitmap;

  	LCD_SetDisplayWindow(Xpos, Ypos, Width-1, Height-1);

  	Write_Cmd (0x00,0x22);    // 0x0022,Start to Write Data to GRAM

  	for(index = 0; index < size; index++)
  	{
    	Write_Data_U16(*bitmap_ptr++);
  	}
	
	Write_Cmd_Data(0x50, 0x0000); 
	Write_Cmd_Data(0x51, 0x00EF); 
	Write_Cmd_Data(0x52, 0x0000); 
	Write_Cmd_Data(0x53, 0x013F); 
}



//===========================================================
//	        实现功能： TFT初始化                       //
//===========================================================
void TFT_Initial(void)
{  

    RES_1;    
	delayms(1);                    // Delay 1ms 
	RES_0;  
	delayms(10);                   // Delay 10ms            
	RES_1;  
	delayms(50);                   // Delay 50 ms    
        
//  if(Device_code==0x9325)
  {

        Write_Cmd_Data(0x0000,0x0001);//可以去掉
        Write_Cmd_Data(0x00E5,0x78F0); 
		Write_Cmd_Data(0x0001,0x0100); 
		Write_Cmd_Data(0x0002,0x0700); 
		Write_Cmd_Data(0x0003,0x1030); 
		Write_Cmd_Data(0x0004,0x0000); 
		Write_Cmd_Data(0x0008,0x0202);  
		Write_Cmd_Data(0x0009,0x0000);
		Write_Cmd_Data(0x000A,0x0000); 
		Write_Cmd_Data(0x000C,0x0000); 
		Write_Cmd_Data(0x000D,0x0000);
		Write_Cmd_Data(0x000F,0x0000);
		//power on sequence VGHVGL///
		Write_Cmd_Data(0x0010,0x0000);   
		Write_Cmd_Data(0x0011,0x0007);  
		Write_Cmd_Data(0x0012,0x0000);  
		Write_Cmd_Data(0x0013,0x0000); 
		Write_Cmd_Data(0x0007,0x0000); 
		/////////////vgh///////////// 
		Write_Cmd_Data(0x0010,0x1690);   
		Write_Cmd_Data(0x0011,0x0227);
		//////////vregiout/////////// 
		Write_Cmd_Data(0x0012,0x009D); 
		/////////vom amplitude///////
		Write_Cmd_Data(0x0013,0x1900);
		/////////////vom H///////////
		Write_Cmd_Data(0x0029,0x0025); 
		Write_Cmd_Data(0x002B,0x000D); 
		////////////gamma////////////
		Write_Cmd_Data(0x0030,0x0007);
		Write_Cmd_Data(0x0031,0x0303);
		Write_Cmd_Data(0x0032,0x0003);
		Write_Cmd_Data(0x0035,0x0206);
		Write_Cmd_Data(0x0036,0x0008);
		Write_Cmd_Data(0x0037,0x0406); 
		Write_Cmd_Data(0x0038,0x0304);
		Write_Cmd_Data(0x0039,0x0007); 
		Write_Cmd_Data(0x003C,0x0602);
		Write_Cmd_Data(0x003D,0x0008); 
		////////////ram//////////////
		Write_Cmd_Data(0x0050,0x0000); 
		Write_Cmd_Data(0x0051,0x00EF);
		Write_Cmd_Data(0x0052,0x0000); 
		Write_Cmd_Data(0x0053,0x013F);  
		Write_Cmd_Data(0x0060,0xA700); 
		Write_Cmd_Data(0x0061,0x0001); 
		Write_Cmd_Data(0x006A,0x0000); 
		/////////////////////////////
		Write_Cmd_Data(0x0080,0x0000); 
		Write_Cmd_Data(0x0081,0x0000); 
		Write_Cmd_Data(0x0082,0x0000); 
		Write_Cmd_Data(0x0083,0x0000); 
		Write_Cmd_Data(0x0084,0x0000); 
		Write_Cmd_Data(0x0085,0x0000); 
		/////////////////////////////       
		Write_Cmd_Data(0x0090,0x0010); 
		Write_Cmd_Data(0x0092,0x0600); 
                                
                Write_Cmd_Data(0x0007,0x0133);   
  } 
        

        
      LCD_Scan();
    
      LCD_LED;
}




