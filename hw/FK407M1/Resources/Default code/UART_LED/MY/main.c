/***
	***************************************************************************
	*	@file  	main.c
	*	@version V1.0	
	*  @date    2018.5.2		
	*	@author  ���ͿƼ�	
	*	@brief   ���ڷ����ַ�+LED��˸
   ***************************************************************************
   *  @description
	*
	*	ʵ��ƽ̨������STM32F407VET6���İ�
	*	�Ա���ַ��https://shop212360197.taobao.com
	*	QQ����Ⱥ��536665479
	*
	*	����˵����
	*
	*  1.��ʼ��USART1������������Ϊ115200��ʹ��printf������������
	*	2.LED��˸
	* 	
	***************************************************************************
***/

#include "stm32f4xx.h"
#include "led.h"
#include "key.h"  
#include "delay.h"
#include "usart.h"

int main(void)
{
	Delay_Init();
   Usart_Config();
	LED_Init();

	while(1)
	{		
		LED1_ON;		
		Delay_ms(250);	
		LED1_OFF;
		Delay_ms(250);	
		printf("STM32F407VET6���İ崮�ڲ���\r\n");
		printf("���ͿƼ�\r\n");		
	}
}





