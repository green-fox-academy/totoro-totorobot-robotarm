#include "data_display.h"

void start_lcd_data_display(void)
{
	log_msg(USER, "Stopping LCD log utility\n");
	log_msg(USER, "Starting LCD data display thread\n");

	init_buttons();

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
	BSP_LCD_DisplayStringAtLine(8, (uint8_t*) "          srv0   srv1   srv2");

	draw_buttons();

	// Start continuously updating data on display
	lcd_data_display_on = 1;

    osThreadDef(LCD_DATA_DISPLAY, lcd_data_display_thread, osPriorityBelowNormal, 0, configMINIMAL_STACK_SIZE * 15);
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

	touch_t touch_current;
	uint8_t finger_down = 0;
	uint8_t changed = 0;

	uint32_t pulse[SERVOS];
	angles_t ang_servos_rad;
	uint32_t adc[SERVOS];
	coord_cart_t arm_position;
	char target_text[100];

	while (lcd_data_display_on) {

		/*
		 * Calculate values
		 */

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

		/*
		 * Display values
		 */

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
		sprintf(lcd_data_buff, "  angle:  %4d   %4d   %4d [deg]", rad_to_deg(ang_servos_rad.theta0),
				rad_to_deg(ang_servos_rad.theta1), rad_to_deg(ang_servos_rad.theta2));
		BSP_LCD_DisplayStringAtLine(9, (uint8_t*) lcd_data_buff);
		BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

		// Print pulse values
		sprintf(lcd_data_buff, "  pulse:  %4lu   %4lu   %4lu", pulse[0], pulse[1], pulse[2]);
		BSP_LCD_DisplayStringAtLine(10, (uint8_t*) lcd_data_buff);

		// Print ADC values
		sprintf(lcd_data_buff, "  ADC:    %4lu   %4lu   %4lu /%d", adc[0], adc[1], adc[2], MAX_ADC_P);
		BSP_LCD_DisplayStringAtLine(11, (uint8_t*) lcd_data_buff);

		/*
		 * Handle touch screen
		 */

		// Read touch values
		BSP_TS_GetState(&TS_State);
		if (TS_State.touchDetected) {

			// Get touch coordinates
			touch_current.x = TS_State.touchX[0];
			touch_current.y = TS_State.touchY[0];

			// We only care about touches over the touchable buttons
			for (int i = 0; i < BUTTONS; i++) {

				// Check only touchable buttons
				if (finger_down && !changed && buttons[i].touchable &&
				    (touch_current.x >= buttons[i].x) &&
					(touch_current.x <= buttons[i].x + buttons[i].width) &&
					(touch_current.y >= buttons[i].y) &&
					(touch_current.y <= buttons[i].y + buttons[i].height)) {

					// Execute button function based on actual state
					if (buttons[i].state == 0) {

						switch (i) {

						case 0: // STOP
							// Turn off power
							HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_SET);

							// Stop G-code reader
							end_moving = 1;
							file_reader_on = 0;

							// Stop drawer

							// Stop ADC
							stop_adc_thread();

							// Disable all touch buttons and reset their state
							for (int i = 0; i < BUTTONS; i++) {
								buttons[i].touchable = 0;
								if (i != 0) {
									buttons[i].state = 0;
								}
							}
							break;

						case 1: // G-code

							// TODO start G-code reader thread
							{
								// Launch G-code reader with the given file name
								osThreadDef(G_FILE_READ, file_reader_thread, osPriorityBelowNormal, 0, configMINIMAL_STACK_SIZE * 10);
								osThreadCreate (osThread(G_FILE_READ), "test1.g");
								log_msg(USER, "G-code reader thread started.\n");

								// Launch process to set pulse
								osThreadDef(SET_POSITION, set_position_thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 10);
								osThreadCreate (osThread(SET_POSITION), NULL);
								log_msg(USER, "Set position thread started.\n");

								buttons[i].touchable = 0;
							}
							break;

						case 2: // Draw
							// TODO start draw TCP server thread
							break;

						case 3: // Start ADC
							start_adc_thread();
							break;

						case 8: // Gripper open
							// TODO start gripper open thread
							break;

						case 9: // Gripper close
							// TODO start gripper close thread
							break;
						}

					} else {

						switch (i) {
						case 1: // G-code
							// TODO stop G-code reader thread
							break;
						case 2: // Draw
							// TODO stop draw TCP server thread
							break;
						case 3: // Stop ADC
							stop_adc_thread();
							break;
						}
					}

					// Set new button state
					buttons[i].state = !buttons[i].state;

					// Set sticky touch
					changed = 1;

					// Break after first occurrence as we only expect one button
					// to be pressed at a time
					break;
				}

			} // end for

			finger_down = 1;

		} else {

			// No touch
			finger_down = 0;
			changed = 0;
		}

		/*
		 * Display buttons
		 */

		// Reset draw button

		// Draw updated buttons
		draw_buttons();

		osDelay(50);
	}

	while (1) {
		log_msg(USER, "LCD data display thread terminated\n");
		osThreadTerminate(NULL);
    }
}

void draw_buttons(void)
{
	for (int i = 0; i < BUTTONS; i++) {

		// Set color and text based on button state
		if (buttons[i].state == 0) {
			BSP_LCD_SetTextColor(buttons[i].btn_color0);
			BSP_LCD_FillRect(buttons[i].x, buttons[i].y, buttons[i].width, buttons[i].height);
			BSP_LCD_SetBackColor(buttons[i].btn_color0);
			BSP_LCD_SetTextColor(buttons[i].text_color0);
			BSP_LCD_DisplayStringAt(buttons[i].text_x0, buttons[i].text_y, (uint8_t*) buttons[i].text0, LEFT_MODE);
		} else {
			BSP_LCD_SetTextColor(buttons[i].btn_color1);
			BSP_LCD_FillRect(buttons[i].x, buttons[i].y, buttons[i].width, buttons[i].height);
			BSP_LCD_SetBackColor(buttons[i].btn_color1);
			BSP_LCD_SetTextColor(buttons[i].text_color1);
			BSP_LCD_DisplayStringAt(buttons[i].text_x1, buttons[i].text_y, (uint8_t*) buttons[i].text1, LEFT_MODE);
		}
	}

	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

	return;
}

void init_buttons(void)
{
	// Stop/Reset position button
	buttons[0].x = 396;
	buttons[0].y = 14;
	buttons[0].width = 70;
	buttons[0].height = 50;
	buttons[0].btn_color0 = LCD_COLOR_RED;
	buttons[0].btn_color1 = LCD_COLOR_YELLOW;
	buttons[0].text_y = buttons[0].y + 18;
	buttons[0].text_x0 = 408;
	buttons[0].text_color0 = LCD_COLOR_WHITE;
	strcpy(buttons[0].text0, "STOP");
	buttons[0].text_x1 = 403;
	buttons[0].text_color1 = LCD_COLOR_BLACK;
	strcpy(buttons[0].text1, "Reset");
	buttons[0].touchable = 1;
	buttons[0].state = 0;

	// G-code button
	buttons[1].x = 396;
	buttons[1].y = 80;
	buttons[1].width = 70;
	buttons[1].height = 50;
	buttons[1].btn_color0 = LCD_COLOR_LIGHTGRAY;
	buttons[1].btn_color1 = LCD_COLOR_GREEN;
	buttons[1].text_y = buttons[1].y + 18;
	buttons[1].text_x0 = 398;
	buttons[1].text_color0 = LCD_COLOR_BLACK;
	strcpy(buttons[1].text0, "G-code");
	buttons[1].text_x1 = 398;
	buttons[1].text_color1 = LCD_COLOR_BLACK;
	strcpy(buttons[1].text1, "G-code");
	buttons[1].touchable = 1;
	buttons[1].state = 0;

	// Drawing button
	buttons[2].x = 396;
	buttons[2].y = 146;
	buttons[2].width = 70;
	buttons[2].height = 50;
	buttons[2].btn_color0 = LCD_COLOR_LIGHTGRAY;
	buttons[2].btn_color1 = LCD_COLOR_GREEN;
	buttons[2].text_y = buttons[2].y + 18;
	buttons[2].text_x0 = 408;
	buttons[2].text_color0 = LCD_COLOR_BLACK;
	strcpy(buttons[2].text0, "Draw");
	buttons[2].text_x1 = 408;
	buttons[2].text_color1 = LCD_COLOR_BLACK;
	strcpy(buttons[2].text1, "Draw");
	buttons[2].touchable = 1;
	buttons[2].state = 0;

	// ADC button
	buttons[3].x = 396;
	buttons[3].y = 212;
	buttons[3].width = 70;
	buttons[3].height = 50;
	buttons[3].btn_color0 = LCD_COLOR_LIGHTGRAY;
	buttons[3].btn_color1 = LCD_COLOR_GREEN;
	buttons[3].text_y = buttons[3].y + 18;
	buttons[3].text_x0 = 414;
	buttons[3].text_color0 = LCD_COLOR_BLACK;
	strcpy(buttons[3].text0, "ADC");
	buttons[3].text_x1 = 414;
	buttons[3].text_color1 = LCD_COLOR_BLACK;
	strcpy(buttons[3].text1, "ADC");
	buttons[3].touchable = 1;
	buttons[3].state = 0;

	// End position switch C
	buttons[4].x = 332;
	buttons[4].y = 212;
	buttons[4].width = 48;
	buttons[4].height = 50;
	buttons[4].btn_color0 = LCD_COLOR_LIGHTGREEN;
	buttons[4].btn_color1 = LCD_COLOR_RED;
	buttons[4].text_y = buttons[4].y + 18;
	buttons[4].text_x0 = buttons[4].x + 18;
	buttons[4].text_color0 = LCD_COLOR_BLACK;
	strcpy(buttons[4].text0, "C");
	buttons[4].text_x1 = buttons[4].x + 18;
	buttons[4].text_color1 = LCD_COLOR_WHITE;
	strcpy(buttons[4].text1, "C");
	buttons[4].touchable = 0;
	buttons[4].state = 0;

	// End position switch B
	buttons[5].x = 268;
	buttons[5].y = 212;
	buttons[5].width = 48;
	buttons[5].height = 50;
	buttons[5].btn_color0 = LCD_COLOR_LIGHTGREEN;
	buttons[5].btn_color1 = LCD_COLOR_RED;
	buttons[5].text_y = buttons[5].y + 18;
	buttons[5].text_x0 = buttons[5].x +18;
	buttons[5].text_color0 = LCD_COLOR_BLACK;
	strcpy(buttons[5].text0, "B");
	buttons[5].text_x1 = buttons[5].x + 18;
	buttons[5].text_color1 = LCD_COLOR_WHITE;
	strcpy(buttons[5].text1, "B");
	buttons[5].touchable = 0;
	buttons[5].state = 0;

	// End position switch A
	buttons[6].x = 204;
	buttons[6].y = 212;
	buttons[6].width = 48;
	buttons[6].height = 50;
	buttons[6].btn_color0 = LCD_COLOR_LIGHTGREEN;
	buttons[6].btn_color1 = LCD_COLOR_RED;
	buttons[6].text_y = buttons[6].y + 18;
	buttons[6].text_x0 = buttons[6].x + 18;
	buttons[6].text_color0 = LCD_COLOR_BLACK;
	strcpy(buttons[6].text0, "A");
	buttons[6].text_x1 = buttons[6].x + 18;
	buttons[6].text_color1 = LCD_COLOR_WHITE;
	strcpy(buttons[6].text1, "A");
	buttons[6].touchable = 0;
	buttons[6].state = 0;

	// Empty button
	buttons[7].x = 140;
	buttons[7].y = 212;
	buttons[7].width = 48;
	buttons[7].height = 50;
	buttons[7].btn_color0 = LCD_COLOR_WHITE;
	buttons[7].btn_color1 = LCD_COLOR_WHITE;
	buttons[7].text_y = buttons[7].y + 18;
	buttons[7].text_x0 = buttons[7].x + 18;
	buttons[7].text_color0 = LCD_COLOR_BLACK;
	strcpy(buttons[7].text0, "");
	buttons[7].text_x1 = buttons[7].x + 18;
	buttons[7].text_color1 = LCD_COLOR_BLACK;
	strcpy(buttons[7].text1, "");
	buttons[7].touchable = 0;
	buttons[7].state = 0;

	// Gripper close
	buttons[8].x = 76;
	buttons[8].y = 212;
	buttons[8].width = 48;
	buttons[8].height = 50;
	buttons[8].btn_color0 = LCD_COLOR_DARKGRAY;
	buttons[8].btn_color1 = LCD_COLOR_GREEN;
	buttons[8].text_y = buttons[8].y + 18;
	buttons[8].text_x0 = buttons[8].x + 13;
	buttons[8].text_color0 = LCD_COLOR_WHITE;
	strcpy(buttons[8].text0, "<>");
	buttons[8].text_x1 = buttons[8].x + 13;
	buttons[8].text_color1 = LCD_COLOR_BLACK;
	strcpy(buttons[8].text1, "<>");
	buttons[8].touchable = 1;
	buttons[8].state = 0;

	// Gripper open
	buttons[9].x = 12;
	buttons[9].y = 212;
	buttons[9].width = 48;
	buttons[9].height = 50;
	buttons[9].btn_color0 = LCD_COLOR_DARKGRAY;
	buttons[9].btn_color1 = LCD_COLOR_GREEN;
	buttons[9].text_y = buttons[9].y + 18;
	buttons[9].text_x0 = buttons[9].x + 13;
	buttons[9].text_color0 = LCD_COLOR_WHITE;
	strcpy(buttons[9].text0, "><");
	buttons[9].text_x1 = buttons[9].x + 13;
	buttons[9].text_color1 = LCD_COLOR_BLACK;
	strcpy(buttons[9].text1, "><");
	buttons[9].touchable = 1;
	buttons[9].state = 0;

	return;
}
