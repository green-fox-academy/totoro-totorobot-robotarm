#ifndef __SERVO_CONTROL_H_
#define __SERVO_CONTROL_H_

#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "lcd_log.h"
#include "robot_arm_conf.h"
#include "kinematics.h"
#include <string.h>

#define SERVOS				4
#define MIN_ADC_VALUE		0
#define MAX_ADC_VALUE		4095

/* SERVO 0 Configuration */

// PWM D3 PB4 TIM3_CH1
// ADC A0 PA0 ADC3_IN0
// 0 degre @ 5165
#define SERVO0_INST			TIM3
#define SERVO0_PERIOD		0xffff
#define SERVO0_PRESCALER	30
#define SERVO0_MIN_PULSE	2150
#define SERVO0_MAX_PULSE	7800
#define SERVO0_MIN_ANGLE	-40
#define SERVO0_MAX_ANGLE	36
#define SERVO0_PWM_PORT		GPIOB
#define SERVO0_PWM_PIN		GPIO_PIN_4
#define SERVO0_PWM_AF		GPIO_AF2_TIM3
#define SERVO0_ADC_PORT		GPIOA
#define SERVO0_ADC_PIN		GPIO_PIN_0
#define SERVO0_ADC_CHANNEL	ADC_CHANNEL_0

/* SERVO 1 Configuration */

// PWM D6 PH6 TIM12_CH1
// ADC A1 PF10 ADC3_IN8
// 90 degree @ 5063
#define SERVO1_INST			TIM12
#define SERVO1_PERIOD		0xffff
#define SERVO1_PRESCALER	30
#define SERVO1_MIN_PULSE	4075
#define SERVO1_MAX_PULSE	6200
#define SERVO1_MIN_ANGLE	55
#define SERVO1_MAX_ANGLE	142
#define SERVO1_PWM_PORT		GPIOH
#define SERVO1_PWM_PIN		GPIO_PIN_6
#define SERVO1_PWM_AF		GPIO_AF9_TIM12
#define SERVO1_ADC_PORT		GPIOF
#define SERVO1_ADC_PIN		GPIO_PIN_10
#define SERVO1_ADC_CHANNEL	ADC_CHANNEL_8

/* SERVO 2 Configuration */

// PWM D9 PA15 TIM2_CH1
// ADC A2 PF9 ADC3_IN7
// 0 degree @ 5490
#define SERVO2_INST			TIM2
#define SERVO2_PERIOD		0xffff
#define SERVO2_PRESCALER	30
#define SERVO2_MIN_PULSE	3900
#define SERVO2_MAX_PULSE	6520
#define SERVO2_MIN_ANGLE	-57
#define SERVO2_MAX_ANGLE	20
#define SERVO2_PWM_PORT		GPIOA
#define SERVO2_PWM_PIN		GPIO_PIN_15
#define SERVO2_PWM_AF		GPIO_AF1_TIM2
#define SERVO2_ADC_PORT		GPIOF
#define SERVO2_ADC_PIN		GPIO_PIN_9
#define SERVO2_ADC_CHANNEL	ADC_CHANNEL_7

/* SERVO 3 Configuration */

// PWM D10 PA8 TIM1_CH1
// ADC A3 PF8 ADC3_IN6
#define SERVO3_INST			TIM1
#define SERVO3_PERIOD		0xffff
#define SERVO3_PRESCALER	60
#define SERVO3_MIN_PULSE	1900
#define SERVO3_MAX_PULSE	8500
#define SERVO3_MIN_ANGLE	0
#define SERVO3_MAX_ANGLE	174
#define SERVO3_PWM_PORT		GPIOA
#define SERVO3_PWM_PIN		GPIO_PIN_8
#define SERVO3_PWM_AF		GPIO_AF1_TIM1
#define SERVO3_ADC_PORT		GPIOF
#define SERVO3_ADC_PIN		GPIO_PIN_8
#define SERVO3_ADC_CHANNEL	ADC_CHANNEL_6

typedef struct {
	TIM_TypeDef* instance;
	uint32_t period;
	uint32_t prescaler;
	uint32_t pulse;
} pwm_conf_t;

typedef struct {
	int16_t min_angle_deg;
	int16_t max_angle_deg;
	double min_angle_rad;
	double max_angle_rad;
	uint32_t min_pulse;
	uint32_t max_pulse;
} servo_conf_t;

uint32_t adc_values[SERVOS];
uint32_t adc_ch_conf[SERVOS];

ADC_HandleTypeDef adc;
ADC_ChannelConfTypeDef adc_ch;

pwm_conf_t pwm_conf[SERVOS];
TIM_HandleTypeDef pwm[SERVOS];
TIM_OC_InitTypeDef pwm_oc_init[SERVOS];

servo_conf_t servo_conf[SERVOS];
uint32_t servo_pulse[SERVOS];

coord_polar_t arm_pos_p;
coord_cart_t arm_pos_c;

uint8_t debug;
uint8_t adc_on;
uint8_t pwm_ready;
char lcd_log[100];

osMutexId servo_pulse_mutex;
osMutexId servo_adc_mutex;

void servo_config(void);
void pwm_init(void);
void pwm_set_pulse(uint8_t servo, uint32_t pulse);
void adc_init(void);
void adc_deinit(void);
void adc_measure(void);
void pwm_thread(void const * argument);
void start_adc_thread(void);
void stop_adc_thread(void);
void adc_thread(void const * argument);
double map(double input, double min_in, double max_in, double min_out, double max_out);
void xyz_to_pulse(coord_cart_t* pos_cart);
void pulse_to_ang(angles_t* joint_angles);
void ang_to_xyz(angles_t* joint_angles, coord_cart_t* pos_cart);
void pulse_to_xyz(coord_cart_t* pos_cart);

#endif /* __SERVO_CONTROL_H_ */
