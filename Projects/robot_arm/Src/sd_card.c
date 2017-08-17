/* Includes ------------------------------------------------------------------*/
#include "sd_card.h"
#include "lcd_log.h"
#include <string.h>
#include <stdlib.h>
#include "cmsis_os.h"

/* Private variables ---------------------------------------------------------*/
FATFS SDFatFs;  /* File system object for SD card logical drive */
FIL MyFile;     /* File object */
char SDPath[4]; /* SD card logical drive path */
FRESULT res;    /* FatFs function common result code */

uint32_t bytesread;      				            /* File write/read counts */
uint64_t size;
char wtext[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n"; /* File write buffer */
char btext[] = "STM1.TXT";						/* Name of the file */
char readf[] = "STM2.txt";
char rtext[10];												/* File read buffer */
char buff[100];
char tomb[10];
/* Private function prototypes -----------------------------------------------*/
int file_length(FIL* fp)
{
	uint32_t size;

	size = sizeof(&MyFile);
	return size;
}

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

void read_sd_card()
{
	FatFs_Init();

	/*## Open the text file object with read access ###############*/
	res = f_open(&MyFile, readf, FA_READ);
	if (res != FR_OK)
		LCD_ErrLog((char*) "Open the file has failed.\n");

	/*## Read data from the text file ###########################*/
	int boolean = 0;
	char* pch;
	while (boolean == 0) {
		f_gets(buff, 100, &MyFile);
		LCD_UsrLog((char*) buff);
		pch = strtok(buff," ");
		if (buff[0] == 'G')
			boolean = 1;
	}
	LCD_UsrLog("%s\n", pch);
	uint8_t i = 0;
	while (pch != NULL) {
		if (i == 0) {
			raw_G_code.G_code = atoi(pch);
			LCD_UsrLog("raw_G_code.G_code: %d\n", raw_G_code.G_code);
		} else if (i == 1) {
			raw_G_code.x_param = atof(pch);
			LCD_UsrLog("raw_G_code.x_param: %f\n", raw_G_code.x_param);
		} else if (i == 2) {
			raw_G_code.y_param = atof(pch);
			LCD_UsrLog("raw_G_code.y_param: %f\n", raw_G_code.y_param);
		} else {
			raw_G_code.e_param = atof(pch);
			LCD_UsrLog("raw_G_code.e_param: %f\n", raw_G_code.e_param);
		}
		LCD_UsrLog("%d: %s\n", i, pch);
		pch = strtok(NULL, " XYE");
		i++;
	}

	/*while (1) {
	f_gets(buff, 100, &MyFile);
	LCD_UsrLog((char*) buff);
	}*/
	/*##-9- Close the open text file #############################*/
	f_close(&MyFile);
}

void write_sd_card()
{
	FatFs_Init();

	/*## Open a new an existing text file object with write access #####*/
	if (f_open(&MyFile, btext, FA_OPEN_ALWAYS | FA_WRITE) == FR_OK) {
		LCD_UsrLog((char*) "Open an existing file\n");
	} else {
		f_open(&MyFile, btext, FA_CREATE_ALWAYS | FA_WRITE);
		LCD_UsrLog((char*) "Create and open a new file\n");
	}

	/*## Write data to the text file ################################*/
	size = (&MyFile)->fsize;
	f_lseek(&MyFile, size);
	f_printf(&MyFile, wtext);
	LCD_UsrLog((char*) wtext);

	/*## Close the open text file #################################*/
	f_close(&MyFile);
	LCD_UsrLog((char*) "Close the open text file\n");
}
