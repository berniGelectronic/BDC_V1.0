/*************************************************************************************
*-----------------------------BUS DOOR CONTROLLER-------------------------------------
*
*@Type    Assignment based on STM32F4DISCOVERY board and FreeRTOS
*@Module  Systems Programming for Embedded Devices(ELE00063M) @ University of York
*@Version V1.0
*@Student Y3875795
*@Date    March 2020
**************************************************************************************
*@Brief   This is breadboard prototype of Bus Door Controller with following 
*					features: Setting the threshold current (LIMIT) via trimmer
*										Measuring(simulate) motor current via trimmer
*	    							Safety pressure switch	
*										
*					By default, after powering up doors are CLOSING, hitting closed limit 	
*					switch(SW8) changes the state of doors to CLOSED, toggle Open/Close
*					switch(SW4) will change the state to OPENING, after opened limit 
*				  switch(SW1) is pressed, state changes to OPENED.
*					While Opening state user can triger SW4 to close the doors and vice 
*					versa.
*					During the Closing state of the doors if the pressure switch is activated 
*					interrupts arrives, state goes to ERROR OBSTACLE, second press will
*					simulate REMOVED OBSTACLE and doors will go to OPEN state
*					Current state of the doors, set thershold current and measured motor current
*					are printed on the 16x2 LCD display, additionally, position of the doors
*					and motor direction are represented with LEDs
*
*					For DEBUG purpose uncomment line  #define DEBUG
*
*	---------------------------------------------------------------------------------
*			INPUTS: 		    PORT(PIN)			DESIGNATOR(PCB)      DESCRIPTION
*		2 x Analog  :  PC1 (GPIO_Pin_1) 		RV2  					Motor Current Trimmer
*			    				 PC2 (GPIO_Pin_2)		  RV3	 			  	Threshold (Limit) current Trimmer
*		4 x Digital :  PE8 (GPIO_Pin_8)		  SW1  					OPENED Limit Switch(Interrupt)
*									 PE11(GPIO_Pin_11)    SW4  					Open/Close Switch(Interrupt)
*									 PE13(GPIO_Pin_13)    SW6  					Pressure Switch(Interrupt)
*									 PE15(GPIO_Pin_15)    SW8  					CLOSED Limit Switch(Interrupt)
*----------------------------------------------------------------------------------
*			OUTPUTS: 		   PORT(PIN)		 	DESIGNATOR(PCB)     DESCRIPTION	
*		16x2 LCD		   PD0-7(GP_P_0-7)    	LCD1 					16x2 LCD Display
*		8 x Digital :	 PD8 (GPIO_Pin_8) 	 	L1  					OPENED LIMIT (Doors OPENED)
*									 PD15(GPIO_Pin_15) 		L8  					CLOSED LIMIT (Doors CLOSED)
*									 PD9,10 (GP_P_9,10)	  L2,L3  				OPENING Doors
*									 PD13,14(GP_P_13,14)	L13,L14  		  CLOSING Doors
*									 PD8-11 (GP_P_8-11) 	L1-L4 				ERROR OVERCURRENT while OPENING
*									 PD12-15(GP_P_12-15) 	L12-L15				ERROR OVERCURRENT while CLOSING					 
*-----------------------------------------------------------------------------------													  
*@template As a project template, freeRTOS, lab 1 is used 
*
************************************************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "read_values.h"
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "utils.h"
#include "in_out_periph.h"
#include "delay.h"
#include "serial.h"
#include "lcd.h"
#include "queue.h"
#include "data.h"
#include "semphr.h"

//#define DEBUG

xTaskHandle hReadValues;
xTaskHandle hDisplayPrint;

portTASK_FUNCTION_PROTO(vReadValues, pvParameters);
portTASK_FUNCTION_PROTO(vDisplayPrint, pvParameters);

xSemaphoreHandle printMutex;

static xQueueHandle valuesQueue;
static xQueueHandle statesQueue;
static xQueueHandle buttonsQueue;

//------------------------------------------------------------------------------------
int main(void) {

	// Initialise all of the STM32F4DISCOVERY hardware (including the serial port)
	vUSART2_Init();
	lcd_init();
	Configure_ADC();
	Configure_LEDS();
	Configure_Buttons();
	
	// Initialise the queues for values, states and buttons
	valuesQueue  = xQueueCreate( 2, sizeof(values_t));
	statesQueue  = xQueueCreate( 2, sizeof(states_t));
	buttonsQueue = xQueueCreate( 2, sizeof(buttons_t));
	
  printMutex = xSemaphoreCreateRecursiveMutex();
	
	//Welcome Message
	printf("\r\n****Bus Door Controller V1.0 Test****\r\n");

	//Tasks starts here 
	xTaskCreate(vReadValues, "READ", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, &hReadValues);
	xTaskCreate(vDisplayPrint, "PRINT", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, &hDisplayPrint);

	vTaskStartScheduler(); // This should never return.

	while(1);  
}

//-----------------------------------------------------------------------------------------
/**
	* @brief  This task Reads analog signal from 2 inputs, checks limit switches, changes
	*					states according to the position of door
	*	@param  None			 
	* @note   This task read values from ADC1 and ADC2, sends values to queue then 
	*					gets buttons state and send the right state to queue for LCD
	*		
  * @retval None
	*/ 
portTASK_FUNCTION(vReadValues, pvParameters) {
	BaseType_t xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;
	
	values_t values;
	states_t state;
	uint16_t limitCurrentTemp,motorCurrentTemp;
	buttons_t buttons=CLOSE;
	
	while(1)
	{
		limitCurrentTemp = ADC_Value_Threshold();
		motorCurrentTemp = ADC_Value_Current();
		
		//checks if the value from ADC is valid
		if(limitCurrentTemp <= 4095){	
			values.limitCurrent = scale_number(limitCurrentTemp,0,4095,0,9.9);
		}
		else{
			#ifdef DEBUG
				xSemaphoreTakeRecursive(printMutex, portMAX_DELAY);		
				printf("****ERROR****ADC1");
			  xSemaphoreGiveRecursive(printMutex);
			#endif
			break;
		}
		//checks if the value from ADC is valid
		if(motorCurrentTemp <=4095){		
			values.motorCurrent = scale_number(motorCurrentTemp,0,4095,0,9.9);
		}
		else{
			#ifdef DEBUG
				xSemaphoreTakeRecursive(printMutex, portMAX_DELAY);		
				printf("****ERROR****ADC2");
			  xSemaphoreGiveRecursive(printMutex);
			#endif
			break;
		}
		
	  #ifdef DEBUG
		  xSemaphoreTakeRecursive(printMutex, portMAX_DELAY);		
			printf("Threshold_VAL:: %.1f  \n\r",values.limitCurrent);
			printf("Current_VAL  :: %.1f  \n\r",values.motorCurrent);
		  xSemaphoreGiveRecursive(printMutex);
		#endif
				
		//if values are valid send them to queue
		xQueueSendToBack( valuesQueue, &values, portMAX_DELAY );
		//Read button from Queue
		xQueueReceiveFromISR( buttonsQueue, &buttons, &xHigherPriorityTaskWoken );
		
		switch(buttons){
			case CLOSE:	
					if(values.motorCurrent > values.limitCurrent ){
						state=ERROR_OVERCURRENT_CLOSING;
					}
					else{
						state=CLOSING;
					}
				break;
			case OPEN:
				if(state!=OPENED){
					if(values.motorCurrent > values.limitCurrent ){
						state = ERROR_OVERCURRENT_OPENING;
					}
					else{
						state=OPENING;		
					}		
				}
				break;
			case OBSTACLE:
				if(state  == CLOSING){
					state=ERROR_OBSTACLE;
				}
			case OPENED_LIMIT:
				if(state == OPENING){
					state=OPENED;
				}
				break;
			case CLOSED_LIMIT:
				if(state ==CLOSING){
					state=CLOSED;
				}		
				break;
			default:
				break;
		}
		
		//Send the current state to queue 
		xQueueSendToBack( statesQueue, &state, portMAX_DELAY );		
	
		#ifdef DEBUG
			xSemaphoreTakeRecursive(printMutex, portMAX_DELAY);
			printf("------------BUTTON____STATE:: %d \n\r",buttons);
			printf("------------STATE:: %d \n\r",state);
			xSemaphoreGiveRecursive(printMutex);		
		#endif
		
	}
}

//----------------------------------------------------------------------------------------
/**
	* @brief  This task should run every 15ms and deals only with printing to LCD
	*	@param  None			 
	* @note   This task gets value and state from queues according to that, prints
	*				  the values on the screen and chagnes the state message
	*				  refresh rate of 10ms is enough for human eye to see smoothly operation
	*				  while changing the values
  * @retval None
	*/ 
portTASK_FUNCTION(vDisplayPrint, pvParameters) {
	
	portTickType xLastWakeTime = xTaskGetTickCount();	
	values_t readValues;
	states_t readState, lastState;
	uint8_t  printOnce=0;
	char limitCurrent[4], motorCurrent[4];
	
	while(1) {
	  xQueueReceive( statesQueue, &readState, portMAX_DELAY );
		xQueueReceive( valuesQueue, &readValues, portMAX_DELAY );	
		
		snprintf(limitCurrent,4,"%.1f",readValues.limitCurrent);
		snprintf(motorCurrent,4,"%.1f",readValues.motorCurrent);	
	
		#ifdef DEBUG
			xSemaphoreTakeRecursive(printMutex, portMAX_DELAY);
			printf("READ_STATE:: %d \n\r", readState);
			xSemaphoreGiveRecursive(printMutex);
		#endif
		
		//Static charachters are printed only once on LCD, after startup or reset
		if(printOnce==0){
			lcd_move(0,0);
			lcd_print("I:");
			lcd_move(5,0);
			lcd_print("A");
			lcd_move(8,0);
			lcd_print("Lim:");
			lcd_move(15,0);
			lcd_print("A");
			lcd_move(0,1);
			lcd_print("Doors:");
			printOnce=1;		
		}		
		//Update values of motor current and limit current
		lcd_move(2,0);
		lcd_print(motorCurrent);
	  lcd_move(12,0);
		lcd_print(limitCurrent);
		lcd_move(7,1); 
		
		//Reset LEDs only if there is a change of state
		if(lastState != readState ){
			GPIO_ResetBits(GPIOD, LED_RESET <<8);
		}
		
		switch(readState){
			case CLOSED:
				GPIO_SetBits(GPIOD, LED_CLOSED <<8);					
				lcd_print("CLOSED   ");	
				break;
			case OPENING:
				GPIO_SetBits(GPIOD, LED_OPENING <<8);
				lcd_print("OPENING  ");		
				break;
			case ERROR_OVERCURRENT_OPENING:	
				GPIO_SetBits(GPIOD, (LED_ERROR_OVERCURRENT>>4) <<8); //invert LEDs
				lcd_print("ERROR-OVC");		
	  		break;				
			case OPENED:
				GPIO_SetBits(GPIOD, LED_OPENED <<8);
				lcd_print("OPENED   ");				
				break;
			case CLOSING:
				GPIO_SetBits(GPIOD, LED_CLOSING <<8);
				lcd_print("CLOSING  ");	
				break;
			case ERROR_OVERCURRENT_CLOSING:
				GPIO_SetBits(GPIOD, LED_ERROR_OVERCURRENT <<8);
				lcd_print("ERROR-OVC");		
	  		break;
			case ERROR_OBSTACLE:
				GPIO_SetBits(GPIOD, LED_ERROR_OBSTACLE <<8);
				lcd_print("ERROR-OBS");		
				break;
			default:
				lcd_print("FFFFFFFFF");
				break;
		}
		lastState=readState;
		
		//10 ms is enough for human eye not to recognize any flickering
		vTaskDelayUntil( &xLastWakeTime, ( 10 / portTICK_RATE_MS ) );
	}
}
//------------------------------------------------------------------------------------------
/**
	* @brief  Interrupt handler for OPENED LIMIT
	* @param  None
	* @note   Sends the OPENED LIMIT to buttons queue  
	* @retval None
	*/
void EXTI9_5_IRQHandler(void){
	BaseType_t xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;
	
  static buttons_t buttons;

	if(EXTI_GetITStatus(EXTI_Line8) != 0) {
		buttons=OPENED_LIMIT;
		#ifdef DEBUG
			printf("---------LIMIT OPENED---- SW:: %d \n\r",buttons);
		#endif
		EXTI_ClearITPendingBit(EXTI_Line8);
	}
	xQueueSendToBackFromISR(buttonsQueue, &buttons, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	
}
//-------------------------------------------------------------------------------------
/**
	* @brief  Interrupt handler sends button to queue
	* @param  None
	* @note   Designed to handle differnet order of button presses  
	* 				First press on Open/Close will change state to OPEN any
	*					press on Pressure switch is ignored while in OPEN state
	* 				When state is CLOSE first press on pressure switch will
	*					trigger state OBSTACLE, second press will move state to OPEN
	*         or press on Open/Close while state OBSTACLE will change state
	*					to OPEN
	* @retval None
	*/
void EXTI15_10_IRQHandler(void){	
	BaseType_t xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;	
	
	static buttons_t buttons=CLOSE, lastButton=CLOSE; //static to preserve state for next press
	
	if(EXTI_GetITStatus(EXTI_Line11) != 0) {		
		if(lastButton== OPEN){
			buttons=CLOSE;
		}
		else{
			buttons=OPEN;
		}			
		#ifdef DEBUG
			printf("---------OPEN/CLOSE Doors ===== %d \n\r",buttons);
		#endif
	  EXTI_ClearITPendingBit(EXTI_Line11);
	}
		
	else if(EXTI_GetITStatus(EXTI_Line13) != 0){
		//Pressure switch is recognized only when the doors are closing
		if(lastButton == CLOSE){
			buttons=OBSTACLE;		
		}
		else if(lastButton==OBSTACLE){
			buttons=OPEN;
		}		
		else {
			buttons=lastButton;
		}		
		#ifdef DEBUG
			printf("------------OBSTACLE SW:: %d \n\r",buttons);
		#endif
		EXTI_ClearITPendingBit(EXTI_Line13);
	}
	
	else if(EXTI_GetITStatus(EXTI_Line15) != 0) {
		buttons=CLOSED_LIMIT;
		#ifdef DEBUG
			printf("---------LIMIT CLOSED---- SW:: %d \n\r",buttons);
		#endif
		EXTI_ClearITPendingBit(EXTI_Line15);
	}
	
	#ifdef DEBUG
		printf("---------BUTTONS STATE====IRQ:: %d \n\r",buttons);
		printf("****LAST BUTTONS STATE******IRQ:: %d \n\r",lastButton);
	#endif
	
	lastButton=buttons;
	
	xQueueSendToBackFromISR(buttonsQueue, &buttons, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
