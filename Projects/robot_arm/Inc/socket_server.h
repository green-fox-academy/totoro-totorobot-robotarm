/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SOCKET_SERVER_H
#define __SOCKET_SERVER_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#include "sd_card.h"
#include "cmsis_os.h"
#include "app_ethernet.h"
#include "lwip/sockets.h"
#include <string.h>
#include "robot_arm_conf.h"
#include "servo_control.h"

#define SERVER_PORT			54003
#define SERVER_QUEUE_SIZE	2
#define SERVER_BUFF_LEN		8

typedef struct {
	uint8_t command_type;
	uint8_t button_value;
	uint16_t x;
	uint16_t y;
	uint16_t z;
} draw_command_t;

typedef struct {
	char text[100];
} mail_t;

osMailQId  mail_q_id;

uint8_t socket_server_on;


void socket_server_thread(void const *argument);
void deserialize(char* buffer, draw_command_t* command);

extern uint8_t udp_client_on;
//extern double map(double input, double min_in, double max_in, double min_out, double max_out);
//extern void set_position_thread(void const *argument);


#endif /* __SOCKET_SERVER_H */

