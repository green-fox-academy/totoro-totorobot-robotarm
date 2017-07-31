#ifndef __SERVO_CONTROL_H_
#define __SERVO_CONTROL_H_

#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "lcd_log.h"
#include "robot_arm_conf.h"

#define SERVOS				4

/* SERVO 0 Configuration */

// 4 D3 PB4 TIM3_CH1
#define SERVO0_INST			TIM3
#define SERVO0_PERIOD		0xffff
#define SERVO0_PRESCALER	30
#define SERVO0_MIN_PULSE	2000
#define SERVO0_MAX_PULSE	4000
#define SERVO0_MIN_ANGLE	0
#define SERVO0_MAX_ANGLE	180
#define SERVO0_ADC_CHANNEL	ADC_CHANNEL_0

/* SERVO 1 Configuration */

// 7 D6 PH6 TIM12_CH1
#define SERVO1_INST			TIM12
#define SERVO1_PERIOD		0xffff
#define SERVO1_PRESCALER	30
#define SERVO1_MIN_PULSE	2000
#define SERVO1_MAX_PULSE	4000
#define SERVO1_MIN_ANGLE	0
#define SERVO1_MAX_ANGLE	180
#define SERVO1_ADC_CHANNEL	ADC_CHANNEL_8

/* SERVO 2 Configuration */

// 2 D9 PA15 TIM2_CH1
#define SERVO2_INST			TIM2
#define SERVO2_PERIOD		0xffff
#define SERVO2_PRESCALER	30
#define SERVO2_MIN_PULSE	2000
#define SERVO2_MAX_PULSE	4000
#define SERVO2_MIN_ANGLE	0
#define SERVO2_MAX_ANGLE	180
#define SERVO2_ADC_CHANNEL	ADC_CHANNEL_7

/* SERVO 3 Configuration */

// 3 D10 PA8 TIM1_CH1
#define SERVO3_INST			TIM1
#define SERVO3_PERIOD		0xffff
#define SERVO3_PRESCALER	30
#define SERVO3_MIN_PULSE	2000
#define SERVO3_MAX_PULSE	4000
#define SERVO3_MIN_ANGLE	0
#define SERVO3_MAX_ANGLE	180
#define SERVO3_ADC_CHANNEL	ADC_CHANNEL_6

/* ADC Confoguration */

#define MIN_ADC_VALUE		0
#define MAX_ADC_VALUE		4095

typedef struct {
	TIM_TypeDef* instance;
	uint32_t period;
	uint32_t prescaler;
	uint32_t pulse;
} pwm_conf_t;

typedef struct {
	uint8_t min_angle;
	uint8_t max_angle;
	uint32_t min_pulse;
	uint32_t max_pulse;
	float adc_to_angle_const;
	float angle_to_pulse;
	float adc_to_pulse;
} servo_pos_conf_t;

typedef struct {
	uint32_t x;
	uint32_t y;
	uint32_t z;
} arm_pos_t;

typedef struct {
	uint32_t pulse;
	uint8_t angle;
} servo_pos_t;


uint32_t adc_channels[SERVOS];
ADC_HandleTypeDef adc[SERVOS];
ADC_ChannelConfTypeDef adc_ch[SERVOS];

pwm_conf_t pwm_conf[SERVOS];
TIM_HandleTypeDef pwm[SERVOS];
TIM_OC_InitTypeDef pwm_oc_init[SERVOS];

servo_pos_conf_t servo_pos_conf[SERVOS];
servo_pos_t servo_pos[SERVOS];
arm_pos_t arm_position;
uint8_t debug;
uint8_t adc_ready;
uint8_t pwm_ready;

osMutexId servo_pos_mutex;

void servo_config(void);
void pwm_init(void);
void pwm_set_pulse(uint8_t servo, uint32_t pulse);
void adc_init(void);
uint16_t adc_measure(uint8_t servo);
uint8_t adc_to_angle(uint8_t servo, uint16_t adc_value);
uint32_t angle_to_pulse(uint8_t servo, uint8_t degree);
uint32_t adc_to_pulse(uint8_t servo, uint16_t adc_value);
void adc_thread(void const * argument);
void pwm_thread(void const * argument);

#endif /* __SERVO_CONTROL_H_ */
