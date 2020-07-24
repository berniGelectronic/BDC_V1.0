# Bus Door Controller - BDC_V1.0

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

************************************************************************************

## Diagram
![Bus_door_module_Project (5)](https://user-images.githubusercontent.com/37474395/88382069-ff9e0500-cda7-11ea-9dc9-31def59d5c9c.png)
***


![Bus_door_module_Project (6)](https://user-images.githubusercontent.com/37474395/88382326-86eb7880-cda8-11ea-91b5-094402ffa446.png)


## Prototype 
![Opening](https://user-images.githubusercontent.com/37474395/88382552-f6616800-cda8-11ea-94b7-07c17ec3ee87.jpg)

![Error_OVC_OPening](https://user-images.githubusercontent.com/37474395/88382528-f19cb400-cda8-11ea-8d9a-ce2891caed3e.jpg)

![Opened](https://user-images.githubusercontent.com/37474395/88382537-f3ff0e00-cda8-11ea-81ce-c37a7c10bd15.jpg)

![closing](https://user-images.githubusercontent.com/37474395/88382816-8d2e2480-cda9-11ea-9d08-da3b7fad5a88.jpg)

![ERROR_OBS](https://user-images.githubusercontent.com/37474395/88382866-a8009900-cda9-11ea-9493-e5ff368887c8.jpg)

![Error_OVC_Closing](https://user-images.githubusercontent.com/37474395/88382818-8ef7e800-cda9-11ea-8314-96e21afe9e85.jpg)

![closed_wh_fingr](https://user-images.githubusercontent.com/37474395/88382813-899a9d80-cda9-11ea-9b91-bdadb390e831.jpg)
