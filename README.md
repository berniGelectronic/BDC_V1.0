# Bus Door Controller - BDC_V1.0

/*************************************************************************************

*----------------------------BUS DOOR CONTROLLER--------------------------------------

*@Type    Assignment based on STM32F4DISCOVERY board and FreeRTOS

*@Module  Systems Programming for Embedded Devices(ELE00063M) @ University of York

*@Version V1.0

*@Student Bernard

*@Date    March 2020

**************************************************************************************

@Brief   This is breadboard prototype of Bus Door Controller with following 
					features: Setting the threshold current (LIMIT) via trimmer
										Measuring(simulate) motor current via trimmer									
	    							Safety pressure switch	
										
*	By default, after powering up doors are CLOSING, hitting closed limit 	
	switch(SW8) changes the state of doors to CLOSED, toggle Open/Close
	switch(SW4) will change the state to OPENING, after opened limit 
	switch(SW1) is pressed, state changes to OPENED.
  While Opening state user can triger SW4 to close the doors and vice 
	versa.
	During the Closing state of the doors if the pressure switch is activated 
  interrupts arrives, state goes to ERROR OBSTACLE, second press will
	simulate REMOVED OBSTACLE and doors will go to OPEN state
	Current state of the doors, set thershold current and measured motor current
	are printed on the 16x2 LCD display, additionally, position of the doors
	and motor direction are represented with LEDs

*	For DEBUG purpose uncomment line  #define DEBUG
************************************************************************************/
