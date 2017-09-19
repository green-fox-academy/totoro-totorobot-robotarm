#include "data_display.h"

void start_lcd_data_display(void)
{
	log_msg(USER, "Stopping LCD log utility\n");
	log_msg(USER, "Starting LCD data display thread\n");

	// Disable logging to LCD screen
	lcd_logger_on = 0;

	// Disable LCD debug utility
	LCD_LOG_DeInit();

	BSP_LCD_Clear(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_SetFont(&Font20);
	BSP_LCD_DisplayStringAtLine(0, (uint8_t*) "   TotoRobot runtime parameters   ");
	BSP_LCD_DisplayStringAtLine(7, (uint8_t*) "       Serv0  Serv1  Serv2  Serv3");

	// Start continuously updating data on display
	lcd_data_display_on = 1;

    osThreadDef(LCD_DATA_DISPLAY, lcd_data_display_thread, osPriorityBelowNormal, 0, configMINIMAL_STACK_SIZE * 5);
    osThreadCreate (osThread(LCD_DATA_DISPLAY), NULL);

	return;
}

void stop_lcd_data_display(void)
{
	// Disable data display
	lcd_data_display_on = 0;
	osDelay(500);

	// Restart LCD logging
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	LCD_LOG_Init();

	// Show Header and Footer texts
	LCD_LOG_SetHeader((uint8_t *)"TotoRobot - robot arm");
	LCD_LOG_SetFooter((uint8_t *)"STM32746G-DISCO - GreenFoxAcademy");

	// Enable debug to LCD screen
	lcd_logger_on = 1;

	log_msg(USER, "LCD logging utility started\n");

	return;
}

void lcd_data_display_thread(void const * argument)
{
	while (lcd_data_display_on) {

		uint32_t pulse[SERVOS];
		angles_t servo_angles;
		uint32_t adc[SERVOS];
		coord_cart_t arm_position;
		coord_polar_t arm_polar;
		char target_text[100];

		// Get PWM values
		osMutexWait(servo_pulse_mutex, osWaitForever);
		for (int i = 0; i < SERVOS; i++) {
				pulse[i] = servo_pulse[i];
		}
		osMutexRelease(servo_pulse_mutex);

		// Calculate relative angles
		pulse_to_ang_rel(&servo_angles);

		// Calculate polar coordinates
		calc_forward_kinematics(&servo_angles, &arm_polar);

		// Calculate XYZ
		ang_rel_to_xyz(&servo_angles, &arm_position);

		// Get ADC data
		osMutexWait(servo_adc_mutex, osWaitForever);
		for (int i = 0; i < SERVOS; i++) {
			adc[i] = adc_values[i];
		}
		osMutexRelease(servo_adc_mutex);

		// Get target display data
		osMutexWait(arm_coord_mutex, osWaitForever);
		strcpy(target_text, target_display);
		osMutexRelease(arm_coord_mutex);

		// Print and log target info
		sprintf(lcd_data_buff, "target  %s", target_text);
		BSP_LCD_DisplayStringAtLine(2, (uint8_t*) lcd_data_buff);
		log_msg(DEBUG, lcd_data_buff);

		// Print and log polar coordinates
		sprintf(lcd_data_buff, "actual  R: %3d  A: %3d  Z: %3d  ", (int16_t) arm_polar.r, (int16_t) rad_to_deg(arm_polar.angle), (int16_t) arm_polar.z);
		BSP_LCD_DisplayStringAtLine(4, (uint8_t*) lcd_data_buff);
		log_msg(DEBUG, lcd_data_buff);

		// Print and log coordinates
		BSP_LCD_SetTextColor(LCD_COLOR_RED);
		sprintf(lcd_data_buff, "actual  X: %3d  Y: %3d  Z: %3d  ", (int16_t) arm_position.x, (int16_t) arm_position.y, (int16_t) arm_position.z);
		BSP_LCD_DisplayStringAtLine(5, (uint8_t*) lcd_data_buff);
		BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
		log_msg(DEBUG, lcd_data_buff);

		// Print and log relative servo angles
		sprintf(lcd_data_buff, "ang R:  %4d   %4d   %4d", rad_to_deg(servo_angles.theta0),
				rad_to_deg(servo_angles.theta1), rad_to_deg(servo_angles.theta2));
		BSP_LCD_DisplayStringAtLine(9, (uint8_t*) lcd_data_buff);
		log_msg(DEBUG, lcd_data_buff);

		// Print and log absolute servo angles
		BSP_LCD_SetTextColor(LCD_COLOR_RED);
		rel_to_abs_angle(&servo_angles);
		sprintf(lcd_data_buff, "ang A:  %4d   %4d   %4d", rad_to_deg(servo_angles.theta0),
				rad_to_deg(servo_angles.theta1), rad_to_deg(servo_angles.theta2));
		BSP_LCD_DisplayStringAtLine(10, (uint8_t*) lcd_data_buff);
		BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
		log_msg(DEBUG, lcd_data_buff);

		// Print and log pulse values
		sprintf(lcd_data_buff, "pulse:  %4lu   %4lu   %4lu   %4lu", pulse[0], pulse[1], pulse[2], pulse[3]);
		BSP_LCD_DisplayStringAtLine(11, (uint8_t*) lcd_data_buff);
		log_msg(DEBUG, lcd_data_buff);

		// Print and log ADC values
		sprintf(lcd_data_buff, "ADC:    %4lu   %4lu   %4lu   %4lu", adc[0], adc[1], adc[2], adc[3]);
		BSP_LCD_DisplayStringAtLine(12, (uint8_t*) lcd_data_buff);
		log_msg(DEBUG, lcd_data_buff);

		osDelay(500);
	}

	while (1) {
		log_msg(USER, "LCD data display thread terminated\n");
		osThreadTerminate(NULL);
    }
}
