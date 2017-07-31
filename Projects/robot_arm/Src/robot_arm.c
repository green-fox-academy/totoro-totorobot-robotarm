#include "robot_arm.h"

void servo_control_thread(void const * argument)
{
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

