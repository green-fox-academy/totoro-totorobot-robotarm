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
		//servo0 PWM; servo1 PWM; servo2 PWM; servo3 PWM; TIME: ms
		uint32_t demo_pulse[12][5] = {
			 {5360, 4070, 5870, 2000, 1000},
			 {5360, 5920, 5870, 2000, 1000},
			 {5360,	4070, 5870, 2000, 1000},
			 {5360, 6190, 3900, 2000, 1000},
			 {2180,	6190, 3900, 2000, 1000},
			 {7780, 6190, 3900,	2000, 1000},
			 {7780, 4070, 6520, 2000, 1000},
			 {2160,	4070, 6520,	2000, 1000},
			 {2160,	6190, 6520,	2000, 1000},
			 {2160,	6190, 6520, 2000, 1000},
			 {7420,	6190, 6520,	2000, 1000},
			 {5360,	4070, 5870,	2000, 1000}
		};


		for (int i = 0; i < 12; i++) {
				osMutexWait(servo_pulse_mutex, osWaitForever);
				for (int servo = 0; servo < SERVOS; servo++) {
					servo_pulse[i] = demo_pulse[i][servo];
				}
				osMutexRelease(servo_pulse_mutex);
				osDelay(demo_pulse[i][4]);
		}

	}

	while (1) {
		if (debug) {
			LCD_ErrLog((char*) "Demo thread terminated\n");
		}
		osThreadTerminate(NULL);
    }
}
