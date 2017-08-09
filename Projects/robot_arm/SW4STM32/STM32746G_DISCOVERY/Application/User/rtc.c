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
RTC_TimeTypeDef ntp_time;



/* Include core modules */


RTC_HandleTypeDef RtcHandle;

typedef struct
{
	uint8_t		 Hours;
	uint8_t	 	 Minutes;
	uint8_t	 	 Seconds;
	uint32_t	 SubSeconds;
	uint32_t	 SecondFraction;
	uint8_t	     TimeFormat;
	uint32_t	 DayLightSaving;
	uint32_t	 StoreOperation;

}time_data_t;


/**
  * @brief  Configure the current time and date.
  * @param  None
  * @retval None
  */
void k_CalendarBkupInit(void)
{
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
	}
}

/**
  * @brief RTC MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  * @param  hrtc: RTC handle pointer
  * @retval None
  */
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
	RCC_OscInitTypeDef        RCC_OscInitStruct;
	RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

	/*##-1- Configure LSE as RTC clock source ##################################*/
	RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
	if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		return;
	}

	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
	if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
		return;
	}

  /*##-2- Enable RTC peripheral Clocks #######################################*/
  /* Enable RTC Clock */
	__HAL_RCC_RTC_ENABLE();
}

/**
  * @brief RTC MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  * @param  hrtc: RTC handle pointer
  * @retval None
  */
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc)
{
	/*##-1- Reset peripherals ##################################################*/
	__HAL_RCC_RTC_DISABLE();
}


/**
  * @brief  Backup save parameter
  * @param  address: RTC Backup data Register number.
  *                  This parameter can be: RTC_BKP_DRx where x can be from 0 to 19 to
  *                                         specify the register.
  * @param  Data:    Data to be written in the specified RTC Backup data register.
  * @retval None
  */
void k_BkupSaveParameter(uint32_t address, uint32_t data)
{
	HAL_RTCEx_BKUPWrite(&RtcHandle,address,data);
}

/**
  * @brief  Backup restore parameter.
  * @param  address: RTC Backup data Register number.
  *                  This parameter can be: RTC_BKP_DRx where x can be from 0 to 19 to
  *                                         specify the register.
  * @retval None
  */
uint32_t k_BkupRestoreParameter(uint32_t address)
{
	return HAL_RTCEx_BKUPRead(&RtcHandle,address);
}

/**
  * @brief  RTC Get time.
  * @param  Time: Pointer to Time structure
  * @retval None
  */

/**
  * @brief  RTC Set time.
  * @param  Time: Pointer to Time structure
  * @retval None
  */

/**
  * @brief  RTC Get date
  * @param  Date: Pointer to Date structure
  * @retval None
  */
void k_GetDate(RTC_DateTypeDef *Date)
{
	HAL_RTC_GetDate(&RtcHandle, Date, FORMAT_BIN);

	if((Date->Date == 0) || (Date->Month == 0)) {
		Date->Date = Date->Month = 1;
	}
}
/**
  * @brief  RTC Set date
  * @param  Date: Pointer to Date structure
  * @retval None
  */
void k_SetDate(RTC_DateTypeDef *Date)
{
	HAL_RTC_SetDate(&RtcHandle, Date, FORMAT_BIN);
}
/**
  * @}
  */
void time_on_board_thread(void* parameter)
{
	//rtc_data_t* input_time = (rtc_data_t*)parameter;

	// time_t t = time(NULL);
	// struct tm tm = *localtime(&t);

	struct tm input_time = &txTm;

	ntp_time.Seconds = input_time.tm_sec;
	ntp_time.Minutes = input_time.tm_min;
	ntp_time.Hours = input_time.tm_hour;
	HAL_RTC_SetTime(RtcHandle, *ntp_time, RTC_HOURFORMAT_24);

	LCD_UsrLog("Time now:  %d:%d:%d\n", input_time.tm_hour, input_time.tm_min, input_time.tm_sec);


	HAL_RTC_GetTime(RtcHandle, *input_time, RTC_FORMAT_BIN);
	//HAL_RTC_GetTime();
	//LCD_UsrLog("Time: %s", time_out);
	/*LCD_UsrLog("loooooooooooooooolllll");
	char text[16] = {0};
	sprintf(text,"%.2u:%.2u:%.2u",rtc_data->hour,rtc_data->minute,rtc_data->second);
	LCD_UsrLog("Time: %s", text);*/

	//printf("now: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}
/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
