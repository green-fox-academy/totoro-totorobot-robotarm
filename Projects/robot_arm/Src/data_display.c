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
	BSP_LCD_SetFont(&Font16);
	BSP_LCD_DisplayStringAtLine(1, (uint8_t*) "       *** TotoRobot ***");
	BSP_LCD_DisplayStringAtLine(2, (uint8_t*) "       runtime parameters");
	BSP_LCD_DisplayStringAtLine(4, (uint8_t*) "           X    Y    Z");
	BSP_LCD_DisplayStringAtLine(8, (uint8_t*) "         Serv0  Serv1  Serv2");

	draw_buttons();

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
		angles_t ang_servos_rad;
		uint32_t adc[SERVOS];
		coord_cart_t arm_position;
		char target_text[100];

		// Get PWM values
		osMutexWait(servo_pulse_mutex, osWaitForever);
		for (int i = 0; i < SERVOS; i++) {
				pulse[i] = servo_pulse[i];
		}
		osMutexRelease(servo_pulse_mutex);

		// Calculate XYZ
		pulse_to_xyz(&arm_position);

		// Calculate servo absolute angles
		pulse_to_ang_abs(&ang_servos_rad);

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

		// Print target info
		sprintf(lcd_data_buff, "  target  %s", target_text);
		BSP_LCD_DisplayStringAtLine(5, (uint8_t*) lcd_data_buff);

		// Print actual XYZ coordinates
		BSP_LCD_SetTextColor(LCD_COLOR_RED);
		sprintf(lcd_data_buff, "  actual  %3d  %3d  %3d  ", (int16_t) arm_position.x, (int16_t) arm_position.y, (int16_t) arm_position.z);
		BSP_LCD_DisplayStringAtLine(6, (uint8_t*) lcd_data_buff);
		BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

		// Print absolute servo angles
		BSP_LCD_SetTextColor(LCD_COLOR_RED);
		sprintf(lcd_data_buff, "  ang A:  %4d   %4d   %4d", rad_to_deg(ang_servos_rad.theta0),
				rad_to_deg(ang_servos_rad.theta1), rad_to_deg(ang_servos_rad.theta2));
		BSP_LCD_DisplayStringAtLine(9, (uint8_t*) lcd_data_buff);
		BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

		// Print pulse values
		sprintf(lcd_data_buff, "  pulse:  %4lu   %4lu   %4lu", pulse[0], pulse[1], pulse[2]);
		BSP_LCD_DisplayStringAtLine(10, (uint8_t*) lcd_data_buff);

		// Print ADC values
		sprintf(lcd_data_buff, "  ADC:    %4lu   %4lu   %4lu /%d", adc[0], adc[1], adc[2], MAX_ADC_P);
		BSP_LCD_DisplayStringAtLine(11, (uint8_t*) lcd_data_buff);

		osDelay(500);
	}

	while (1) {
		log_msg(USER, "LCD data display thread terminated\n");
		osThreadTerminate(NULL);
    }
}

void draw_buttons(void)
{
	uint8_t i = 0;
	uint8_t j = 0;

	// Stop/Reset position button
	BSP_LCD_SetTextColor(LCD_COLOR_RED);
	BSP_LCD_FillRect(BUTTON_X_START_VERT - j * (BUTTON_WIDTH + BUTTON_DIST_X), BUTTON_Y_START_VERT + i * (BUTTON_HEIGHT + BUTTON_DIST_Y), BUTTON_WIDTH, BUTTON_HEIGHT);
	i++;

	// G-code button
	BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
	BSP_LCD_FillRect(BUTTON_X_START_VERT - j * (BUTTON_WIDTH + BUTTON_DIST_X), BUTTON_Y_START_VERT + i * (BUTTON_HEIGHT + BUTTON_DIST_Y), BUTTON_WIDTH, BUTTON_HEIGHT);
	i++;

	// Drawing button
	BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
	BSP_LCD_FillRect(BUTTON_X_START_VERT - j * (BUTTON_WIDTH + BUTTON_DIST_X), BUTTON_Y_START_VERT + i * (BUTTON_HEIGHT + BUTTON_DIST_Y), BUTTON_WIDTH, BUTTON_HEIGHT);
	i++;

	// ADC button
	BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
	BSP_LCD_FillRect(BUTTON_X_START_VERT - j * (BUTTON_WIDTH + BUTTON_DIST_X), BUTTON_Y_START_VERT + i * (BUTTON_HEIGHT + BUTTON_DIST_Y), BUTTON_WIDTH, BUTTON_HEIGHT);
	j++;

	// End position switch C
	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	BSP_LCD_FillRect(BUTTON_X_START_VERT - j * (BUTTON_2_WIDTH + BUTTON_DIST_X), BUTTON_Y_START_VERT + i * (BUTTON_HEIGHT + BUTTON_DIST_Y), BUTTON_2_WIDTH, BUTTON_HEIGHT);
	j++;

	// End position switch B
	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	BSP_LCD_FillRect(BUTTON_X_START_VERT - j * (BUTTON_2_WIDTH + BUTTON_DIST_X), BUTTON_Y_START_VERT + i * (BUTTON_HEIGHT + BUTTON_DIST_Y), BUTTON_2_WIDTH, BUTTON_HEIGHT);
	j++;

	// End position switch A
	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	BSP_LCD_FillRect(BUTTON_X_START_VERT - j * (BUTTON_2_WIDTH + BUTTON_DIST_X), BUTTON_Y_START_VERT + i * (BUTTON_HEIGHT + BUTTON_DIST_Y), BUTTON_2_WIDTH, BUTTON_HEIGHT);
	j++;

	// Empty button
	BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
	BSP_LCD_FillRect(BUTTON_X_START_VERT - j * (BUTTON_2_WIDTH + BUTTON_DIST_X), BUTTON_Y_START_VERT + i * (BUTTON_HEIGHT + BUTTON_DIST_Y), BUTTON_2_WIDTH, BUTTON_HEIGHT);
	j++;

	// Gripper close
	BSP_LCD_SetTextColor(LCD_COLOR_ORANGE);
	BSP_LCD_FillRect(BUTTON_X_START_VERT - j * (BUTTON_2_WIDTH + BUTTON_DIST_X), BUTTON_Y_START_VERT + i * (BUTTON_HEIGHT + BUTTON_DIST_Y), BUTTON_2_WIDTH, BUTTON_HEIGHT);
	j++;

	// Gripper open
	BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
	BSP_LCD_FillRect(BUTTON_X_START_VERT - j * (BUTTON_2_WIDTH + BUTTON_DIST_X), BUTTON_Y_START_VERT + i * (BUTTON_HEIGHT + BUTTON_DIST_Y), BUTTON_2_WIDTH, BUTTON_HEIGHT);


}
