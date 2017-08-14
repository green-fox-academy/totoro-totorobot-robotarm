/* Includes ------------------------------------------------------------------*/
#include "uart.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void uart_init(void)
{
	// Initialize circular buffer pointers
	RX_buffer.head_p = &(RX_buffer.buffer[0]);
	RX_buffer.tail_p = &(RX_buffer.buffer[RXBUFFERSIZE - 1]);
	RX_buffer.read_p = RX_buffer.head_p;
	RX_buffer.write_p = RX_buffer.head_p;

	// Zero out circular buffer
	for (int i = 0; i < RXBUFFERSIZE; i++) {
		RX_buffer.buffer[i] = 0;
	}

	// Init flags
	command_in = 0;

	// Configure UART instance
	uart_handle.Instance        = USART1;
	uart_handle.Init.BaudRate   = 9600;
	uart_handle.Init.WordLength = UART_WORDLENGTH_8B;
	uart_handle.Init.StopBits   = UART_STOPBITS_1;
	uart_handle.Init.Parity     = UART_PARITY_NONE;
	uart_handle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	uart_handle.Init.Mode       = UART_MODE_TX_RX;

	// Configure COM1 as UART
	BSP_COM_Init(COM1, &uart_handle);

	//Setup interrupts for UART
	HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(USART1_IRQn);

	// Start UART receiver in interrupt mode
	HAL_UART_Receive_IT(&uart_handle, &char_buff, 1);

	log_msg(DEBUG, "UART init done\n");

	return;
}

void UART_send(char* buffer)
{
	uint16_t buffer_len = strlen(buffer);
	uint32_t timeout = 500;

	// Send buffer content
	HAL_UART_Transmit(&uart_handle, (uint8_t*) buffer, buffer_len, timeout);

	// Send new line
	HAL_UART_Transmit(&uart_handle, (uint8_t*) "\r\n", 2, timeout);

	// Log buffer content
	char tmp[100];
	sprintf(tmp, "UART TX: %s\n", buffer);
	log_msg(DEBUG, tmp);

	return;
}

void UART_send_help(void)
{
	UART_send("*** Greetings from TotoRobot! ***");
	UART_send(" ");
	UART_send("Commands:");
	UART_send(" ");
	UART_send("get pulse                    - Get servos' current pulse width");
	UART_send("get angle                    - Get servos' current angle");
	UART_send("get pos                      - Get robot arm's current xyz coordinates");
	UART_send("get manual                   - Get current manual control status");
	UART_send("get display                  - Get current status of LCD data display");
	UART_send("get demo                     - Get current status of the demo");
	UART_send(" ");
	UART_send("set pulse <servo> <value>    - Set servo pulse width");
	UART_send("set angle <servo> <value>    - Set servo angle");
	UART_send("set position <x,y,z>         - Set robot arm xyz coordinates");
	UART_send("set manual <0|1>             - Turn on or off manual control");
	UART_send("set display <0|1>            - Turn on or off data display on LCD");
	UART_send("set demo <0|1>               - Start/stop demo");
	UART_send(" ");
	UART_send("Always terminate commands with LF!");
	UART_send(" ");

	return;
}

void UART_rx_thread(void const * argument)
{
	uint8_t command_buffer[RXBUFFERSIZE];
	uint8_t char_to_copy;
	command_buffer[0] = 0;

	uart_init();
	UART_send_help();

	while (1) {

		if (command_in) {

			// TODO do a chek on the circ buffer state
			// if nearly full, send alert -> Command buffer full


			int i = 0;

			// Copy data from circular buffer to command buffer
			// between the last read pointer and the next LF
			do {
				uint8_t char_to_copy = *(RX_buffer.read_p++);

				// Loop read pointer
				if (RX_buffer.read_p > RX_buffer.tail_p) {
					RX_buffer.read_p = RX_buffer.head_p;
				}

				command_buffer[i++]= char_to_copy;

			} while (char_to_copy != '\n');

			// Reset flag
			command_in = 0;

			// Remove CR/LF char
			command_buffer[i - 1] = 0;
			if (command_buffer[i - 2] == '\r') {
				command_buffer[i - 2] = 0;
			}

			// Process command

			// Decrease counter
			// TODO use mutex
			command_in--;

		}


//		// UART RX polling mode
//		if (HAL_UART_Receive(&uart_handle, RX_buffer, RXBUFFERSIZE, timeout) != HAL_OK) {
//			UART_Error_Handler();
//		}
//
//		for (int i = 0; i < RXBUFFERSIZE; i++) {
//			if ((RX_buffer[i] == '\r') || (RX_buffer[i]) == '\n') {
//				RX_buffer[i] = '\0';
//				break;
//			}
//		}
//
//		// Process command
//		if (RX_buffer[0] != '\0') {
//
//			// Log buffer content
//			char tmp[100];
//			sprintf(tmp, "UART RX: %s\n", RX_buffer);
//			log_msg(DEBUG, tmp);
//
//			// Process command
//			process_command();
//			execute_command();
//
//			// Clear buffer
//			RX_buffer[0] = '\0';
//		}
	}

	while (1) {
		log_msg(USER, "UART RX thread terminating\n");
		osThreadTerminate(NULL);
	}
}

void process_command(void)
{
	// Clear command structure
	c_params.attrib = NO_ATTRIB;
	c_params.command = NO_COMMAND;
	c_params.device_id = 255;
	c_params.value = 0;
	c_params.value_x = 65535;
	c_params.value_y = 65535;
	c_params.value_z = 65535;
	c_params.error = 0;

	// Copy command from UART RX buffer
	char received[RXBUFFERSIZE];
	//strcpy(received, (char*) RX_buffer);

	// Command
	char* s = strtok(received, " ");

	if ((strcmp(s, "set") == 0) || (strcmp(s, "s") == 0)) {
		c_params.command = SET_VALUE;
	} else if ((strcmp(s, "get") == 0) || (strcmp(s, "g") == 0)) {
		c_params.command = GET_VALUE;
	} else if ((strcmp(s, "help") == 0) || (strcmp(s, "h") == 0)) {
		c_params.command = HELP;
		return;
	} else {
		c_params.error = 4;
		return;
	}

	// Attribute
	s = strtok(NULL, " ");

	if ((strcmp(s, "pulse") == 0) || (strcmp(s, "pul") == 0)) {
		c_params.attrib = PULSE;
	} else if ((strcmp(s, "position") == 0) || (strcmp(s, "pos") == 0)) {
		c_params.attrib = POSITION;
	} else if ((strcmp(s, "angle") == 0) || (strcmp(s, "ang") == 0)) {
		c_params.attrib = ANGLE;
	} else if ((strcmp(s, "manual") == 0) || (strcmp(s, "man") == 0)) {
		c_params.attrib = MANUAL_CONTROL;
	} else if ((strcmp(s, "display") == 0) || (strcmp(s, "dis") == 0)) {
		c_params.attrib = DATA_DISP;
	} else if ((strcmp(s, "demo") == 0) || (strcmp(s, "dem") == 0)) {
		c_params.attrib = DEMO;
	} else {
		c_params.error = 2;
		return;
	}

	// Device id
	if ((c_params.command == SET_VALUE) &&
		((c_params.attrib == PULSE) || (c_params.attrib == ANGLE))) {
		char* s = strtok(NULL, " ");

		// Convert ASCII to integer
		c_params.device_id = atoi(s);

		// Check if we are in the accepted range
		if (c_params.device_id >= SERVOS) {
			c_params.error = 3;
		}
	}

	// Value
	if ((c_params.command != HELP) && (c_params.command != GET_VALUE)
		&& (c_params.attrib != POSITION)) {
		char* s = strtok(NULL, " ");

		// Convert ASCII to integer
		c_params.value = atoi(s);

		if ((c_params.command == SET_VALUE) && (c_params.attrib == PULSE)) {
			c_params.error = verify_pulse(c_params.device_id, c_params.value);
		}

		if ((c_params.command == SET_VALUE) && (c_params.attrib == ANGLE)) {
			c_params.error = verify_angle(c_params.device_id, c_params.value);
		}
	}

	// XYZ value
	if ((c_params.command == SET_VALUE) && (c_params.attrib == POSITION)) {
		char* s = strtok(NULL, " ");

		char* coord = strtok(s, ",");
		c_params.value_x = atoi(coord);

		coord = strtok(NULL, ",");
		c_params.value_y = atoi(coord);

		coord = strtok(NULL, ",");
		c_params.value_z = atoi(coord);

		c_params.error = verify_coordinates(c_params.value_x, c_params.value_y, c_params.value_z);
	}

	// Log command parameters
	char tmp[100];
	sprintf(tmp, "command: %d, attrib: %d, dev: %d, value: %d, x: %d, y: %d, z: %d, err: %d\n",
				c_params.command, c_params.attrib, c_params.device_id, c_params.value,
				c_params.value_x, c_params.value_y, c_params.value_z, c_params.error);
	log_msg(DEBUG, tmp);

	return;
}

void execute_command(void)
{
	// Send error message
	if (c_params.error) {
		sprintf((char*)TX_buffer, "Unrecognized command or value: %s", RX_buffer);
		UART_send((char*)TX_buffer);
		return;
	}

	// Execute command
	switch (c_params.command) {

	// Help
	case HELP:
		UART_send_help();
		break;

	// Get value
	case GET_VALUE:
		UART_send_settings();
		break;

	// Set value
	case SET_VALUE:
		set_value();
		break;

	// Error
	default:
		UART_send("Unrecognized command or value");
		return;
	}
	return;
}

void UART_send_settings(void)
{
	switch (c_params.attrib) {

	case PULSE:
		for (int i = 0; i < SERVOS; i++) {
			// Get value
			osMutexWait(servo_pulse_mutex, osWaitForever);
			uint32_t pulse = servo_pulse[i];
			osMutexRelease(servo_pulse_mutex);
			// Send value
			sprintf((char*) TX_buffer, "Servo%d pulse: %lu", i, pulse);
			UART_send((char*) TX_buffer);
		}
		break;

	case ANGLE:
		for (int i = 0; i < SERVOS; i++) {
			// Get pulse value
			osMutexWait(servo_pulse_mutex, osWaitForever);
			uint32_t pulse = servo_pulse[i];
			osMutexRelease(servo_pulse_mutex);

			// Calculate angle
			uint8_t angle = (uint8_t) map((double) pulse, (double) servo_conf[i].min_pulse,
					(double) servo_conf[i].max_pulse, (double) servo_conf[i].min_angle_deg,
					(double) servo_conf[i].max_angle_deg);

			// Send value
			sprintf((char*) TX_buffer, "Servo%d angle: %4d degrees", i, angle);
			UART_send((char*)TX_buffer);
		}
		break;

	case POSITION:

		// A block statement is needed for the declaration
		{
			coord_cart_t xyz;

			// Get xyz values
			pulse_to_xyz(&xyz);

			// Send value
			sprintf((char*) TX_buffer, "arm position: x:%d y:%d z:%d", (int16_t) xyz.x, (int16_t) xyz.y, (int16_t) xyz.z);
			UART_send((char*) TX_buffer);
		}
		break;

	case MANUAL_CONTROL:
		sprintf((char*) TX_buffer, "Manual control is %s", adc_on ? "on" : "off");
		UART_send((char*) TX_buffer);
		break;

	case DATA_DISP:
		sprintf((char*) TX_buffer, "LCD data display is %s", lcd_data_display_on ? "on" : "off");
		UART_send((char*) TX_buffer);
		break;

	case DEMO:
		sprintf((char*) TX_buffer, "Demo is %s", demo_on ? "running" : "off");
		UART_send((char*) TX_buffer);
		break;

	case NO_ATTRIB:
		break;
	}
	return;
}

void set_value(void)
{
	switch (c_params.attrib) {

	case PULSE:

		osMutexWait(servo_pulse_mutex, osWaitForever);
		servo_pulse[c_params.device_id] = c_params.value;
		osMutexRelease(servo_pulse_mutex);
		UART_send("Set pulse done.");
		break;

	case ANGLE:

		// TODO correct 2nd joint angle

		// A block statement is needed for the declaration
		{
			// Convert degree to radians
			double ang_rad = deg_to_rad(c_params.value);

			// Calculate pulse
			uint32_t pulse = (uint32_t) map(ang_rad, servo_conf[c_params.device_id].min_angle_rad,
										  servo_conf[c_params.device_id].max_angle_rad,
										  (double) servo_conf[c_params.device_id].min_pulse,
										  (double) servo_conf[c_params.device_id].max_pulse);

			// Set pulse
			osMutexWait(servo_pulse_mutex, osWaitForever);
			servo_pulse[c_params.device_id] = pulse;
			osMutexRelease(servo_pulse_mutex);
			UART_send("Set angle done.");
		}
		break;

	case POSITION:

		// A block statement is needed for the declaration
		{
			// Read in xyz values
			coord_cart_t coord;
			coord.x = (double) c_params.value_x;
			coord.y = (double) c_params.value_y;
			coord.z = (double) c_params.value_z;

			// Set pwm pulse
			xyz_to_pulse(&coord);

			UART_send("Set position done.");
		}
		break;

	case MANUAL_CONTROL:
		if (c_params.value > 0) {
			start_adc_thread();
			UART_send("Manual control started, ADC running.");
		} else {
			stop_adc_thread();
			UART_send("Manual control ended, ADC terminated.");
		}
		break;

	case DATA_DISP:
		if (c_params.value > 0) {
			start_lcd_data_display();
			UART_send("LCD data display turned on.");
		} else {
			stop_lcd_data_display();
			UART_send("LCD data display turned off.");
		}
		break;

	case DEMO:
		if (c_params.value > 0) {
			start_demo();
			UART_send("Demo is on.");
		} else {
			stop_demo();
			UART_send("Demo is turned off.");
		}
		break;

	case NO_ATTRIB:
		break;
	}
	return;
}

uint8_t verify_coordinates(int16_t x, int16_t y, int16_t z) {

	if ((x > WORK_AREA_MAX_X) || (x < WORK_AREA_MIN_X)) {
		return 1;	// Flag error
	}

	if ((y > WORK_AREA_MAX_Y) || (y < WORK_AREA_MIN_Y)) {
		return 1;	// Flag error
	}

	if ((z > WORK_AREA_MAX_Z) || (z < WORK_AREA_MIN_Z)) {
		return 1;	// Flag error
	}

	return 0;
}

uint8_t verify_pulse(uint8_t servo, uint32_t pulse) {

	if ((pulse > servo_conf[servo].max_pulse) || (pulse < servo_conf[servo].min_pulse)) {
		return 1; // Flag error
	}

	return 0;
}

uint8_t verify_angle(uint8_t servo, int16_t angle) {

	if ((angle > servo_conf[servo].max_angle_deg) || (angle < servo_conf[servo].min_angle_deg)) {
		return 1;  // Flag error
	}

	return 0;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	// This interrupt is fired when we receive a single character via USART1
	// Copy character into the circular buffer
	*(RX_buffer.write_p) = char_buff;

	// Increase write pointer value;
	RX_buffer.write_p++;

	// Loop if write pointer is out of bounds
	if (RX_buffer.write_p > RX_buffer.tail_p) {
		RX_buffer.write_p = RX_buffer.head_p;
	}

	// Raise flag if we received LF character
	if (char_buff == '\n') {
		// TODO use mutex
		command_in++;
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
