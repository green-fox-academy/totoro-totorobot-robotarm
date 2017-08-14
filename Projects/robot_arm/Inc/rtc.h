/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RTC_H
#define __RTC_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f7xx_hal_rtc.h"
#include "client.h"
#include <time.h>

/* Exported types ------------------------------------------------------------*/
#include "stm32f7xx_hal.h"
/* Include my libraries here */
#include "defines.h"
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"
#include <stdio.h>

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Defines related to Clock configuration */
#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void rtc_init(void);
void rtc_set(void);

void rtc_get_time_thread(void const * argument);
#endif /*__RTC_H */
