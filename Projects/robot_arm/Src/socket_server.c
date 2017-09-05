#include "socket_server.h"

void socket_server_thread(void const *argument)
{
	// Define params
	int server_socket;
	struct sockaddr_in server_addr;
	socket_server_on = 1;

	// Send arm to starting position
	while(1) {
		osMutexWait(arm_coord_mutex, osWaitForever);
		if (!next_coord_set) {

			// Set xyz coordinates
			target_xyz.x = (double) DRAW_X_ZERO_CALC;
			target_xyz.y = (double) DRAW_Y_ZERO_CALC;
			target_xyz.z = (double) DRAW_Z_ZERO_CALC;

			// Set display message
			sprintf(target_display, "%3d  %3d  %3d   ", DRAW_X_ZERO_CALC, DRAW_Y_ZERO_CALC, DRAW_Z_ZERO_CALC);

			next_coord_set = 1;
			osMutexRelease(arm_coord_mutex);
			break;
		}
		osMutexRelease(arm_coord_mutex);
		osDelay(10);
	}

	// Set flag to one movement only
	end_moving = 1;

	log_msg(DEBUG, "Moving arm to drawing position.\n");

	// Launch process to set pulse
	osThreadDef(SET_POSITION, set_position_thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 10);
	osThreadCreate (osThread(SET_POSITION), NULL);

	log_msg(USER, "TCP Server thread started.\n");

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
	} else {
		log_msg(DEBUG, "TCP server is listening.\n");
	}

	// Accept incoming connection -> client socket
	while (socket_server_on) {

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

	    log_msg(DEBUG, "TCP server: connection accepted.\n");

	    // Turn off UDP client
	    udp_client_on = 0;


	    // Change draw button color to green
	    buttons[2].btn_color1 = LCD_COLOR_GREEN;


	    // Keep receiving messages
	    if ((client_socket > 0) && socket_server_on) {
			do {
				draw_command_t command;

				// Receive a command
				memset(buffer, 0, SERVER_BUFF_LEN);


				received_bytes = lwip_recv(client_socket, buffer, SERVER_BUFF_LEN, 0);

				if (!socket_server_on) {
					break;
				}


				if (received_bytes == 0) {
					osDelay(5);
				} else {

					char tmp[100];
					sprintf(tmp, "r:%d, b0:%d, b1:%d, b2:%d, b3:%d, b4:%d, b5:%d, b6:%d, b7:%d\n", received_bytes, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7]);
					log_msg(DEBUG, tmp);


					// Deserialize buffer into command structure
					deserialize(buffer, &command);

					sprintf(tmp, "t:%d, b:%d, x:%d, y:%d, z:%d\n", command.command_type, command.button_value, command.x, command.y, command.z);
					log_msg(DEBUG, tmp);

					/*
					 *  Interpret command
					 */

					// Button press sent
					if (command.command_type == 0) {
						if (command.button_value == 0) {

							// Stop action
							break;

						} else if (command.button_value == 2) {

							// Send robot to start position

						}

					// Coordinate sent
					} else {

						// Calculate drawing coordinates, rotate by 90 degrees
						int16_t x  = (int16_t) map((double) command.x, (double) DRAW_X_ZERO_RECV, (double) DRAW_X_MAX_RECV,
								                   (double) DRAW_Y_ZERO_CALC, (double) DRAW_Y_MAX_CALC);
						int16_t y  = (int16_t) map((double) command.y, (double) DRAW_Y_ZERO_RECV, (double) DRAW_Y_MAX_RECV,
												   (double) DRAW_X_ZERO_CALC, (double) DRAW_X_MAX_CALC);

						int16_t z = DRAW_Z_ZERO_CALC;
					}

					// After command ran, send acknowledge byte
					lwip_send(client_socket, "0", 1, 0);
					log_msg(DEBUG, "TCP ack sent.\n");
				}

			} while ((received_bytes > 0) && socket_server_on);

		// Send close connection byte
		lwip_send(client_socket, "1", 1, 0);
		log_msg(DEBUG, "TCP close byte sent.\n");

		// Close client connection
		lwip_close(client_socket);

		// Change button color to orange
		buttons[2].btn_color1 = LCD_COLOR_ORANGE;

		// Send buffer content to another thread via mail queue
//		mail_t* mail;
//		mail = (mail_t*) osMailAlloc(mail_q_id, osWaitForever);
//		strcpy(mail->text, buffer);
//		osMailPut(mail_q_id, mail);
	    }
	}

	// Close server socket
	while(1) {
		lwip_close(server_socket);
		log_msg(USER, "TCP server socket thread terminating.\n");
		osThreadTerminate(NULL);
	}
}

void deserialize(char* buffer, draw_command_t* command)
{
	command->command_type = buffer[0];
	command->button_value = buffer[1];
	command->x = buffer[2] << 8 | buffer[3];
	command->y = buffer[4] << 8 | buffer[5];
	command->z = buffer[6] << 8 | buffer[7];

	return;
}
