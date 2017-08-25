#ifndef __SD_CARD_H_
#define __SD_CARD_H_

#include "stm32f7xx_hal.h"
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_sd.h"
#include "stm32746g_discovery_lcd.h"
#include "cmsis_os.h"
#include "lcd_log.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "kinematics.h"

/* FatFs includes component */
#include "ff_gen_drv.h"
#include "sd_diskio.h"

#define NONE	0
#define ERROR	1
#define USER	2
#define	DEBUG	3

typedef struct {
	char string[100];
	uint8_t log_level;
	// TODO add timestamp
} msg_log_t;

osMailQId(msg_log_q);

uint8_t lcd_log_level;
uint8_t file_log_level;
uint8_t uart_log_level;
uint8_t http_log_level;

uint8_t sd_logger_on;
uint8_t lcd_logger_on;
uint8_t file_reader_on;

static FATFS SDFatFs;  /* File system object for SD card logical drive */
static FRESULT res;    /* FatFs function common result code */
static char sd_path[4]; /* SD card logical drive path */

typedef struct {
	uint16_t g;
	double x;
	double y;
	double z;
	double e;
	double f;
} G_code_t;

extern osMutexId arm_coord_mutex;

extern uint8_t set_position_on;
extern uint8_t next_coord_set;
extern uint8_t end_moving;

extern coord_cart_t target_xyz;
extern char target_display[100];


char msg_log[100];
char logfile_name[100];

uint8_t FAT_fs_init(void);
void write_sd_card(char* file_name, char* line_to_write);
void sd_logger_thread(void const * argument);
void file_reader_thread(void const * argument);
void log_msg(uint8_t log_level, char* message);
uint8_t read_G_code(FIL* file_o, G_code_t* G_code);
uint8_t verify_file(char* file_name);

#endif /* __SD_CARD_H_ */
