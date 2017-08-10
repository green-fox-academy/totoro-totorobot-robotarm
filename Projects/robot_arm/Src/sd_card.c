/* Includes ------------------------------------------------------------------*/
#include "sd_card.h"
#include "lcd_log.h"

/* Private variables ---------------------------------------------------------*/
FATFS SDFatFs;  /* File system object for SD card logical drive */
FIL MyFile;     /* File object */
char SDPath[4]; /* SD card logical drive path */

/* Private function prototypes -----------------------------------------------*/
void FatFs_Init()
{
	/*## Link the micro SD disk I/O driver ################################*/
	FATFS_LinkDriver(&SD_Driver, SDPath);
	LCD_UsrLog((char*) "Link the micro SD disk I/O driver is successful.\n");

	/*## Register the file system object to the FatFs module ##########*/
	f_mount(&SDFatFs, (TCHAR const*)SDPath, 0);
}

void read_sd_card()
{
	FRESULT res;                                       	/* FatFs function common result code */
	uint32_t bytesread;      				            /* File write/read counts */
	char btext[] = "STM333.TXT";						/* Name of the file */
	char rtext[100];                                   	/* File read buffer */

	FatFs_Init();
	/*##-7- Open the text file object with read access ###############*/
	if(f_open(&MyFile, "STM32.TXT", FA_READ) != FR_OK)
	{
		/* 'STM32.TXT' file Open for read Error */
		Error_Handler();
	}
	else
	{
		/*##-8- Read data from the text file ###########################*/
		f_read(&MyFile, rtext, sizeof(rtext), (UINT*)&bytesread);

		/*##-9- Close the open text file #############################*/
		f_close(&MyFile);
	}
}

void write_sd_card()
{
	FRESULT res;                                        						/* FatFs function common result code */
	uint32_t size;                   											/* File write/read counts */
	char wtext[] = "Link the micro SD disk I/O driver is successfuuuuuuuuuuuul\n"; 		/* File write buffer */
	char btext[] = "STM333.TXT";												/* Name of the file */

	FatFs_Init();
	/*## Open a new an existing text file object with write access #####*/
	f_open(&MyFile, btext, FA_OPEN_EXISTING | FA_WRITE);

	/*## Write data to the text file ################################*/
	size = (&MyFile)->fsize;
	res = f_lseek(&MyFile, size);
	f_printf(&MyFile, wtext);

	LCD_UsrLog((char*) wtext);
	LCD_UsrLog((char*) "Data has written to SD card.\n");

	/*## Close the open text file #################################*/
	f_close(&MyFile);
	LCD_UsrLog((char*) "Close the open text file\n");
}

static void Error_Handler(void)
{
  /* Turn LED1 on */
  BSP_LED_On(LED1);
  while(1)
  {
    BSP_LED_Toggle(LED1);
    HAL_Delay(200);
  }
}
