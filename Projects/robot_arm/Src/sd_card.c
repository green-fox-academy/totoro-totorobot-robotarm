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
uint64_t size;
char wtext[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n"; /* File write buffer */
char btext[] = "STM1.TXT";						/* Name of the file */
char readf[] = "STM2.txt";
char rtext[10];												/* File read buffer */
char buff[100];
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
	f_gets(buff, 100, &MyFile);
	LCD_UsrLog((char*) buff);
	//char str[] = "This a sample string";
	char* pch;
	pch = strtok (buff," ");
	while (pch != NULL) {
		LCD_UsrLog("%s\n", pch);
		pch = strtok(NULL, " ");
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
