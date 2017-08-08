#include "data_display.h"

void lcd_show_data_thread(void const * argument)
{
	if (debug) {
		LCD_UsrLog((char*) "LCD show data thread started\n");
	}

	// Disable debug and LCD debug utility
	debug = 0;
	LCD_LOG_DeInit();
	BSP_LCD_Clear(LCD_COLOR_WHITE);

	// Show data
	char lcd_buff[70];
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);



	sprintf(lcd_buff, "ADC0: %4d  ADC1: %4d  ADC2: %4d  ADC3: %4d", adc_values[0], adc_values[1], adc_values[2], adc_values[3]);
	BSP_LCD_DisplayStringAtLine(0, (uint8_t*) lcd_buff);
	BSP_LCD_DisplayStringAtLine(1, (uint8_t*) lcd_buff);

	osDelay(10);


	while (1) {
		// Restart LCD logging
		BSP_LCD_Clear(LCD_COLOR_WHITE);
		LCD_LOG_Init();

		// Show Header and Footer texts
		LCD_LOG_SetHeader((uint8_t *)"TotoRobot - robot arm");
		LCD_LOG_SetFooter((uint8_t *)"STM32746G-DISCO - GreenFoxAcademy");

		if (debug) {
			LCD_UsrLog((char*) "LCD show data thread terminated\n");
		}

		osThreadTerminate(NULL);
    }
}

