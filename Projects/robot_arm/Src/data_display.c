#include "data_display.h"

void start_lcd_data_display(void)
{
	if (debug) {
		LCD_UsrLog((char*) "Stopping LCD log utility\n");
		LCD_UsrLog((char*) "Starting LCD data display thread\n");
	}
	// Disable debug
	debug = 0;

	// Disable LCD debug utility
	LCD_LOG_DeInit();

	BSP_LCD_Clear(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_RED);
	BSP_LCD_SetFont(&Font20);

	BSP_LCD_DisplayStringAtLine(1, (uint8_t*) "  TotoRobot runtime parameters   ");
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAtLine(3, (uint8_t*) "Coordinates");
	BSP_LCD_DisplayStringAtLine(5, (uint8_t*) "   X:      Y:      Z:");
	BSP_LCD_DisplayStringAtLine(7, (uint8_t*) "       Serv0  Serv1  Serv2  Serv3");
	BSP_LCD_DisplayStringAtLine(9, (uint8_t*) "angle:");
	BSP_LCD_DisplayStringAtLine(10, (uint8_t*) "pulse:");
	BSP_LCD_DisplayStringAtLine(11, (uint8_t*) "ADC:");

	// Start continuously updating data on display
	lcd_data_display_on = 1;

    osThreadDef(LCD_DATA_DISPLAY, lcd_data_display_thread, osPriorityLow, 0, configMINIMAL_STACK_SIZE * 5);
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

	// Enable debug
	debug = 1;

	if (debug) {
		LCD_UsrLog((char*) "LCD logging utility started\n");
	}
	return;
}

void lcd_data_display_thread(void const * argument)
{
	while (lcd_data_display_on) {

		/* Lines to print:
		 *  Title
		 *  Coordinates
		 *  XYZ coord
		 *  Ang RZ coord
		 *  J0-3
		 *  Angle degree
		 *  S0-4
		 *  ADC
		 *  Angle deg
		 *  PWM pulse
		 */

	//	sprintf(lcd_data_buff, "   X: %4d  Y: %4d  Z: %4d", arm_pos_c.x, arm_pos_c.y, arm_pos_c.z);
	//	BSP_LCD_DisplayStringAtLine(2, (uint8_t*) lcd_data_buff);

		osDelay(10);
	}

	while(1) {
		if (debug) {
			LCD_UsrLog((char*) "LCD data display thread terminated\n");
		}
		osThreadTerminate(NULL);
    }
}

