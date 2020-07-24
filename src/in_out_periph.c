
	#include "stm32f4xx.h"             
	#include "in_out_periph.h"
  #include <stdio.h>
	
	
	/** 
		* @brief  Configure LEDs
		* @param  None
		* @note   8 LEDS connected to Port D
		* 				LED OPENED : GPIO_Pin_8
		* 				LED CLOSED : GPIO_Pin_15
		* 				ERROR OVERCURRENT (CLOSING 4 leds) = GPIO_Pin_12 | 13 | 14 | 15 
		* 				ERROR OVERCURRENT (OPENING 4 leds) = GPIO_Pin_8  |  9 | 10 | 11
		*					ERROR OBSTACLE (6 leds) = GPIO_Pin_9 | 10 | 11 | 12 | 13 | 14
		* @retval None
		*/
	void Configure_LEDS(void){
		//enable clock for port D
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
		GPIO_InitTypeDef GPIO_Struct;
		
		GPIO_Struct.GPIO_Pin= 0xFF00;
		GPIO_Struct.GPIO_Mode=GPIO_Mode_OUT;
		GPIO_Struct.GPIO_OType = GPIO_OType_PP;
		GPIO_Struct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
		GPIO_Struct.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_Init(GPIOD,&GPIO_Struct);
		
	}
	/**
		* @brief  Configures buttons 
		* @note   4x SW connected to Port E
		*				  Opened Doors LIMIT SW1: GPIO_Pin_8
		*				  Closed Doors LIMIT SW8: GPIO_Pin_15
		*				  Open/Close Doors   SW4: GPIO_Pin_11
		*				  Pressure security  SW6: GPIO_Pin_13	
		* @param  None
		* @retval None
		*/
	void Configure_Buttons(void){
		//enable clock for port E and Interrupt
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
		
		GPIO_InitTypeDef GPIO_Struct;
		EXTI_InitTypeDef EXTI_Struct;
		NVIC_InitTypeDef NVIC_Struct;
		
		//Input buttons
		GPIO_Struct.GPIO_Pin=GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_15;
		GPIO_Struct.GPIO_Mode=GPIO_Mode_IN;
		GPIO_Struct.GPIO_PuPd=GPIO_PuPd_NOPULL;
		GPIO_Struct.GPIO_Speed=GPIO_Speed_100MHz;
		GPIO_Init(GPIOE, &GPIO_Struct);
		
		//Select line which will be used as interrupt
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource8);
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource11);
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource13);
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource15);

		//Interrupts 		
		EXTI_Struct.EXTI_Line=EXTI_Line8 | EXTI_Line11 | EXTI_Line13 | EXTI_Line15;
		EXTI_Struct.EXTI_Mode=EXTI_Mode_Interrupt;
		EXTI_Struct.EXTI_Trigger=EXTI_Trigger_Rising;
		EXTI_Struct.EXTI_LineCmd=ENABLE;
		EXTI_Init(&EXTI_Struct);
		
		//Initialize NVIC
		NVIC_Struct.NVIC_IRQChannel= EXTI15_10_IRQn;
		NVIC_Struct.NVIC_IRQChannelPreemptionPriority=10;
		NVIC_Struct.NVIC_IRQChannelSubPriority=10;
		NVIC_Struct.NVIC_IRQChannelCmd=ENABLE;
		NVIC_Init(&NVIC_Struct);
		NVIC_Struct.NVIC_IRQChannel= EXTI9_5_IRQn;
		NVIC_Init(&NVIC_Struct);
		NVIC_SetPriority(EXTI15_10_IRQn,10);
		NVIC_SetPriority(EXTI9_5_IRQn,10);

	}
	
