
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef READ_VALUES_H
	#define READ_VALUES_H

	void Configure_ADC(void);
	uint16_t ADC_Value_Threshold(void);
	uint16_t ADC_Value_Current(void);
	float scale_number(uint16_t x, uint16_t input_min, uint16_t input_max,
												float out_min, float out_max);

	 
#endif

