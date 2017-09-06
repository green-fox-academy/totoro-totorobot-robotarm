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
	udp_client_on = 1;

    // Create a new socket to listen for client connections.
    int udp_client_socket;

	udp_client_socket = lwip_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (udp_client_socket < 0) {
		log_msg(ERROR, "Cannot create UDP client socket.\n");
		log_msg(ERROR, "Closing UDP client.\n");
		osThreadTerminate(NULL);
	} else {
		log_msg(USER, "UDP client socket is up.\n");
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
			log_msg(ERROR, "Cannot set UDP client to broadcast.\n");
			log_msg(ERROR, "Closing UDP client.\n");
			lwip_close(udp_client_socket);
			osThreadTerminate(NULL);
		} else {
			log_msg(DEBUG, "UDP client is ready to broadcast.\n");
		}

	udp_client_ready = 1;

    // Construct message
    char send_buffer[255] = SERVICE_ID;

    // Send messages
    while(udp_client_on) {

    	// Send udp package
		uint8_t send_result = sendto(udp_client_socket, send_buffer, strlen(send_buffer),
									 0, (struct sockaddr*) &udp_remote_addr, sizeof(udp_remote_addr));
		log_msg(DEBUG, "UDP packet sent.\n");

	    	osDelay(1500);
    }

    // Close socket
    lwip_close(udp_client_socket);

    // Close thread
    while (1) {
    	udp_client_ready = 0;
    	log_msg(USER, "Closing UDP client.\n");
    	osThreadTerminate(NULL);
    }
}

void udp_syslog_client_thread(void const *argument)
{
	udp_syslog_client_ready = 0;
	udp_syslog_client_on = 1;
	char send_buffer[255];

    // Create a new client socket
	int udp_syslog_client_socket = lwip_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (udp_syslog_client_socket < 0) {
		log_msg(ERROR, "Cannot create UDP syslog client socket.\n");
		log_msg(ERROR, "Closing UDP syslog client.\n");
		osThreadTerminate(NULL);
	} else {
		log_msg(USER, "UDP syslog client socket is up.\n");
	}

	// Create remote server address structure
	struct sockaddr_in udp_remote_addr;
	udp_remote_addr.sin_family = AF_INET;
	udp_remote_addr.sin_addr.s_addr = inet_addr(SYSLOG_SERVER_IP);
	udp_remote_addr.sin_port = htons(SYSLOG_SERVER_PORT);

	udp_syslog_client_ready = 1;

	while(udp_syslog_client_on) {

		// mail queue

		// Construct message
		send_buffer[255] = SERVICE_ID;

		// Send udp package
		sendto(udp_syslog_client_socket, send_buffer, strlen(send_buffer), 0, (struct sockaddr*) &udp_remote_addr, sizeof(udp_remote_addr));
		osDelay(5);

	}

    // Close socket
    lwip_close(udp_syslog_client_socket);

    // Close thread
    while (1) {
    	udp_syslog_client_ready = 0;
    	log_msg(USER, "Closing UDP syslog client.\n");
    	osThreadTerminate(NULL);
    }
}
