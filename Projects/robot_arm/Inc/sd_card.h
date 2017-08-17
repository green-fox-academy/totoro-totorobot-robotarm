#ifndef __SD_CARD_H_
#define __SD_CARD_H_

#include "stm32f7xx_hal.h"
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_sd.h"
#include "stm32746g_discovery_lcd.h"

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

void FatFs_Init();
void write_sd_card();
void read_sd_card();
static void Error_Handler(void);
#endif /* __SD_CARD_H_ */
