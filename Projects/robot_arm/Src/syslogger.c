#include "syslogger.h"

void udp_client_thread(void const *argument)
{
	udp_client_ready = 0;
	udp_client_on = 1;

    // Create a new socket to listen for client connections.
    int udp_client_socket;

	udp_client_socket = lwip_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (udp_client_socket < 0) {
		LCD_ErrLog("LOCAL Cannot create UDP client socket.\n");
		LCD_ErrLog("LOCAL Closing UDP client.\n");
		osThreadTerminate(NULL);
	} else {
		LCD_UsrLog("LOCAL UDP client socket is up.\n");
	}

	// Create remote server address structure
	struct sockaddr_in udp_remote_addr;
	udp_remote_addr.sin_family = AF_INET;
	udp_remote_addr.sin_addr.s_addr = INADDR_BROADCAST;
	udp_remote_addr.sin_port = htons(UDP_REMOTE_PORT);

	 // Set socket to broadcast
	int opt_val = 1;
	int opt_len = sizeof(int);
	int set_broadcast = lwip_setsockopt(udp_client_socket, SOL_SOCKET, SO_BROADCAST, &opt_val, (socklen_t) opt_len);
	if (set_broadcast < 0) {
		LCD_ErrLog("LOCAL Cannot set UDP client to broadcast.\n");
		LCD_ErrLog("LOCAL Closing UDP client.\n");
			lwip_close(udp_client_socket);
			osThreadTerminate(NULL);
		} else {
			LCD_UsrLog("LOCAL UDP client is ready to broadcast.\n");
		}

	udp_client_ready = 1;

    // Construct message
    char send_buffer[255] = SERVICE_ID;

    // Send messages
    while(udp_client_on) {

    	// Send udp package
		sendto(udp_client_socket, send_buffer, strlen(send_buffer), 0, (struct sockaddr*) &udp_remote_addr, sizeof(udp_remote_addr));
		LCD_UsrLog("LOCAL UDP packet sent.\n");

	    osDelay(1000);
    }

    // Close socket
    lwip_close(udp_client_socket);

    // Close thread
    while (1) {
    	udp_client_ready = 0;
    	LCD_ErrLog("LOCAL Closing UDP client.\n");
    	osThreadTerminate(NULL);
    }
}

void udp_server_thread(void const *argument)
{
	// Create 'an UDP' new socket to listen for client connections.
    int udp_server_socket;

	udp_server_socket = lwip_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (udp_server_socket < 0) {
		LCD_ErrLog((char*) "LOCAL Cannot create UDP server socket\n");
		LCD_ErrLog((char*) "LOCAL Closing application\n");
		osThreadTerminate(NULL);
	} else {
		LCD_UsrLog((char*) "LOCAL UDP server socket is up.\n");
	}

	// Create server address structure
	struct sockaddr_in udp_server_addr;
	udp_server_addr.sin_family = AF_INET;
	udp_server_addr.sin_addr.s_addr = INADDR_ANY;
	udp_server_addr.sin_port = htons(UDP_SERVER_PORT);

	// Bind the server address info to socket
	if (lwip_bind(udp_server_socket, (struct sockaddr *)&udp_server_addr, (socklen_t)sizeof(udp_server_addr)) < 0) {
		LCD_ErrLog((char*) "LOCAL UDP server socket bind failed.\n");
		LCD_ErrLog((char*) "LOCAL Closing application\n");
		lwip_close(udp_server_socket);
		osThreadTerminate(NULL);
	} else {
		LCD_UsrLog((char*) "LOCAL UDP server bind successful.\n");
	}

	LCD_UsrLog((char*) "LOCAL UDP server is ready.\n");

    while(1) {

    	osDelay(5);

        struct sockaddr_in udp_client_addr;
        int udp_client_addr_size = sizeof(udp_client_addr);
        char recvbuff[1024];

        int message = recvfrom(udp_server_socket, recvbuff, sizeof(recvbuff),
        			  	  	   0, (struct sockaddr *)&udp_client_addr,
							   (socklen_t *)&udp_client_addr_size);

        recvbuff[message] = 0;	// insert end of string terminator
        udp_client_on = 0;

        printf(recvbuff);

    } // END while

    // Close socket
    lwip_close(udp_server_socket);

    // Close thread
    while (1) {
    	osThreadTerminate(NULL);
    }
}

