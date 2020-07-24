	#include "stm32f4xx.h"                  // Device header
	#include "read_values.h"
	#include <stdio.h>

//#define DEBUG


	/**
		* @brief  Configures ADC1, ADC2
		* @param  None
		* @note   2 Analog inputs connect to GPIO_PIN_ 1 and 2 
		*				 PC1 (GPIO_Pin_1) => RV2, 
		*				 PC2 (GPIO_Pin_2) => RV3
		* @retval None
		*/
	void Configure_ADC()
	{
		GPIO_InitTypeDef GPIO_Pot_Init;
		ADC_InitTypeDef ADC_Pot_Init;
		
		//enable clock for ADC1, ADC2 , port C
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		
		//Initialize GPIO ports where Potentiometers are connected 
		GPIO_Pot_Init.GPIO_Pin= GPIO_Pin_1 | GPIO_Pin_2;
		GPIO_Pot_Init.GPIO_Mode= GPIO_Mode_AN;
		GPIO_Pot_Init.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOC,&GPIO_Pot_Init);
		
		//Initialize ADC for 2 GPIOs	
		ADC_Pot_Init.ADC_Resolution = ADC_Resolution_12b;
		ADC_Pot_Init.ADC_ScanConvMode = DISABLE;
		ADC_Pot_Init.ADC_ContinuousConvMode = DISABLE;
		ADC_Pot_Init.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
		ADC_Pot_Init.ADC_ExternalTrigConv = DISABLE;
		ADC_Pot_Init.ADC_DataAlign=ADC_DataAlign_Right;
		ADC_Pot_Init.ADC_NbrOfConversion=1;
		
		ADC_Init(ADC1, &ADC_Pot_Init);
		ADC_Init(ADC2, &ADC_Pot_Init);
		
		//Configure channel to match GPIO pin
		ADC_RegularChannelConfig(ADC2, ADC_Channel_11, 1, ADC_SampleTime_84Cycles);
		ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 1, ADC_SampleTime_84Cycles);
		
		//Enable ADC1 & ADC2
		ADC_Cmd(ADC1,ENABLE);
		ADC_Cmd(ADC2,ENABLE);
		
			
	}
	
	 /**
		* @brief  Reads Value from RV2 (GPIO_Pin_1)
		* @param  None
		* @note   Starts conversion and waits for conversion to end
		* @retval Converted value integer
		*/
	uint16_t ADC_Value_Threshold(void)
	{	
		ADC_SoftwareStartConv(ADC1);
	
    #ifdef DEBUG
		printf("EOC ADC1 FLAG:: %d ",ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    #endif //debug
		
		while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
		return ADC_GetConversionValue(ADC1);	
	}
	
	/**
		* @brief  Reads Value from RV3 (GPIO_Pin_2)
		* @param  None
		* @note   Starts conversion and waits for conversion to end
		* @retval Converted value integer
		*/
	uint16_t ADC_Value_Current(void)
	{
		ADC_SoftwareStartConv(ADC2);
		
		#ifdef DEBUG
		printf("EOC ADC2 FLAG:: %d ",ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC));
    #endif //debug
		
		while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC));
		return ADC_GetConversionValue(ADC2);	
	}
	
	/**
		* @brief  Scales any interger value to any float value
		* @note   None
		* @param  @arg X input data
		*					@arg input_min - minimum boundary of input data
		*					@arg input_max - maximum boundary of input data
		*					@arg out_min   - new minimum range of data (float)
		*					@arg out_max   - new maximum range of data (float)
		* 
		* @retval float scaled number 
		*/	
	float scale_number(uint16_t x, uint16_t input_min, uint16_t input_max,
											float out_min, float out_max) {
												
		return (float)(x-input_min)*(out_max-out_min)/(input_max-input_min)+out_min;
	}

