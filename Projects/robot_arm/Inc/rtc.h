/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RTC_H
#define __RTC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f7xx_hal_rtc.h"
#include "client.h"
#include <time.h>

/* Exported types ------------------------------------------------------------*/
#include "stm32f7xx_hal.h"
/* Include my libraries here */
#include "defines.h"
#include "tm_stm32_disco.h"
#include "tm_stm32_delay.h"
#include "tm_stm32_rtc.h"
#include "tm_stm32_lcd.h"
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

 typedef struct
 {
	unsigned char second;
	unsigned char minute;
	unsigned char hour;
	unsigned char day;
	unsigned char month;
	unsigned char year;
 }rtc_data_t;

void rtc_get(rtc_data_t* rtc_data);
void rtc_set(rtc_data_t* rtc_data);

void k_CalendarBkupInit(void);
void k_BkupSaveParameter(uint32_t address, uint32_t data);
uint32_t k_BkupRestoreParameter(uint32_t address);

void k_SetTime(RTC_TimeTypeDef *Time);
void k_GetTime(RTC_TimeTypeDef *Time);
void k_SetDate(RTC_DateTypeDef *Date);
void k_GetDate(RTC_DateTypeDef *Date);

void time_on_board_thread(void* parameter);

#ifdef __cplusplus
}
#endif

#endif /*__K_RTC_H */
