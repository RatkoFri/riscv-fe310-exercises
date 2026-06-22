// include the header file for the GPIO HAL
#include "hal_gpio.h"


// This function should be reentrat as it is called by several threads
void primitivedelay(unsigned int delay){
	while(delay>0){
		__asm__ volatile ( "nop ;"
				           "nop");
		delay--;
	}
}


int main() {

	// Step 1: Define settings for the GPIO pins using the GPIO_InitTypeDef structure
    //        - Set the mode to GPIO_MODE_OUTPUT to configure the pins as outputs
    //        - Set the pin mask to include the green, blue, and red LED pins (LED_GREEN_PIN, LED_BLUE_PIN, LED_RED_PIN)


	// Step 2: Set up pointer to GPIO device

	// Step 3: Initialize GPIO device with configuration


    // Step 4: Toggle the state of the LEDs in a loop with a delay, pattern should be: R -> G -> B -> R -> G -> B ...
    //        - Use the HAL_GPIO_WritePin function to set the state of each LED pin, 
    //                  for example: HAL_GPIO_WritePin(GPIO, LED_BLUE_PIN, GPIO_PIN_RESET) to turns on the blue LED
    //         - Use the primitivedelay function to create a delay between toggling the LEDs primitivedelay(500000)       
	while(1){
		
	}



	return 0;
}

