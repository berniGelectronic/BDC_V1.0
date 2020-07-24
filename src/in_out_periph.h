/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef IN_OUT_PERIPH_H
	#define IN_OUT_PERIPH_H

	#define LED_CLOSED 0x80
	#define LED_OPENED 0x01
	#define LED_ERROR_OBSTACLE 0x7E
	#define LED_ERROR_OVERCURRENT 0xF0	// invert for opening state
	#define LED_RESET 0xFF
	#define LED_OPENING 0x06
	#define LED_CLOSING 0x60

	#define LIMIT_CLOSED 0x80
	#define LIMIT_OPENED 0x01
	#define LIMIT_CLOSED_MASK 0x80
	#define LIMIT_OPENED_MASK 0x01

	void Configure_LEDS(void);
	void Configure_Buttons(void);


#endif 
