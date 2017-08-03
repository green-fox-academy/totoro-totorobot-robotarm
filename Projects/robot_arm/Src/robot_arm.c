#include "robot_arm.h"

void servo_control_thread(void const * argument)
{
	BSP_LED_Init(LED_GREEN);
	BSP_LED_Off(LED_GREEN);

		BSP_LED_Toggle(LED_GREEN);
		LCD_UsrLog((char*) "toggle led\n");
		osDelay(1000);
	if (debug) {
		LCD_UsrLog((char*) "Servo control thread started\n");
	}

	// Implement code here

	while (1) {
		if (debug) {
			LCD_ErrLog((char*) "Servo control thread terminated\n");
		}
		osThreadTerminate(NULL);
    }
}

