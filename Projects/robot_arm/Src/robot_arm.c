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

void start_demo(void) {

	demo_on = 1;

    osThreadDef(DEMO, demo_thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 5);
    osThreadCreate (osThread(DEMO), NULL);

    return;
}

void stop_demo(void) {

	demo_on = 0;

	return;
}

void demo_thread(void const * argument) {

	if (debug) {
		LCD_UsrLog((char*) "Demo thread started\n");
	}

	while (demo_on) {

		// Implement demo here

	}


	while (1) {
		if (debug) {
			LCD_ErrLog((char*) "Demo thread terminated\n");
		}
		osThreadTerminate(NULL);
    }
}
