#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "GUI.h"
#include "ECG_Process.h"
#include "string.h"
#include "HRAlarmSet.h"
#include <stdio.h>
#include "DriversHeader.h"

void RCC_Configuration(void);
void NVIC_Configuration(void);
void TIM_Configuration(void);

int main(void)
{
	ECGDataTypeDef ECGData;
	uint16_t HR;

	RCC_Configuration();
	GPIO_Configuration();
	KEY_Configuration();
	NVIC_Configuration();
	TIM_Configuration();
    LCD_Initializtion();
    LCD_Clear(Black);
	
	ECG_Disp_Init();
	HRAlarm_Init();
	ADS1292_Init();


	TIM_Cmd(TIM3, ENABLE);							// 开启时钟 用于报警LED闪烁

	while(1)
	{

		if(ADS1292Data.updata == 1)//一次ADS1292数据采集完成，进行数据滤波
		{
			ECGDataFilter(&ADS1292Data,&ECGData);//数据滤波
			ADS1292Data.updata =  0;
		}


                
		if(HRBufferFullflag == 1) //心率计算buffer满，进行一次心率计算
		{
			HR = ECGHRProcess();		  //进行心率计算
			HRBufferFullflag = 0;//完成心率计算，将标志位清0
		}

		AlarmLEDStatus = HRAlarmProcess(HR);//心率报警处理

                
                
		if(ECGData.updatadisp == 1)//当UpdataDisp == 1 时刷新显示
		{
			ECG_Disp(ECGData);		   //显示心电波
			HR_Disp(HR);//显示心率
			ECGData.updatadisp = 0;	   //显示完成
		}
	}
}

void RCC_Configuration(void)
{
	ErrorStatus HSEStartUpStatus;
  /* RCC system reset(for debug purpose) */
	RCC_DeInit();

  /* Enable HSE */
	RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
	HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    /* HCLK = SYSCLK = 72MHz */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    /* PCLK2 = HCLK = 72MHz */
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    /* PCLK1 = HCLK/2 = 36MHz */
    RCC_PCLK1Config(RCC_HCLK_Div2);

    /* ADCCLK = PCLK2/6 = 12MHz */
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
  
    /* PLLCLK = 12MHz * 6 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_6);

    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
}



/*******************************************************************************/
void TIM_Configuration(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);				//用于报警LED闪烁
  TIM_DeInit(TIM3);
  TIM_TimeBaseStructure.TIM_Period=20-1;		 					/* 自动重装载寄存器周期的值(计数值) 20k=0.05ms,0.05ms*20=1ms=1KHz*/
																	/* 累计 TIM_Period个频率后产生一个更新或者中断 */
  TIM_TimeBaseStructure.TIM_Prescaler= (3600 - 1);				    /* 时钟预分频数   例如：时钟频率=72MHZ/(时钟预分频+1)=72M/3600=20K */
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 			/* 采样分频 */
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 		/* 向上计数模式 */
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  TIM_ClearFlag(TIM3, TIM_FLAG_Update);							    /* 清除溢出中断标志 */
  TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
  TIM_Cmd(TIM3, DISABLE);											/* 暂时关闭 */
}
/*******************************************************************************/
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
		 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置中断组为2bit抢占优先级 2bit副优先级  													

 	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;		  // 配置中断线9-5
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 设置占先优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  // 设置副优先级为1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // 使能中断线9-5
	NVIC_Init(&NVIC_InitStructure);	

	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel4_5_IRQn;//DMA2_Channel5_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;	  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


 	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;		  // 配置中断线9-5
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // 设置占先优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		  // 设置副优先级为1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // 使能中断线9-5
	NVIC_Init(&NVIC_InitStructure);	

 	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;		// 配置中断线15-10
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // 设置占先优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		  // 设置副优先级为2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // 使能中断线15-10
	NVIC_Init(&NVIC_InitStructure);	

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;				//最低优先级 用于报警LED闪烁	  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
}

