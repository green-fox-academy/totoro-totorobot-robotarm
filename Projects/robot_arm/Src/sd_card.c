/* Includes ------------------------------------------------------------------*/
#include "sd_card.h"
#include "lcd_log.h"

/* Private variables ---------------------------------------------------------*/
FATFS SDFatFs;  /* File system object for SD card logical drive */
FIL MyFile;     /* File object */
char SDPath[4]; /* SD card logical drive path */

/* Private function prototypes -----------------------------------------------*/
//static void SystemClock_Config(void);
//static void Error_Handler(void);
//static void CPU_CACHE_Enable(void);

void sd_card()
{
	FRESULT res;                                        /* FatFs function common result code */
	uint32_t byteswritten, bytesread;                   /* File write/read counts */
	char wtext[] = "aa\n"; 								/* File write buffer */
	char btext[] = "STM333.TXT";						/* Name of the file */
	char rtext[100];                                   	/* File read buffer */

	/*##-1- Link the micro SD disk I/O driver ##################################*/
	if(FATFS_LinkDriver(&SD_Driver, SDPath) == 0)
	{
		LCD_UsrLog((char*) "Link the micro SD disk I/O driver is successful.\n");
		/*##-2- Register the file system object to the FatFs module ##############*/
	    if(f_mount(&SDFatFs, (TCHAR const*)SDPath, 0) != FR_OK)
	    {
			/* FatFs Initialization Error */
	    	Error_Handler();
	    	LCD_ErrLog("Register the file system object to the FatFs module is failed\n");
	    }
	    else
	    {
	        /*##-3- Create and Open a new text file object with write access #####*/
	        f_open(&MyFile, btext, FA_OPEN_EXISTING | FA_WRITE);
	        LCD_UsrLog((char*) "Create and Open a new text file object with write access.\n");

			/*##-4- Write data to the text file ################################*/
			f_lseek(&MyFile, sizeof(MyFile));
			sprintf((char*) wtext,"File.filscsíacdsvíds");
			f_write(&MyFile, wtext, sizeof(wtext), (void *)&byteswritten);
			LCD_UsrLog((char*) "Data has written to SD card1.\n");

			f_lseek(&MyFile, sizeof(MyFile));

			LCD_UsrLog((char*) "Data has written to SD card2.\n");

			/*##-5- Close the open text file #################################*/
			f_close(&MyFile);
			LCD_UsrLog((char*) " Close the open text file\n");
		}
	}
	  /*##-11- Unlink the micro SD disk I/O driver ###############################*/
	  FATFS_UnLinkDriver(SDPath);
	  LCD_ErrLog("Link the micro SD disk I/O driver has failed");
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
