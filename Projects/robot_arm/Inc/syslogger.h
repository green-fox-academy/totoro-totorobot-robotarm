#ifndef __SYSLOGGER_H_
#define __SYSLOGGER_H_

#include "stm32f7xx_hal.h"
#include "stm32746g_discovery_ts.h"
#include "lwip/sockets.h"
#include "cmsis_os.h"

#include "lcd_log.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define UDP_SERVER_PORT		54005
#define UDP_REMOTE_PORT		54006
#define SERVICE_ID			"trobotsyslog"

uint8_t udp_client_ready;
uint8_t udp_client_on;

void udp_client_thread(void const *argument);
void udp_server_thread(void const *argument);

#endif /* __SYSLOGGER_H_ */
