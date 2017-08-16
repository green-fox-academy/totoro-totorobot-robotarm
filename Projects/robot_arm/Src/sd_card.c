/* Includes ------------------------------------------------------------------*/
#include "sd_card.h"
#include "lcd_log.h"
#include <string.h>
#include "cmsis_os.h"

/* Private variables ---------------------------------------------------------*/
FATFS SDFatFs;  /* File system object for SD card logical drive */
FIL MyFile;     /* File object */
char SDPath[4]; /* SD card logical drive path */
FRESULT res;    /* FatFs function common result code */

uint32_t bytesread;      				            /* File write/read counts */
uint32_t size;
char wtext[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n"; /* File write buffer */
char btext[] = "STM1.TXT";						/* Name of the file */
char rtext[100];                                   	/* File read buffer */

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

void read_sd_card()
{
	FatFs_Init();

	/*## Open the text file object with read access ###############*/
	res = f_open(&MyFile, btext, FA_READ);
	if (res != FR_OK)
		LCD_ErrLog((char*) "Open the file has failed.\n");

	/*## Read data from the text file ###########################*/
	f_read(&MyFile, rtext, sizeof(rtext), (UINT*)&bytesread);
	LCD_UsrLog((char*) rtext);
	LCD_UsrLog("\n");
	osDelay(2000);

    char buff[3][50];
    char * pch;
	pch = strtok (rtext,"\n");
	uint8_t i = 0;

	while (pch != NULL) {
		//LCD_UsrLog("%s\n", pch);
		strcpy(buff[i], pch);
		pch = strtok(NULL, ";");
		i++;
	}

	for (uint8_t j = 0; j <= i; j++) {
		//LCD_UsrLog("%s\n", buff[j]);
		pch = strtok(buff[j]," ");
		while (pch != NULL) {
			LCD_UsrLog("%s\n", pch);
			pch = strtok(NULL, " ");
		}
		osDelay(5000);
	}

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
