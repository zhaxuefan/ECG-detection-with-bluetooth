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


	TIM_Cmd(TIM3, ENABLE);							// ����ʱ�� ���ڱ���LED��˸

	while(1)
	{

		if(ADS1292Data.updata == 1)//һ��ADS1292���ݲɼ���ɣ����������˲�
		{
			ECGDataFilter(&ADS1292Data,&ECGData);//�����˲�
			ADS1292Data.updata =  0;
		}


                
		if(HRBufferFullflag == 1) //���ʼ���buffer��������һ�����ʼ���
		{
			HR = ECGHRProcess();		  //�������ʼ���
			HRBufferFullflag = 0;//������ʼ��㣬����־λ��0
		}

		AlarmLEDStatus = HRAlarmProcess(HR);//���ʱ�������

                
                
		if(ECGData.updatadisp == 1)//��UpdataDisp == 1 ʱˢ����ʾ
		{
			ECG_Disp(ECGData);		   //��ʾ�ĵ粨
			HR_Disp(HR);//��ʾ����
			ECGData.updatadisp = 0;	   //��ʾ���
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

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);				//���ڱ���LED��˸
  TIM_DeInit(TIM3);
  TIM_TimeBaseStructure.TIM_Period=20-1;		 					/* �Զ���װ�ؼĴ������ڵ�ֵ(����ֵ) 20k=0.05ms,0.05ms*20=1ms=1KHz*/
																	/* �ۼ� TIM_Period��Ƶ�ʺ����һ�����»����ж� */
  TIM_TimeBaseStructure.TIM_Prescaler= (3600 - 1);				    /* ʱ��Ԥ��Ƶ��   ���磺ʱ��Ƶ��=72MHZ/(ʱ��Ԥ��Ƶ+1)=72M/3600=20K */
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 			/* ������Ƶ */
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 		/* ���ϼ���ģʽ */
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  TIM_ClearFlag(TIM3, TIM_FLAG_Update);							    /* �������жϱ�־ */
  TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
  TIM_Cmd(TIM3, DISABLE);											/* ��ʱ�ر� */
}
/*******************************************************************************/
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
		 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // �����ж���Ϊ2bit��ռ���ȼ� 2bit�����ȼ�  													

 	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;		  // �����ж���9-5
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // ����ռ�����ȼ�Ϊ2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  // ���ø����ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // ʹ���ж���9-5
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


 	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;		  // �����ж���9-5
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // ����ռ�����ȼ�Ϊ2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		  // ���ø����ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // ʹ���ж���9-5
	NVIC_Init(&NVIC_InitStructure);	

 	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;		// �����ж���15-10
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // ����ռ�����ȼ�Ϊ2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		  // ���ø����ȼ�Ϊ2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // ʹ���ж���15-10
	NVIC_Init(&NVIC_InitStructure);	

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;				//������ȼ� ���ڱ���LED��˸	  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
}

