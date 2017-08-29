/* Includes ------------------------------------------------------------------*/
#include "robot_arm.h"
#include "lwip/sockets.h"
#include "stm32746g_discovery_ts.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
typedef struct {
    int32_t x;
    int32_t y;
} coordinate_t;

TS_StateTypeDef touch_scr;

int16_t save_x;
int16_t save_y;

int connect_to_server(int *client_sock, uint16_t SERVER_PORT, char *CLIENT_SERVER_IP)
{
	// Creating client socket
	(*client_sock) = socket(PF_INET, SOCK_STREAM, IPPROTO_IP);
	if (*client_sock < 0) {
		LCD_ErrLog("Socket client - can't create socket\n");
		return -1;
	}

	// Creating server address structure
	struct sockaddr_in addr_in;
	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons(SERVER_PORT);
	addr_in.sin_addr.s_addr = inet_addr(CLIENT_SERVER_IP);

	// Connecting the client socket to the server
	int connect_retval = connect(*client_sock, (struct sockaddr *)&addr_in, sizeof(addr_in));
	if (connect_retval < 0) {
		LCD_ErrLog("Socket client - can't connect to server\n");
		return -1;
	} else {
		LCD_UsrLog("Socket client - connected to server\n");
		return 0;
	}
}

void system_stop_animation()
{
	//Create BUTTONS
	BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);
	BSP_LCD_FillRect(396, 208, 70, 50);
	BSP_LCD_SetTextColor(LCD_COLOR_DARKGREEN);
	BSP_LCD_FillRect(396, 144, 70, 50);
	BSP_LCD_SetTextColor(LCD_COLOR_DARKYELLOW);
	BSP_LCD_FillRect(396, 80, 70, 50);
	BSP_LCD_DisplayChar(428, 99, 82);
}

void drawing_stage(char *sys_opening_scr)
{
	//Set BACKGROUND
	BSP_LCD_Clear(LCD_LOG_BACKGROUND_COLOR);
	//Create BUTTONS
	BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);
	BSP_LCD_FillRect(396, 208, 70, 50);
	BSP_LCD_SetTextColor(LCD_COLOR_DARKGREEN);
	BSP_LCD_FillRect(396, 144, 70, 50);
	BSP_LCD_SetTextColor(LCD_COLOR_DARKYELLOW);
	BSP_LCD_FillRect(396, 80, 70, 50);
	BSP_LCD_DisplayChar(428, 99, 82);
	BSP_LCD_SetTextColor(LCD_COLOR_RED);
	BSP_LCD_FillRect(396, 14, 70, 50);
	BSP_LCD_DisplayChar(428, 33, 83);
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

void red_button_animation()
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
	}
}

void yellow_button_animation()
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

void circle_delete_animation(coordinate_t last_ts_coord, TS_StateTypeDef ts_state)
{
	if ((20 < ts_state.touchX[0]) && (30 < ts_state.touchY[0]) && (376 > ts_state.touchX[0]) && (260 > ts_state.touchY[0])) {
		BSP_LCD_SetTextColor(LCD_LOG_BACKGROUND_COLOR);
		BSP_LCD_DrawCircle(save_x, save_y, 20);
		BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	}
}

void mouse_coordinate_thread(void const * argument)
{
		LCD_UsrLog("Im in the MousE CoorD THREAD!\n");

		TS_StateTypeDef ts_state;

		coordinate_t last_ts_coord;
		last_ts_coord.x = 0;
		last_ts_coord.y = 0;

		coordinate_t first_ts_coord;
		first_ts_coord.x = 0;
		first_ts_coord.y = 0;

		uint8_t first_touch_detected_flag = 0;
		uint8_t possible_click_event = 0;

		int drawing_flag = 0;
		int red_button_flag = 0;

		//Ez azért szükséges, mert ha nem a történik érintés a négyzeten belül, fagy
		char sys_opening_scr[] = "                        START DRAWING!";
		char sys_stop[] = "                        SYSTEM STOPPED";
		char sys_restart[] = "                        SYSTEM RESTART";

		int16_t cor_x = 0;
		int16_t cor_y = 0;
		ts_state.touchX[0] = 0;
		ts_state.touchY[0] = 0;

		char coordinates[100];

		//LCD_UsrLog("X:%d - Y:%d", ts_state.touchX[0], ts_state.touchY[0]);

		/* Run Application (Interrupt mode) */
		while (1) {
			// Get touch screen state
			BSP_TS_GetState(&ts_state);
			// Reset USB HID buffer
			//HID_Buffer[0] = 0;
			HID_Buffer[1] = 0;
			HID_Buffer[2] = 0;

			osDelay(10);

			if (BSP_PB_GetState(BUTTON_KEY)) {
				drawing_stage(sys_opening_scr);
			}

			if (ts_state.touchDetected) {
				BSP_LED_On(LED1);
				if ((ts_state.touchX[0] > 0) && (ts_state.touchY[0] > 0) && drawing_flag) {
					if ((20 < ts_state.touchX[0]) && (30 < ts_state.touchY[0]) && (376 > ts_state.touchX[0]) && (260 > ts_state.touchY[0])) {
						//WHITE circle
						circle_delete_animation(last_ts_coord, ts_state);
					}
					drawing_flag = 0;
				}
				if ((20 < ts_state.touchX[0]) && (30 < ts_state.touchY[0]) && (376 > ts_state.touchX[0]) && (260 > ts_state.touchY[0]) && !red_button_flag) {
					BSP_LCD_FillCircle(ts_state.touchX[0], ts_state.touchY[0], 4);
				}
				//BLUE button
				if ((396 < ts_state.touchX[0]) && (208 < ts_state.touchY[0]) && (466 > ts_state.touchX[0]) && (258 > ts_state.touchY[0]) && !red_button_flag) {
					blue_button_animation();
					circle_delete_animation(last_ts_coord, ts_state);
					BSP_LCD_SetTextColor(LCD_COLOR_DARKGREEN);
					BSP_LCD_FillRect(396, 144, 70, 50);
					BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				}
				//GREEN button
				if ((396 < ts_state.touchX[0]) && (144 < ts_state.touchY[0]) && (466 > ts_state.touchX[0]) && (194 > ts_state.touchY[0]) && !red_button_flag) {
					green_button_animation();
					circle_delete_animation(last_ts_coord, ts_state);
					BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);
					BSP_LCD_FillRect(396, 208, 70, 50);
					BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				}
				//YELLOW button
				if ((396 < ts_state.touchX[0]) && (80 < ts_state.touchY[0]) && (466 > ts_state.touchX[0]) && (130 > ts_state.touchY[0])) {
					yellow_button_animation();
					circle_delete_animation(last_ts_coord, ts_state);
					BSP_LCD_DisplayStringAtLine(1, (uint8_t *)sys_restart);
					osDelay(2000);
					drawing_stage(sys_opening_scr);
					red_button_flag = 0;
				}
				//RED button ON
				if ((396 < ts_state.touchX[0]) && (14 < ts_state.touchY[0]) && (466 > ts_state.touchX[0]) && (64 > ts_state.touchY[0]) && !red_button_flag) {
					circle_delete_animation(last_ts_coord, ts_state);
					system_stop_animation();
					red_button_animation();
					BSP_LCD_DisplayStringAtLine(1, (uint8_t *)sys_stop);
					red_button_flag = 1;
					osDelay(100);
				}
				//RED button OFF
				else if ((396 < ts_state.touchX[0]) && (14 < ts_state.touchY[0]) && (466 > ts_state.touchX[0]) && (64 > ts_state.touchY[0]) && red_button_flag){
					circle_delete_animation(last_ts_coord, ts_state);
					//system_stop_animation();
					BSP_LCD_SetTextColor(LCD_COLOR_RED);
					BSP_LCD_FillRect(396, 14, 70, 50);
					BSP_LCD_DisplayChar(428, 33, 83);
					BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
					BSP_LCD_DisplayStringAtLine(1, (uint8_t *)sys_opening_scr);
					BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
					BSP_LCD_DrawCircle(save_x, save_y, 20);
					BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
					sprintf(coordinates, " X%3d - Y%3d", save_x, abs(save_y - 272));
					BSP_LCD_DisplayStringAtLine(1, (uint8_t *)coordinates);
					red_button_flag = 0;
					osDelay(100);
				}

				if (!first_touch_detected_flag) {
					first_touch_detected_flag = 1;
					possible_click_event = 1;
					last_ts_coord.x = ts_state.touchX[0];
					last_ts_coord.y = ts_state.touchY[0];
					first_ts_coord.x = ts_state.touchX[0];
					first_ts_coord.y = ts_state.touchY[0];
				} else {
					int8_t diff_x = ts_state.touchX[0] - last_ts_coord.x;
					int8_t diff_y = ts_state.touchY[0] - last_ts_coord.y;

					HID_Buffer[1] = diff_x * 3;
					HID_Buffer[2] = diff_y * 3;

					last_ts_coord.x = ts_state.touchX[0];
					last_ts_coord.y = ts_state.touchY[0];

					// Check if the user finger left a predefined area
					// This means that this is not a clicking, just a cursor movement
					int32_t click_diff_x = ts_state.touchX[0] - first_ts_coord.x;
					int32_t click_diff_y = ts_state.touchY[0] - first_ts_coord.y;
					if (abs(click_diff_x) > TS_CLICK_THRESHOLD || abs(click_diff_y) > TS_CLICK_THRESHOLD)
						possible_click_event = 0;

					cor_x = ts_state.touchX[0];
					cor_y = ts_state.touchY[0];
					sprintf(coordinates, " X%3d - Y%3d", cor_x, abs(cor_y - 272));

					if ((20 < ts_state.touchX[0]) && (30 < ts_state.touchY[0]) && (376 > ts_state.touchX[0]) && (260 > ts_state.touchY[0]) && !red_button_flag) {
						BSP_LCD_FillCircle(ts_state.touchX[0], ts_state.touchY[0], 4);
						BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
						BSP_LCD_DisplayStringAtLine(1, (uint8_t *)coordinates);
					}
				}
			} else {
				BSP_LED_Off(LED1);
				if ((ts_state.touchX[0] > 0) && (ts_state.touchY[0] > 0) && !drawing_flag) {
					if ((20 < ts_state.touchX[0]) && (30 < ts_state.touchY[0]) && (376 > ts_state.touchX[0]) && (260 > ts_state.touchY[0]) && !red_button_flag) {
						osDelay(500);
						BSP_LCD_SetTextColor(LCD_COLOR_RED);
						BSP_LCD_DrawCircle(ts_state.touchX[0], ts_state.touchY[0], 20);
						osDelay(2000);
						BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
						BSP_LCD_DrawCircle(ts_state.touchX[0], ts_state.touchY[0], 20);
						BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
						save_x = ts_state.touchX[0];
						save_y = ts_state.touchY[0];
						sprintf(coordinates, " X%3d - Y%3d", save_x, abs(save_y - 272));
						BSP_LCD_DisplayStringAtLine(2, (uint8_t *)coordinates);
					}
					drawing_flag = 1;
				}
				//first_touch_detected_flag = 0;
				if (possible_click_event) {
					possible_click_event = 0;
				}
			}
		}
}

void string_splitter(void){

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
		if (slave_sock < 0)
			LCD_ErrLog("accept()");
		LCD_UsrLog("connection accepted\n");

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

void servo_control_thread(void const * argument)
{
	debug = 1;

	LCD_UsrLog((char*) "Servo control thread started\n");

	pwm_init();
	adc_init();
	position = 0;

	while (1) {
		pwm_set_duty_from_adc();
		osDelay(10);
	}

	while (1) {
		osThreadTerminate(NULL);
    }
}

/**
  * @brief  Initializes TIM1 as PWM source
  * @param  None
  * @retval None
  */
void pwm_init(void)
{
	// TIM3 init as PWM
	pwm_handle.Instance = TIM3;
	pwm_handle.State = HAL_TIM_STATE_RESET;
	pwm_handle.Channel = HAL_TIM_ACTIVE_CHANNEL_1;
	pwm_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	pwm_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	pwm_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
	pwm_handle.Init.Period = 0xFFFF;
	pwm_handle.Init.Prescaler = 30;
	HAL_TIM_PWM_Init(&pwm_handle);

	pwm_oc_init.OCFastMode = TIM_OCFAST_DISABLE;
	pwm_oc_init.OCIdleState = TIM_OCIDLESTATE_RESET;
	pwm_oc_init.OCMode = TIM_OCMODE_PWM1;
	pwm_oc_init.OCPolarity = TIM_OCPOLARITY_LOW;
	pwm_oc_init.Pulse = 0x7FFF;
	HAL_TIM_PWM_ConfigChannel(&pwm_handle, &pwm_oc_init, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&pwm_handle, TIM_CHANNEL_1);
}

/**
  * @brief  Sets the duty cycle of TIM3 CH1
  * @param  duty - duty cycle to set (5.0-10.0)
  * @retval None
  */

void pwm_set_duty_from_adc(void)
{
	pwm_set_duty(get_degrees());
	return;
}

void pwm_set_duty(uint8_t rot_degree)
{
	// Calculate pulse width
	int min_duty = (65536 * MIN_POS_DUTY_CYCLE) / 100;
	int max_duty = (65536 * MAX_POS_DUTY_CYCLE) / 100;

	// Set pulse width
	uint16_t pulse = min_duty + ((max_duty - min_duty) * rot_degree) / 180;
	pwm_oc_init.Pulse = pulse;
	HAL_TIM_PWM_ConfigChannel(&pwm_handle, &pwm_oc_init, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&pwm_handle, TIM_CHANNEL_1);

	if (debug) {
		char tmp[20];
		sprintf(tmp, "Angle: %2d - Pulse: %5d \n", rot_degree, pulse);
		LCD_UsrLog(tmp);
	}
	return;
}

/**
  * @brief  Initializes ADC3 to measure voltage on CH0
  * @param  None
  * @retval None
  */
void adc_init()
{
	adc_handle.State = HAL_ADC_STATE_RESET;
	adc_handle.Instance = ADC3;
	adc_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	adc_handle.Init.Resolution = ADC_RESOLUTION_12B;
	adc_handle.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	adc_handle.Init.DMAContinuousRequests = DISABLE;
	adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	adc_handle.Init.ContinuousConvMode = DISABLE;
	adc_handle.Init.DiscontinuousConvMode = DISABLE;
	adc_handle.Init.ScanConvMode = DISABLE;
	HAL_ADC_Init(&adc_handle);

	adc_ch_conf.Channel = ADC_CHANNEL_0;
	adc_ch_conf.Offset = 0;
	adc_ch_conf.Rank = 1;
	adc_ch_conf.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	HAL_ADC_ConfigChannel(&adc_handle, &adc_ch_conf);
}

/**
  * @brief  Measures the voltage with ADC3 on CH0
  * @param  None
  * @retval Measured value (0-4095)
  */
uint16_t adc_measure(void)
{
	HAL_ADC_Start(&adc_handle);
	HAL_ADC_PollForConversion(&adc_handle, HAL_MAX_DELAY);
	uint16_t value = HAL_ADC_GetValue(&adc_handle);
	HAL_ADC_Stop(&adc_handle);

	if (debug) {
		char tmp[20];
		sprintf(tmp, "ADC value: %d\n", value);
		LCD_UsrLog(tmp);
	}
	return value;
}

/**
  * @brief  Calculates degree of rotation from ADC measurement
  * @param  None
  * @retval Degree (0-180)
  */
uint8_t get_degrees(void)
{
	uint16_t adc_value = adc_measure();
	uint8_t degrees = (uint32_t) ( adc_value * (MAX_DEGREE - MIN_DEGREE)) / (MAX_ADC_VALUE - MIN_ADC_VALUE);

	return degrees;
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
