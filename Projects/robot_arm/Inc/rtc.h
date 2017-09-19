#ifndef __RTC_H_
#define __RTC_H_

#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include "stm32f7xx_hal_rtc.h"
#include "stm32f7xx_hal_rcc.h"
#include "lwip/sockets.h"
#include "app_ethernet.h"
#include "sd_card.h"

#define NTP_TIMESTAMP_DELTA		2208988800ull
#define NTP_SERVER_IP			"193.6.222.47"

#define RTC_ASYNCH_PREDIV	124
#define RTC_SYNCH_PREDIV	7999

RTC_HandleTypeDef rtc;

void ntp_client_thread(void const *argument);
void rtc_init(void);
void rtc_set(time_t* ntp_time);

#endif // __RTC_H_
