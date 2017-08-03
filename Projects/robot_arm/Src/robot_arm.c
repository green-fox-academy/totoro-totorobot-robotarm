#include "robot_arm.h"

void servo_control_thread(void const * argument)
{
	if (debug) {
		LCD_UsrLog((char*) "Servo control thread started\n");
	}

	// Implement code here

	// Test parallel threads
	BSP_LED_Init(LED_GREEN);
	BSP_LED_Off(LED_GREEN);

	while(1) {
		BSP_LED_Toggle(LED_GREEN);
		osDelay(1000);
	}

	while (1) {
		if (debug) {
			LCD_ErrLog((char*) "Servo control thread terminated\n");
		}
		osThreadTerminate(NULL);
    }
}

