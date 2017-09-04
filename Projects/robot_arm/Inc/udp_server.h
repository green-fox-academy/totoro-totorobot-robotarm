/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UDP_SERVER_H
#define __UDP_SERVER_H

#include <string.h>
#include <stdint.h>
#include "lcd_log.h"
#include "cmsis_os.h"
#include "app_ethernet.h"
#include "lwip/sockets.h"
#include "stm32746g_discovery_ts.h"

#define UDP_SERVER_PORT		54002

int udp_server_ready;
void udp_server_thread(void const *argument);

#endif /* __UDP_SERVER_H */
