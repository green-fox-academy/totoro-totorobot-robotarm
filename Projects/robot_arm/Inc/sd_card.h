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
static FIL MyFile;     /* File object */
static char sd_path[4]; /* SD card logical drive path */
static FRESULT res;    /* FatFs function common result code */
static uint32_t bytesread;      				            /* File write/read counts */
static char wtext[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n"; /* File write buffer */
static char btext[] = "STM333.TXT";						/* Name of the file */
static char rtext[100];


typedef struct {
	uint16_t g;
	double x;
	double y;
	double z;
	double e;
	double f;
} G_code_t;


char msg_log[100];
char logfile_name[100];

uint8_t FAT_fs_init(void);
void write_sd_card(char* file_name, char* line_to_write);
void read_sd_card(char* file_name);
void sd_logger_thread(void const * argument);
void file_reader_thread(void const * argument);
void log_msg(uint8_t log_level, char* message);
void read_G_code(char* file_name, uint32_t* read_pos, G_code_t* G_code);
uint8_t verify_file(char* file_name);

#endif /* __SD_CARD_H_ */
