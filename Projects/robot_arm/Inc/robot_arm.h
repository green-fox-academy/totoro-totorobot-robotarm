#ifndef __ROBOT_ARM_H_
#define __ROBOT_ARM_H_

#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "lcd_log.h"

#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define MIN_DEGREE	0
#define MAX_DEGREE	180
#define MIN_POS_DUTY_CYCLE	5				// pwm duty cycle in %
#define MAX_POS_DUTY_CYCLE	10				// pwm duty cycle in %
#define MIN_ADC_VALUE	0
#define MAX_ADC_VALUE	4095
#define UDP_SERVER_PORT		54002

ADC_HandleTypeDef adc_handle;
ADC_ChannelConfTypeDef adc_ch_conf;
TIM_HandleTypeDef pwm_handle;
TIM_OC_InitTypeDef pwm_oc_init;
uint16_t position; 							// angle of rotation 0-180 degrees
uint8_t debug;

void servo_control_thread(void const * argument);
void socket_server_thread(void const * argument);
void touch_screen_test_thread(void const * argument);
void mouse_coordinate_thread(void const * argument);
void udp_server_thread(void const *argument);
void socket_client_thread(void const *argument);
uint8_t get_degrees(void);

int udp_server_ready;
char *client_ip;
int client_sock;

void pwm_init(void);
void adc_init(void);
void pwm_set_duty(uint8_t rot_degree);
void pwm_set_duty_from_adc(void);

#endif /* __ROBOT_ARM_H_ */
