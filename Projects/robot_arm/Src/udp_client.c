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
	char line[255];

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
	udp_remote_addr.sin_addr.s_addr = inet_addr((char*) argument);
	udp_remote_addr.sin_port = htons(UDP_SYSLOG_SERVER_PORT);

	udp_syslog_client_ready = 1;

	while(udp_syslog_client_on) {

		// Wait for log messages in the MailQueue
		osEvent event = osMailGet(sys_log_q, osWaitForever);

		// Collect incoming mail. ".p" indicates that the message is a pointer
		msg_log_t* received = (msg_log_t*) event.value.p;       //

		// Process the message
		switch (received->log_level) {
		case NONE:
			// Nothing to do
			break;
		case ERROR:
			sprintf(line, "ERROR %s", received->string);
			break;
		case USER:
			sprintf(line, "USER %s", received->string);
			break;
		case DEBUG:
			sprintf(line, "DEBUG %s", received->string);
			break;
		}

		// Free up memory block, so it can be reused
		osMailFree(sys_log_q, received);

		// Send udp package
		sendto(udp_syslog_client_socket, line, strlen(line), 0, (struct sockaddr*) &udp_remote_addr, sizeof(udp_remote_addr));
		osDelay(5);

	}

    // Close socket
	udp_syslog_client_ready = 0;
    lwip_close(udp_syslog_client_socket);

    // Close thread
    while (1) {
    	log_msg(USER, "Closing UDP syslog client.\n");
    	osThreadTerminate(NULL);
    }
}

void udp_syslog_server_finder_thread(void const *argument)
{
	udp_syslog_server_finder_ready = 0;
	udp_syslog_server_finder_on = 1;
	char remote_ip_addr[20];

    // Create a new socket to listen for client connections.
    int udp_server_socket;

	udp_server_socket = lwip_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (udp_server_socket < 0) {
		log_msg(ERROR, "Cannot create UDP server socket\n");
		log_msg(ERROR, "Closing application\n");
		osThreadTerminate(NULL);
	} else {
		log_msg(USER, "UDP server socket is up.\n");
	}

	// Create server address structure
	struct sockaddr_in udp_server_addr;
	udp_server_addr.sin_family = AF_INET;
	udp_server_addr.sin_addr.s_addr = INADDR_ANY;
	udp_server_addr.sin_port = htons(UDP_SYSLOG_FINDER_PORT);

	// Bind the server address info to socket
	if (lwip_bind(udp_server_socket, (struct sockaddr*) &udp_server_addr, (socklen_t) sizeof(udp_server_addr)) < 0) {
		log_msg(ERROR, "UDP server socket bind failed.\n");
		log_msg(ERROR, "Closing application\n");
		lwip_close(udp_server_socket);
		osThreadTerminate(NULL);
	} else {
		log_msg(USER, "UDP server bind successful.\n");
	}

	udp_syslog_server_finder_ready = 1;

	log_msg(USER, "UDP syslog server finder is ready.\n");

	printf("syslog finder server ok\n");

    while(udp_syslog_server_finder_on) {

        struct sockaddr_in udp_client_addr;
        int udp_client_addr_size = sizeof(udp_client_addr);
        char recvbuff[1024];

        printf("in syslog finder while\n");

        int message = recvfrom(udp_server_socket, recvbuff, sizeof(recvbuff),
        			  	  	   0, (struct sockaddr*) &udp_client_addr,
							   (socklen_t*) &udp_client_addr_size);

        recvbuff[message] = 0;	// insert end of string terminator

        printf("UDP recv: %s\n", recvbuff);

        if (strcmp(recvbuff, SYSLOG_ID) == 0) {
        	udp_syslog_server_finder_on = 0;
        	strcpy(remote_ip_addr, inet_ntoa(udp_client_addr.sin_addr));
        	break;
        }
        osDelay(10);

    } // END while

    // Close socket
    lwip_close(udp_server_socket);

    // Start UDP syslog sender
    osThreadDef(SYSLOG_SENDER, udp_syslog_client_thread, osPriorityBelowNormal, 0, configMINIMAL_STACK_SIZE * 3);
    osThreadCreate (osThread(SYSLOG_SENDER), remote_ip_addr);

    // Close thread
    while (1) {
    	osThreadTerminate(NULL);
    }
}
