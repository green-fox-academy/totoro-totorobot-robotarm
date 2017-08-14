#ifndef __ROBOT_ARM_H_
#define __ROBOT_ARM_H_

#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "lcd_log.h"
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include "main.h"
#include <stdio.h>


#include "stm32f7xx_hal_rtc.h"
//#include "client.h"
#include <time.h>

//#include "defines.h"


#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */

#define MIN_DEGREE	0
#define MAX_DEGREE	180
#define MIN_POS_DUTY_CYCLE	5	// pwm duty cycle in %
#define MAX_POS_DUTY_CYCLE	10	// pwm duty cycle in %
#define MIN_ADC_VALUE		0
#define MAX_ADC_VALUE		4095

#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */

ADC_HandleTypeDef adc_handle;
ADC_ChannelConfTypeDef adc_ch_conf;
TIM_HandleTypeDef pwm_handle;
TIM_OC_InitTypeDef pwm_oc_init;
uint16_t position; // angle of rotation 0-180 degrees
uint8_t debug;

int udp_client_ready;
int udp_send_allowed;

time_t txTm;

void servo_control_thread(void const * argument);

void pwm_init(void);
void adc_init(void);
void pwm_set_duty(uint8_t rot_degree);
void pwm_set_duty_from_adc(void);
uint8_t get_degrees(void);
void udp_client_thread(void const *argument);
//void rtc_init(void);
//void rtc_set(void);

void rtc_init(void);
void rtc_set(void);
void rtc_get_time_thread(void const * argument);

#endif /* __ROBOT_ARM_H_ */
