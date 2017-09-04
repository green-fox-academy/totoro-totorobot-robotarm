#include "udp_server.h"

void udp_server_thread(void const *argument)
{
	udp_server_ready = 0;

    // Create a new socket to listen for client connections.
    int udp_server_socket;

	udp_server_socket = lwip_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (udp_server_socket < 0) {
		LCD_ErrLog((char*) "Cannot create UDP server socket\n");
		LCD_ErrLog((char*) "Closing application\n");
		osThreadTerminate(NULL);
	} else {
		LCD_UsrLog((char*) "UDP server socket is up.\n");
	}

	// Create server address structure
	struct sockaddr_in udp_server_addr;
	udp_server_addr.sin_family = AF_INET;
	udp_server_addr.sin_addr.s_addr = INADDR_ANY;
	udp_server_addr.sin_port = htons(UDP_SERVER_PORT);

	// Bind the server address info to socket
	if (lwip_bind(udp_server_socket, (struct sockaddr*) &udp_server_addr,
			      (socklen_t) sizeof(udp_server_addr)) < 0) {
		LCD_ErrLog((char*) "UDP server socket bind failed.\n");
		LCD_ErrLog((char*) "Closing application\n");
		lwip_close(udp_server_socket);
		osThreadTerminate(NULL);
	} else {
		LCD_UsrLog((char*) "UDP server bind successful.\n");
	}

	udp_server_ready = 1;

	LCD_UsrLog((char*) "UDP server is ready.\n");



    while(1) {

    	osDelay(15);

        struct sockaddr_in udp_client_addr;
        int udp_client_addr_size = sizeof(udp_client_addr);
        char recvbuff[1024];

        int message = recvfrom(udp_server_socket, recvbuff, sizeof(recvbuff),
        			  	  	   0, (struct sockaddr*) &udp_client_addr,
							   (socklen_t*) &udp_client_addr_size);

        recvbuff[message] = 0;	// insert end of string terminator

        printf("UDP received from %s %d %s\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port, recvbuff);
        

    } // END while

    // Close socket
    lwip_close(udp_server_socket);

    // Close thread
    while (1) {
    	osThreadTerminate(NULL);
    }
}
