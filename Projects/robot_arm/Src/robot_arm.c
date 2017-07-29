#include "robot_arm.h"

void servo_control_thread(void const * argument)
{
	debug = 1;

	LCD_UsrLog((char*) "Servo control thread started\n");

	pwm_init();
	adc_init();
	position = 0;

	while (1) {
		pwm_set_duty_from_adc();
		osDelay(10);
	}

	while (1) {
		osThreadTerminate(NULL);
    }
}

