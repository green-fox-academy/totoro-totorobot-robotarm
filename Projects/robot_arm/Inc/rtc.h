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
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void k_CalendarBkupInit(time_t txTm);
void k_BkupSaveParameter(uint32_t address, uint32_t data);
uint32_t k_BkupRestoreParameter(uint32_t address);

void k_SetTime(RTC_TimeTypeDef *Time);
void k_GetTime(RTC_TimeTypeDef *Time);
void k_SetDate(RTC_DateTypeDef *Date);
void k_GetDate(RTC_DateTypeDef *Date);

#ifdef __cplusplus
}
#endif

#endif /*__K_RTC_H */
