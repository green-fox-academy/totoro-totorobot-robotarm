/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UDP_CLIENT_H
#define __UDP_CLIENT_H

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdint.h>
#include "cmsis_os.h"
#include "sd_card.h"
#include "udp_server.h"

#define SERVICE_ID	"totorobot 54003"
#define UDP_SERVER_PORT		54002

uint8_t udp_client_ready;
uint8_t udp_client_on;


void udp_client_thread(void const *argument);

#endif /* __UDP_CLIENT_H */
