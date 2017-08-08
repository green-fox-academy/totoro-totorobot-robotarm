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

	// Show data
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

	lcd_data_display_on = 1;

    osThreadDef(LCD_DATA_DISPLAY, lcd_data_display_thread, osPriorityLow, 0, configMINIMAL_STACK_SIZE * 5);
    osThreadCreate (osThread(LCD_DATA_DISPLAY), NULL);

	return;
}

void stop_lcd_data_display(void)
{
	// Restart LCD logging
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	LCD_LOG_Init();

	// Show Header and Footer texts
	LCD_LOG_SetHeader((uint8_t *)"TotoRobot - robot arm");
	LCD_LOG_SetFooter((uint8_t *)"STM32746G-DISCO - GreenFoxAcademy");

	if (debug) {
		LCD_UsrLog((char*) "LCD logging utility started\n");
	}
	return;
}

void lcd_data_display_thread(void const * argument)
{
	while (lcd_data_display_on) {

		sprintf(lcd_data_buff, "ADC0: %4d  ADC1: %4d  ADC2: %4d  ADC3: %4d", adc_values[0], adc_values[1], adc_values[2], adc_values[3]);
		BSP_LCD_DisplayStringAtLine(0, (uint8_t*) lcd_data_buff);
		BSP_LCD_DisplayStringAtLine(1, (uint8_t*) lcd_data_buff);

		osDelay(10);
	}

	while(1) {
		if (debug) {
			LCD_UsrLog((char*) "LCD data display thread terminated\n");
		}
		osThreadTerminate(NULL);
    }
}

