#include "data_display.h"

void lcd_show_data_thread(void const * argument)
{
	if (debug) {
		LCD_UsrLog((char*) "LCD show data thread started\n");
	}




	while (1) {
		if (debug) {
			LCD_UsrLog((char*) "LCD show data thread terminated\n");
		}
		osThreadTerminate(NULL);
    }
}

