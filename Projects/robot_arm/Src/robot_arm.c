/* Includes ------------------------------------------------------------------*/
#include "robot_arm.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

TS_StateTypeDef touch_scr;

int16_t save_x = 0;
int16_t save_y = 0;

uint8_t recieving_arm_feedback[1];

/* Functions -----------------------------------------------------------------*/

void catching_answer(void)
{
	uint8_t catcher = 2;

	while (catcher != '0') {
		osDelay(1);
		if (catcher == '1') {
			closesocket(client_sock);
		} else {
		recv(client_sock, recieving_arm_feedback, strlen((char *)recieving_arm_feedback), 0);
		catcher = recieving_arm_feedback[0];
		}
	}
}

void sending_packet(uint8_t *send_command)
{
	/*
	int sent_bytes = send(client_sock, send_command, strlen((char *)send_command), 0);
	if (sent_bytes > 0)
		LCD_UsrLog("Socket client - data sent\n");
	*/
	send(client_sock, send_command, 8, 0);
}

void socket_client_thread(void const *argument)
{
	printf("Initialised.\n");

	// Creating client socket
	client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_IP);
	if (client_sock < 0) {
		LCD_ErrLog("Socket client - can't create socket\n");
	}

	// Creating server address structure
	struct sockaddr_in addr_in;
	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons( 54003 );
	addr_in.sin_addr.s_addr = inet_addr((char *)client_ip);

	// Connecting the client socket to the server
	int connect_retval = connect(client_sock, (struct sockaddr *)&addr_in, sizeof(addr_in));
	if (connect_retval < 0) {
		LCD_ErrLog("Socket client - can't connect to server\n");
	} else {
		LCD_UsrLog("Socket client - connected to server\n");
	}

	LCD_UsrLog("Socket client - startup...\n");
	LCD_UsrLog("Socket client - waiting for IP address...\n");

	// Try to connect to the server
	while (1) {
		the_drawing_function();
	}

	LCD_UsrLog("Socket client - terminating...\n");

	while (1) {
		osThreadTerminate(NULL);
	}
}

void create_buttons_YGB()
{
	//Create BUTTONS
	BSP_LCD_SetTextColor(LCD_COLOR_DARKYELLOW);
	BSP_LCD_FillRect(396, 80, 70, 50);
	BSP_LCD_DisplayChar(428, 99, 82);
	BSP_LCD_SetTextColor(LCD_COLOR_DARKGREEN);
	BSP_LCD_FillRect(396, 144, 70, 50);
	BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);
	BSP_LCD_FillRect(396, 208, 70, 50);
}

void create_button_R()
{
	//Create BUTTONS
	BSP_LCD_SetTextColor(LCD_COLOR_RED);
	BSP_LCD_FillRect(396, 14, 70, 50);
	BSP_LCD_DisplayChar(428, 33, 83);
}

void drawing_stage(char *sys_opening_scr)
{
	//Set BACKGROUND
	BSP_LCD_Clear(LCD_LOG_BACKGROUND_COLOR);
	//Create BUTTONS YELLOW+GREEN+BLUE
	create_buttons_YGB();
	//Create RED button
	create_button_R();
	//DRAWING area
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DrawRect(20, 30, 356, 230);
	BSP_LCD_DisplayStringAtLine(1, (uint8_t *)sys_opening_scr);

	//Alternate DRAWING area
	/*
	BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
	BSP_LCD_FillCircle(420, 50, 26);
	BSP_LCD_SetTextColor(LCD_COLOR_RED);
	BSP_LCD_FillCircle(390, 135, 40);
	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	BSP_LCD_FillCircle(420, 220, 26);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	//for (int i = 1; i < 10; i++)
		//BSP_LCD_DrawLine(15, i * 28, 265, i * 28);
	for (int j = 1; j < 10; j++)
			BSP_LCD_DrawLine(j + 28, 15, j + 28, 260);
	BSP_LCD_SetTextColor(LCD_COLOR_BROWN);
	*/
}

void red_button_animation(char *sys_stop)
{
	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTMAGENTA);
	BSP_LCD_FillRect(396, 14, 70, 50);
	for (int j = 0; j < 7; j++) {
		BSP_LCD_SetTextColor(LCD_COLOR_RED);
		BSP_LCD_DrawLine(396 + j, 14, 396 + j, 14 + 50);
		BSP_LCD_DrawLine(396 + 70 - j, 14, 396 + 70 - j, 14 + 50);
		BSP_LCD_DrawLine(396, 14 + j, 396 + 70, 14 + j);
		BSP_LCD_DrawLine(396, 14 + 50 - j, 396 + 70, 14 + 50 - j);
		BSP_LCD_DisplayChar(428, 33, 83);
		BSP_LCD_DisplayStringAtLine(1, (uint8_t *)sys_stop);
	}
}

void yellow_button_animation(char *sys_restart)
{
	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTYELLOW);
	BSP_LCD_FillRect(396, 80, 70, 50);
	for (int j = 0; j < 7; j++) {
		BSP_LCD_SetTextColor(LCD_COLOR_DARKYELLOW);
		BSP_LCD_DrawLine(396 + j, 80, 396 + j, 80 + 50);
		BSP_LCD_DrawLine(396 + 70 - j, 80, 396 + 70 - j, 80 + 50);
		BSP_LCD_DrawLine(396, 80 + j, 396 + 70, 80 + j);
		BSP_LCD_DrawLine(396, 80 + 50 - j, 396 + 70, 80 + 50 - j);
		BSP_LCD_DisplayChar(428, 99, 82);
		BSP_LCD_DisplayStringAtLine(1, (uint8_t *)sys_restart);
	}
}

void green_button_animation()
{
	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGREEN);
	BSP_LCD_FillRect(396, 144, 70, 50);
	for (int j = 0; j < 7; j++) {
		BSP_LCD_SetTextColor(LCD_COLOR_DARKGREEN);
		BSP_LCD_DrawLine(396 + j, 144, 396 + j, 144 + 50);
		BSP_LCD_DrawLine(396 + 70 - j, 144, 396 + 70 - j, 144 + 50);
		BSP_LCD_DrawLine(396, 144 + j, 396 + 70, 144 + j);
		BSP_LCD_DrawLine(396, 144 + 50 - j, 396 + 70, 144 + 50 - j);
	}
}

void blue_button_animation()
{
	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);
	BSP_LCD_FillRect(396, 208, 70, 50);
	for (int j = 0; j < 7; j++) {
		BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);
		BSP_LCD_DrawLine(396 + j, 208, 396 + j, 208 + 50);
		BSP_LCD_DrawLine(396 + 70 - j, 208, 396 + 70 - j, 208 + 50);
		BSP_LCD_DrawLine(396, 208 + j, 396 + 70, 208 + j);
		BSP_LCD_DrawLine(396, 208 + 50 - j, 396 + 70, 208 + 50 - j);
	}
}

void circle_delete_animation(coordinate_t last_ts_coord, TS_StateTypeDef ts_state)
{
	if ((20 < ts_state.touchX[0]) && (30 < ts_state.touchY[0]) && (376 > ts_state.touchX[0]) && (260 > ts_state.touchY[0])) {
		BSP_LCD_SetTextColor(LCD_LOG_BACKGROUND_COLOR);
		BSP_LCD_DrawCircle(save_x, save_y, 20);
		BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	}
}

void border_limit_settings(TS_StateTypeDef ts_state)
{
	if (ts_state.touchX[0] < 22 ) {
		ts_state.touchX[0] = 22;
	}
	if (ts_state.touchX[0] > 373 ) {
		ts_state.touchX[0] = 373;
	}
	if (ts_state.touchY[0] < 33 ) {
		ts_state.touchY[0] = 33;
	}
	if (ts_state.touchY[0] > 257 ) {
		ts_state.touchY[0] = 257;
	}
}

void the_drawing_function(void)
{
	LCD_UsrLog("Im in the MousE CoorD THREAD!\n");

	TS_StateTypeDef ts_state;

	coordinate_t last_ts_coord;
	last_ts_coord.x = 0;
	last_ts_coord.y = 0;

	int static drawing_flag = 0;
	int static red_button_flag = 0;

	char sys_opening_scr[] = "                        START DRAWING!";
	char sys_stop[] = "                        SYSTEM STOPPED";
	char sys_restart[] = "                        SYSTEM RESTART";

	uint8_t send_command[8] = {0, 0, 0, 0, 0, 0, 0, 0};

	uint8_t x_Lp;
	uint8_t x_Hp;
	uint8_t y_Lp;
	uint8_t y_Hp;
	int16_t cor_x = 0;
	int16_t cor_y = 0;
	ts_state.touchX[0] = 0;
	ts_state.touchY[0] = 0;

	char coordinates[20];

	// Run the APP infinite loop
	while (1) {

		// Get touch screen state
		BSP_TS_GetState(&ts_state);

		if (BSP_PB_GetState(BUTTON_KEY)) {
			drawing_stage(sys_opening_scr);
		}

		if (ts_state.touchDetected) {
			BSP_LED_On(LED1);

			osDelay(100);

			// Ez a négyzet területe, amiben rajzolni tudunk
			// Dupla védelem - kattintás a területen belül ÉS a flag is legyen 1
			// Vagyis csak akkor hívodik meg, ha volt már kattintás a négyzeten belül
			if ((20 < ts_state.touchX[0]) && (30 < ts_state.touchY[0]) && (376 > ts_state.touchX[0]) && (260 > ts_state.touchY[0]) && drawing_flag) {
				if (!red_button_flag){
				//WHITE circle
				circle_delete_animation(last_ts_coord, ts_state);
				drawing_flag = 0;
				}
			}
			//BLUE button
			else if ((396 < ts_state.touchX[0]) && (208 < ts_state.touchY[0]) && (466 > ts_state.touchX[0]) && (258 > ts_state.touchY[0]) && !red_button_flag) {
				blue_button_animation();
				BSP_LCD_SetTextColor(LCD_COLOR_DARKGREEN);
				BSP_LCD_FillRect(396, 144, 70, 50);
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				send_command[0] = 0;
				//Grip OPEN
				send_command[1] = 3;
				sending_packet(send_command);
			}
			//GREEN button
			else if ((396 < ts_state.touchX[0]) && (144 < ts_state.touchY[0]) && (466 > ts_state.touchX[0]) && (194 > ts_state.touchY[0]) && !red_button_flag) {
				green_button_animation();
				BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);
				BSP_LCD_FillRect(396, 208, 70, 50);
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				send_command[0] = 0;
				//Grip CLOSE
				send_command[1] = 4;
				sending_packet(send_command);
			}
			//YELLOW button
			else if ((396 < ts_state.touchX[0]) && (80 < ts_state.touchY[0]) && (466 > ts_state.touchX[0]) && (130 > ts_state.touchY[0])) {
				yellow_button_animation(sys_restart);
				osDelay(2000);
				drawing_stage(sys_opening_scr);
				red_button_flag = 0;
				send_command[0] = 0;
				//RESET position
				send_command[1] = 2;
				//SET the arm to 0,0,0 position
				send_command[2] = 0;
				send_command[3] = 0;
				send_command[4] = 0;
				send_command[5] = 0;
				send_command[6] = 0;
				send_command[7] = 0;
				sending_packet(send_command);
			}
			//RED button ON
			else if ((396 < ts_state.touchX[0]) && (14 < ts_state.touchY[0]) && (466 > ts_state.touchX[0]) && (64 > ts_state.touchY[0]) && !red_button_flag) {
				red_button_animation(sys_stop);
				//create_buttons_YGB();
				red_button_flag = 1;
				send_command[0] = 0;
				//Emergency STOP
				send_command[1] = 0;
				sending_packet(send_command);
				osDelay(300);
			}
			//RED button OFF
			else if ((396 < ts_state.touchX[0]) && (14 < ts_state.touchY[0]) && (466 > ts_state.touchX[0]) && (64 > ts_state.touchY[0]) && red_button_flag){
				create_button_R();
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				BSP_LCD_DisplayStringAtLine(1, (uint8_t *)sys_opening_scr);
				if ((0 < save_x) && (0 < save_y)) {
					sprintf(coordinates, " X%3d - Y%3d", save_x, abs(save_y - 272));
					BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
					BSP_LCD_DisplayStringAtLine(1, (uint8_t *)coordinates);
				}
				red_button_flag = 0;
				send_command[0] = 0;
				//RESTART any process
				send_command[1] = 1;
				sending_packet(send_command);
				osDelay(300);
			}
			//Ez a terület felel azért, hogy rajzolásnál a pötty ne lógjon ki
			//Valamint itt rajzol a program ÉS itt írja ki a koordinátákat a területen
			else if ((22 < ts_state.touchX[0]) && (33 < ts_state.touchY[0]) && (373 > ts_state.touchX[0]) && (257 > ts_state.touchY[0]) && !red_button_flag) {
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				BSP_LCD_FillCircle(ts_state.touchX[0], ts_state.touchY[0], 4);
				cor_x = ts_state.touchX[0];
				cor_y = ts_state.touchY[0];
				//COORDINATES
				send_command[0] = 1;
				send_command[1] = 0;
				x_Hp = cor_x >> 8;
				x_Lp = (uint8_t)cor_x;
				y_Hp = cor_y >> 8;
				y_Lp = (uint8_t)cor_y;
				send_command[2] = x_Hp;
				send_command[3] = x_Lp;
				send_command[4] = y_Hp;
				send_command[5] = y_Lp;
				sending_packet(send_command);
				sprintf(coordinates, " X%3d - Y%3d", cor_x, cor_y);
				BSP_LCD_DisplayStringAtLine(1, (uint8_t *)coordinates);
				sprintf(coordinates, " XHP%3d - XLP%3d - YHP%3d - YLP%3d", x_Hp, x_Lp, y_Hp, y_Lp);
				BSP_LCD_DisplayStringAtLine(2, (uint8_t *)coordinates);
				sprintf(coordinates, " XHP%3d - XLP%3d - YHP%3d - YLP%3d", send_command[2], send_command[3], send_command[4], send_command[5]);
				BSP_LCD_DisplayStringAtLine(3, (uint8_t *)coordinates);
			}
		} else {
			BSP_LED_Off(LED1);
			//Dupla védelem arra, hogy ez a rész csak akkor hívódjon meg
			//HA már volt érintés a négyzeten belül
			if ((ts_state.touchX[0] > 0) && (ts_state.touchY[0] > 0) && !drawing_flag) {
				if ((20 < ts_state.touchX[0]) && (30 < ts_state.touchY[0]) && (376 > ts_state.touchX[0]) && (260 > ts_state.touchY[0]) && !red_button_flag) {
					osDelay(500);
					BSP_LCD_SetTextColor(LCD_COLOR_RED);
					BSP_LCD_DrawCircle(ts_state.touchX[0], ts_state.touchY[0], 20);
					//Vár amíg nem jön jelzés a robottól, hogy mehet tovább
					//catching_answer();
					osDelay(1000);
					BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
					BSP_LCD_DrawCircle(ts_state.touchX[0], ts_state.touchY[0], 20);
					BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
					save_x = ts_state.touchX[0];
					save_y = ts_state.touchY[0];
					drawing_flag = 1;
				}
			}
		}
	}
}

void string_splitter(void)
{
	char str[] = "This a sample string1.This a sample string2.This a sample string3.\n";
	char buff[3][50];
	char * pch;

	pch = strtok(str, ".");
	uint8_t i = 0;

	while (pch != NULL) {
		strcpy(buff[i], pch);
		pch = strtok(NULL, ".");
		i++;
	}

	for (i = 0; i < 3; i++){
	        printf ("%s\n", buff[i]);
	        pch = strtok(buff[i], " ");
	        while (pch != NULL) {
	            printf("%s\n", pch);
	            pch = strtok(NULL, " ");
	        }
	}
}

void touch_screen_test_thread(void const * argument)
{
	string_splitter();

	do {
		BSP_TS_GetState(&touch_scr);
		if (touch_scr.touchDetected) {
			BSP_LED_On(LED1);
			BSP_LCD_FillCircle(touch_scr.touchX[0], touch_scr.touchY[0], 6);
		} else
			BSP_LED_Off(LED1);
	} while (1 > 0);
}

void udp_server_thread(void const *argument)
{
	// Create 'an UDP' new socket to listen for client connections.
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
	if (lwip_bind(udp_server_socket, (struct sockaddr *)&udp_server_addr, (socklen_t)sizeof(udp_server_addr)) < 0) {
		LCD_ErrLog((char*) "UDP server socket bind failed.\n");
		LCD_ErrLog((char*) "Closing application\n");
		lwip_close(udp_server_socket);
		osThreadTerminate(NULL);
	} else {
		LCD_UsrLog((char*) "UDP server bind successful.\n");
	}

	LCD_UsrLog((char*) "UDP server is ready.\n");

    while(1) {

    	osDelay(15);

        struct sockaddr_in udp_client_addr;
        int udp_client_addr_size = sizeof(udp_client_addr);
        char recvbuff[1024];

        int message = recvfrom(udp_server_socket, recvbuff, sizeof(recvbuff),
        			  	  	   0, (struct sockaddr *)&udp_client_addr,
							   (socklen_t *)&udp_client_addr_size);

        recvbuff[message] = 0;	// insert end of string terminator

        client_ip = inet_ntoa(udp_client_addr.sin_addr);

        if (strcmp(recvbuff, "totorobot") == 0) {
        	osThreadDef(CLIENT, socket_client_thread, osPriorityBelowNormal, 0, configMINIMAL_STACK_SIZE * 2);
        	osThreadCreate (osThread(CLIENT), NULL);
        	osDelay(3000);
        }

        printf("UDP received from %s %d %s\n", inet_ntoa(udp_client_addr.sin_addr), udp_client_addr.sin_port, recvbuff);

    } // END while

    // Close socket
    lwip_close(udp_server_socket);

    // Close thread
    while (1) {
    	osThreadTerminate(NULL);
    }
}

void socket_server_thread(void const *argument)
{
	// Server address structure initialization
	struct sockaddr_in addr_in;                                 // Inet address structure definition
	addr_in.sin_family = AF_INET;                               // This address is an internet address
	addr_in.sin_port = htons (54545);                      		// Server port
	addr_in.sin_addr.s_addr = INADDR_ANY;             			// Server IP
	struct sockaddr *addr = (struct sockaddr *)&addr_in;        // Make a struct sockaddr pointer, which points to the address stucture

	// Creating the socket
	int master_sock = socket(AF_INET, SOCK_STREAM, 0);
	// Check if socket is ok
	if (master_sock < 0)
		LCD_ErrLog("socket() ");

	// Start binding the socket to the previously set address
	int flag = bind(master_sock, addr, sizeof(*addr));
	// Check if the binding is ok
	if (flag < 0)
		LCD_ErrLog("bind() ");

	// Start listening on with the set socket with a predefined queue size
	flag = listen(master_sock, 100);
	// Check is listening is ok
	if (flag < 0)
		LCD_ErrLog("listen() ");

	// Client address structure
	struct sockaddr client_addr;
	// Slave socket definition, this will be used to store the incoming socket
	int slave_sock;
	// Buffer for incoming and outgoing data
	char recv_buff[100];
	// Buffer(s) for feedback
	char send_buff[] = "Yeahh, I got it..";
	char position[10];

	while (1) {
		// Accept the connection and save the incoming socket
		slave_sock = accept(master_sock, &client_addr, NULL);

		// Check if the socket is valid
		if (slave_sock < 0) {
			LCD_ErrLog("accept()");
		} else {
			LCD_UsrLog("connection accepted\n");
		}

		// Receive the data sent by the client
		int received_bytes;
		do {
			//received_bytes = recv(slave_sock, recv_buff, 100, 0);
			received_bytes = recv(slave_sock, &touch_scr, sizeof(TS_StateTypeDef), 0);
			if (received_bytes > 0) {
				//recv_buff[received_bytes] = '\0';
				//printf("Received string: %s \n", recv_buff);
				// Send back the received string
				//send(slave_sock, send_buff, sizeof(send_buff), 0);
				int16_t cor_x = touch_scr.touchX[0];
				int16_t cor_y = abs(touch_scr.touchY[0] - 272);
				sprintf(position,"%3d - %3d", cor_x, cor_y);
				BSP_LCD_DisplayStringAtLine(1, (uint8_t *)position);
			} else if (received_bytes < 0) {
				LCD_UsrLog("Something went wrong with the client socket, trying to close it...\n");
				break;
			}
		} while (received_bytes > 0);
		closesocket(slave_sock);
		LCD_UsrLog("client socket closed\n\n");
	}
	// Cleaning up used memory
	LCD_UsrLog("Closing server socket\n");
	closesocket(master_sock);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
