#include "socket_server.h"

void socket_server_thread(void const *argument)
{
	// Define params
	int server_socket;
	struct sockaddr_in server_addr;

	// Create server socket
	server_socket = lwip_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket < 0) {
		log_msg(ERROR, "Cannot create socket.\n");
		osThreadTerminate(NULL);
	}

	// Create sockaddr_in structure
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_len = sizeof(server_addr);

	// Bind socket to port
	int bind_result = lwip_bind(server_socket, (struct sockaddr*) &server_addr, sizeof(server_addr));
	if (bind_result < 0) {
		log_msg(ERROR, "Cannot bind socket.\n");
		lwip_close(server_socket);
		osThreadTerminate(NULL);
	}

	// Listen for incoming connections
	if (lwip_listen(server_socket, SERVER_QUEUE_SIZE) != 0 ) {
		log_msg(ERROR, "Server listen failed.\n");
		lwip_close(server_socket);
		osThreadTerminate(NULL);
	} else

	// Accept incoming connection -> client socket
	while (1) {

		// Define client params
		int client_socket;
		struct sockaddr_in client_addr;
        int client_addr_len = sizeof(client_addr);
	    char buffer[SERVER_BUFF_LEN];
	    int received_bytes;

	    // Let other threads to get some processor time too
	    osDelay(10);

	    // Accept connection
	    client_socket = lwip_accept(server_socket, (struct sockaddr*) &client_addr, (socklen_t*) &client_addr_len);

	    // Receive message
	    if (client_socket > 0) {
	   // 	do {
	    		received_bytes = lwip_recv(client_socket, buffer, sizeof(buffer),0);
	    		buffer[received_bytes] = ' ';
	    		buffer[received_bytes + 1] = '\0';
	    //	} while (received_bytes > 0);

	    	// Close client connection
	    	lwip_close(client_socket);

	        // Send buffer content to another thread via mail queue
	        mail_t* mail;
	        mail = (mail_t*) osMailAlloc(mail_q_id, osWaitForever);
	        strcpy(mail->text, buffer);
	        osMailPut(mail_q_id, mail);
	    }
	}

	// Close server socket
	while(1) {
		lwip_close(server_socket);
		log_msg(USER, "TCP server socket thread terminating.\n");
		osThreadTerminate(NULL);
	}


}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
