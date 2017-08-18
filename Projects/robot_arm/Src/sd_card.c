/* Includes ------------------------------------------------------------------*/
#include "sd_card.h"
#include "lcd_log.h"
#include <string.h>
#include <stdlib.h>

/* Private variables ---------------------------------------------------------*/
FATFS SDFatFs;  /* File system object for SD card logical drive */
FIL MyFile;     /* File object */
char SDPath[4]; /* SD card logical drive path */
FRESULT res;    /* FatFs function common result code */

uint64_t size;										/* Size of the text where the pointer show it */
uint16_t size2 = 0;
char log_text[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaa\n"; /* File write buffer */
char log_file[] = "STM1.TXT";						/* Name of the log file */
char read_file[] = "STM2.txt";						/* Name of the G code file */
char line_buffer[100];

/* Private function prototypes -----------------------------------------------*/
void FatFs_Init()
{
	/*## Link the micro SD disk I/O driver ################################*/
	res = (FATFS_LinkDriver(&SD_Driver, SDPath));
	LCD_UsrLog((char*) "Link the micro SD disk I/O driver is successful.\n");
	if (res != 0)
		LCD_ErrLog((char*) "Link the micro SD disk I/O driver has failed.\n");

	/*## Register the file system object to the FatFs module ##########*/
	res = (f_mount(&SDFatFs, (TCHAR const*)SDPath, 0));
	if (res != FR_OK)
			LCD_ErrLog((char*) "Mount the driver has failed.\n");
}

void read_G_code()
{
	/*## Open the text file object with read access ###############*/
	res = f_open(&MyFile, read_file, FA_READ);
	if (res != FR_OK)
		LCD_ErrLog((char*) "Open the file has failed.\n");

	/*## Read data from the text file ###########################*/
	int boolean = 0;
	char* pch;
	while (boolean == 0) {
		f_lseek(&MyFile, size2);
		f_gets(line_buffer, 100, &MyFile);
		size2 += strlen(line_buffer) + 1;

		if (line_buffer[0] == 'G') {
			boolean = 1;
			LCD_UsrLog((char*) line_buffer);
		}
		pch = strtok(line_buffer," ");
	}
	uint8_t i = 0;
	while (pch != NULL) {
		if (i == 0) {
			raw_G_code.G_code = atoi(pch);
		} else if (i == 1) {
			raw_G_code.x_param = atof(pch);
		} else if (i == 2) {
			raw_G_code.y_param = atof(pch);
		} else {
			raw_G_code.e_param = atof(pch);
		}
		pch = strtok(NULL, " XYE");
		i++;
	}

	/*##-9- Close the open text file #############################*/
	f_close(&MyFile);
}

void write_sd_card()
{
	FatFs_Init();

	/*## Open a new an existing text file object with write access #####*/
	if (f_open(&MyFile, log_file, FA_OPEN_ALWAYS | FA_WRITE) == FR_OK) {
		LCD_UsrLog((char*) "Open an existing file\n");
	} else {
		f_open(&MyFile, log_file, FA_CREATE_ALWAYS | FA_WRITE);
		LCD_UsrLog((char*) "Create and open a new file\n");
	}

	/*## Write data to the text file ################################*/
	size = (&MyFile)->fsize;
	f_lseek(&MyFile, size);
	f_printf(&MyFile, log_text);
	LCD_UsrLog((char*) log_text);

	/*## Close the open text file #################################*/
	f_close(&MyFile);
	LCD_UsrLog((char*) "Close the open text file\n");
}

void G_read_thread(void const * argument)
{
	uint8_t lock;

	osMutexWait(servo_ready_mutex, osWaitForever);
	lock = servo_pos_ready;
	osMutexRelease(servo_ready_mutex);

	if (lock == 1)
		read_G_code();

	osMutexWait(servo_ready_mutex, osWaitForever);
	servo_pos_ready = 0;
	osMutexRelease(servo_ready_mutex);
}

void test_timer_thread(void const * argument)
{
	osMutexWait(servo_ready_mutex, osWaitForever);
	servo_pos_ready = 1;
	osMutexRelease(servo_ready_mutex);

	osDelay(1000);
}
