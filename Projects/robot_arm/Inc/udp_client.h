/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UDP_CLIENT_H
#define __UDP_CLIENT_H

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "cmsis_os.h"
#include "udp_server.h"
#include "pong.h"





int udp_client_ready;
int udp_send_allowed;


void udp_client_thread(void const *argument);
uint8_t serialize_mail(uint8_t* buffer, mail_t* mail_p);
uint8_t serialize_mail_as_string(char* buffer, mail_t* mail_p);

#endif /* __UDP_CLIENT_H */
