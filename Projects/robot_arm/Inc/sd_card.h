#ifndef __SD_CARD_H_
#define __SD_CARD_H_

#include "stm32f7xx_hal.h"
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_sd.h"
#include "stm32746g_discovery_lcd.h"
#include "cmsis_os.h"

/* FatFs includes component */
#include "ff_gen_drv.h"
#include "sd_diskio.h"

typedef struct {
	uint8_t G_code;
	double x_param;
	double y_param;
	double e_param;
} G_code_struct_t;

G_code_struct_t raw_G_code;

osMutexId servo_ready_mutex;
uint8_t servo_pos_ready;

void FatFs_Init();
void write_sd_card();
void read_G_code();
void G_read_thread(void const * argument);
void test_timer_thread(void const * argument);

static void Error_Handler(void);
#endif /* __SD_CARD_H_ */
