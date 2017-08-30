/* Includes ------------------------------------------------------------------*/
#include "udp_client.h"
#include "stm32746g_discovery_ts.h"
#include "lcd_log.h"
#include "cmsis_os.h"
#include "app_ethernet.h"
#include "lwip/sockets.h"
#include "stm32746g_discovery_ts.h"
#include <string.h>

void udp_client_thread(void const *argument)
{
	udp_client_ready = 0;
	udp_send_allowed = 1;

    // Create a new socket to listen for client connections.
    int udp_client_socket;

	udp_client_socket = lwip_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (udp_client_socket < 0) {
		LCD_ErrLog((char*) "Cannot create UDP client socket.\n");
		LCD_ErrLog((char*) "Closing application.\n");
		osThreadTerminate(NULL);
	} else {
		LCD_UsrLog((char*) "UDP client socket is up.\n");
	}

	// Create client address structure
	struct sockaddr_in udp_client_addr;
	udp_client_addr.sin_family = AF_INET;

	// Create remote server address structure
	struct sockaddr_in udp_remote_addr;
	udp_remote_addr.sin_family = AF_INET;
	udp_remote_addr.sin_addr.s_addr = INADDR_BROADCAST;
	udp_remote_addr.sin_port = htons(UDP_SERVER_PORT);

	 // Set socket to broadcast
	int opt_val = 1;
	int opt_len = sizeof(int);
	int set_broadcast = lwip_setsockopt(udp_client_socket, SOL_SOCKET,
									    SO_BROADCAST, &opt_val, (socklen_t) opt_len);
	if (set_broadcast < 0) {
			LCD_ErrLog((char*) "Cannot set UDP client to broadcast.\n");
			LCD_ErrLog((char*) "Closing application.\n");
			lwip_close(udp_client_socket);
			osThreadTerminate(NULL);
		} else {
			LCD_UsrLog((char*) "UDP client is ready to broadcast.\n");
		}

	udp_client_ready = 1;

	LCD_UsrLog((char*) "UDP client is ready.\n");

    // Construct message
    char send_buffer[255] = SERVICE_ID;

    // Send messages
    while(1) {

    	osDelay(15);

    	// Wait for mail
    	osEvent evt = osMailGet(msg_q_id, 15);
    	if (evt.status == osEventMail) {

    		// Unpack and serialize struct in mail
    		mail_t* mail_p = (mail_t*) evt.value.p;
			uint8_t mail_size = serialize_mail_as_string(send_buffer, mail_p);

			// Send udp package
			uint8_t send_result = sendto(udp_client_socket, send_buffer, mail_size,
			    				         0, (struct sockaddr*) &udp_remote_addr, sizeof(udp_remote_addr));

			// Free up memory used by mail struct
			osMailFree(msg_q_id, mail_p);

    		// Do error checking if necessary

			/* Uncomment for debug
			char temp[100];
			sprintf(temp, "%d:%s", mail_size, send_buffer);
			BSP_LCD_ClearStringLine(3);
			BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
			BSP_LCD_DisplayStringAtLine(3, (uint8_t*) temp);
			*/
    	}
    }

    // Close socket
    lwip_close(udp_client_socket);

    // Close thread
    while (1) {
    	osThreadTerminate(NULL);
    }
}

uint8_t serialize_mail(uint8_t* buffer, mail_t* mail_p)
{
	// Buffer already has SERVICE_ID in its first SERVICE_ID_SIZE bytes
	uint8_t i = SERVICE_ID_SIZE;
	uint8_t buffer_size;

	buffer[i++] = (uint8_t) mail_p->data_type;
	buffer[i++] = (uint8_t) mail_p->action;

	switch (mail_p->data_type) {
	case 'B':
		buffer[i++] = mail_p->ball_data.id;
		buffer[i++] = (uint8_t) (mail_p->ball_data.x >> 8);
		buffer[i++] = (uint8_t) (mail_p->ball_data.x);
		buffer[i++] = (uint8_t) (mail_p->ball_data.y >> 8);
		buffer[i++] = (uint8_t) (mail_p->ball_data.y);
		buffer[i++] = (uint8_t) (mail_p->ball_data.prev_x >> 8);
		buffer[i++] = (uint8_t) (mail_p->ball_data.prev_x);
		buffer[i++] = (uint8_t) (mail_p->ball_data.prev_y >> 8);
		buffer[i++] = (uint8_t) (mail_p->ball_data.prev_y);
		buffer[i++] = (uint8_t) mail_p->ball_data.speed_x;
		buffer[i++] = (uint8_t) mail_p->ball_data.speed_y;
		buffer[i++] = mail_p->ball_data.radius;
		buffer[i++] = (uint8_t) (mail_p->ball_data.color >> 16);
		buffer[i++] = (uint8_t) (mail_p->ball_data.color >> 8);
		buffer[i++] = (uint8_t) (mail_p->ball_data.color);
		buffer[i++] = (uint8_t) '#';

		break;
	case 'U':
		buffer[i++] = mail_p->user_data.id;
		buffer[i++] = (uint8_t) (mail_p->user_data.points >> 8);
		buffer[i++] = (uint8_t) (mail_p->user_data.points);
		for (uint8_t j = 0; j < mail_p->user_data.name_length; j++) {
			i += j;
			buffer[i] = (uint8_t) mail_p->user_data.name[j];
		}
		buffer[i++] = (uint8_t) '#';
		break;
	}

	buffer_size = i;

	return buffer_size;
}


uint8_t serialize_mail_as_string(char* buffer, mail_t* mail_p)
{
	// Buffer already has SERVICE_ID in its first SERVICE_ID_SIZE bytes
	uint8_t i = SERVICE_ID_SIZE;
	uint8_t buffer_size;

	buffer[i++] = ' ';
	buffer[i++] = mail_p->data_type;
	buffer[i++] = ' ';
	buffer[i++] = mail_p->action;
	buffer[i] = 0;

	char temp[100];

	switch (mail_p->data_type) {
	case 'B':
		sprintf(temp, " %02d %03d %03d %03d %03d %03d %03d %03d %x", mail_p->ball_data.id, mail_p->ball_data.x,
				mail_p->ball_data.y, mail_p->ball_data.prev_x, mail_p->ball_data.prev_y,
				mail_p->ball_data.speed_x, mail_p->ball_data.speed_y, mail_p->ball_data.radius, (uint) mail_p->ball_data.color >> 8);
		break;
	case 'U':
		sprintf(temp, " %02d %05d %03d %s", mail_p->user_data.id, mail_p->user_data.points,
				mail_p->user_data.name_length, mail_p->user_data.name);
		break;
	}

	strcat(buffer, temp);
	buffer_size = strlen(buffer);

	return buffer_size;
}
