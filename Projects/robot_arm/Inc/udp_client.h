/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UDP_CLIENT_H
#define __UDP_CLIENT_H

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdint.h>
#include "cmsis_os.h"
#include "sd_card.h"
#include "udp_server.h"

#define SERVICE_ID	"totorobot"
#define UDP_SERVER_PORT		54002
#define UDP_SYSLOG_FINDER_PORT	54006
#define UDP_SYSLOG_SERVER_PORT	54005

uint8_t udp_client_ready;
uint8_t udp_client_on;
uint8_t udp_syslog_client_ready;
uint8_t udp_syslog_client_on;
uint8_t udp_syslog_server_finder_ready;
uint8_t udp_syslog_server_finder_on;

char syslog_server_ip[20];

void udp_client_thread(void const *argument);
void udp_syslog_client_thread(void const *argument);
void udp_syslog_server_finder_thread(void const *argument);

#endif /* __UDP_CLIENT_H */
