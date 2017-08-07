#ifndef __CLIENT_H_
#define __CLIENT_H_

#include <string.h>
#include <stdint.h>
#include <math.h>
#include "cmsis_os.h"


int udp_client_ready;
int udp_send_allowed;

time_t txTm;

void udp_client_thread(void const *argument);

#endif // __CLIENT_H_
