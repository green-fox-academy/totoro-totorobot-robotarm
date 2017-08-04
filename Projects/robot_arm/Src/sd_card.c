/* Includes ------------------------------------------------------------------*/
#include "sd_card.h"

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
	FRESULT res;                                          /* FatFs function common result code */
	uint32_t byteswritten, bytesread;                     /* File write/read counts */
	char wtext[] = "a"; /* File write buffer */
	char rtext[100];                                   /* File read buffer */
}
