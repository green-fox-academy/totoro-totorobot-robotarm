/* Includes ------------------------------------------------------------------*/
#include "rtc.h"
#include "stm32f7xx_hal_rtc.h"
#include "stm32f7xx_hal_rcc.h"
#include "client.h"
#include "lcd_log.h"
#include "cmsis_os.h"
#include "ethernetif.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "app_ethernet.h"
#include "lcd_log.h"
#include "main.h"



/** @addtogroup CORE
  * @{
  */

/** @defgroup KERNEL_RTC
  * @brief Kernel rtc routines
  * @{
  */
/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */

#define SENSOR_ADDRESS 0xD0 // set this according to HW configuration

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Include core modules */

RTC_HandleTypeDef RtcHandle;
RTC_InitTypeDef rtcInit;

/**
  * @brief  Configure the current time and date.
  * @param  None
  * @retval None
  */
void rtc_init(void)
{

	// Enable RTC //
	__HAL_RCC_RTC_ENABLE();

	/*##-1- Configure the RTC peripheral #######################################*/
	/* Configure RTC prescaler and RTC data registers */
	/* RTC configured as follow:
	- Hour Format    = Format 24
	- Asynch Prediv  = Value according to source clock
	- Synch Prediv   = Value according to source clock
	- OutPut         = Output Disable
	- OutPutPolarity = High Polarity
	- OutPutType     = Open Drain */
	RtcHandle.Instance = RTC;
	RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
	RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
	RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
	RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
	RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

	if(HAL_RTC_Init(&RtcHandle) != HAL_OK) {
		LCD_UsrLog("RTC error: RTC initialization failed.");
	}
}


void rtc_set(void)
{
	struct tm input_time = *localtime(&txTm);

	RTC_DateTypeDef dateStruct;
	RTC_TimeTypeDef timeStruct;

	timeStruct.Hours = input_time.tm_hour;
    timeStruct.Minutes = input_time.tm_min;
    timeStruct.Seconds = input_time.tm_sec;
    timeStruct.TimeFormat = RTC_HOURFORMAT_24;
    timeStruct.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    timeStruct.StoreOperation = RTC_STOREOPERATION_RESET;

    dateStruct.WeekDay = input_time.tm_wday;
    dateStruct.Month = input_time.tm_mon;
	dateStruct.Date = input_time.tm_mday;
	dateStruct.Year = input_time.tm_year;

	HAL_RTC_SetTime(&RtcHandle, &timeStruct, RTC_FORMAT_BIN);
	HAL_RTC_SetDate(&RtcHandle, &dateStruct, RTC_FORMAT_BIN);

}

void rtc_get_time_thread(void const * argument)
{
	while(1){
	rtc_set();
	printf("RTC TIME:\n");
	//rtc_data_t* rtc_data = (rtc_data_t*)parameter;

	RTC_DateTypeDef dateStruct;
	RTC_TimeTypeDef timeStruct;

	HAL_RTC_GetTime(&RtcHandle, &timeStruct, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&RtcHandle, &dateStruct, RTC_FORMAT_BIN);
	char text[16] = {0};
	sprintf(text,"%.2d:%.2d:%.2d", timeStruct.Hours, timeStruct.Minutes, timeStruct.Seconds);
	LCD_UsrLog("Time: %s\n", text);
	osDelay(1000);
	}
	while (1) {
		/* Delete the Init Thread */
		osThreadTerminate(NULL);
}
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
