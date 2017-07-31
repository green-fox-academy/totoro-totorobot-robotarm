#ifndef __CLIENT_H_
#define __CLIENT_H_

#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "lcd_log.h"
#include "stm32f7xx_hal.h"
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_lcd.h"
#include "lcd_log.h"
#include "cmsis_os.h"
#include "app_ethernet.h"
#include "lwip/sockets.h"

void socket_client_thread(void const *argument);

#endif // __CLIENT_H_
