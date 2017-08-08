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
	// Configure UART instance
	uart_handle.Instance        = USARTx;
	uart_handle.Init.BaudRate   = 115200;
	uart_handle.Init.WordLength = UART_WORDLENGTH_8B;
	uart_handle.Init.StopBits   = UART_STOPBITS_1;
	uart_handle.Init.Parity     = UART_PARITY_NONE;
	uart_handle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	uart_handle.Init.Mode       = UART_MODE_TX_RX;

	// Configure COM1 as UART
	BSP_COM_Init(COM1, &uart_handle);

	return;
}

void UART_Error_Handler(void)
{

}

void UART_send(char* buffer)
{
	uint16_t buffer_len = strlen(buffer);
	uint32_t timeout = 100;

	// Send buffer content
	HAL_UART_Transmit(&uart_handle, (uint8_t*) buffer, buffer_len, timeout);

	// Send new line
	HAL_UART_Transmit(&uart_handle, (uint8_t*) "\r\n", 2, timeout);

	if (debug) {
		LCD_UsrLog((char*) "UART TX: ");
		LCD_UsrLog((char*) buffer);
		LCD_UsrLog((char*) "\n");
	}

	return;
}

void UART_send_help(void)
{
	UART_send("*** Greetings from TotoRobot! ***");
	UART_send("");
	UART_send("Commands:");
	UART_send("");
	UART_send("get pulse                    - Get servos' current pulse width");
	UART_send("get angle                    - Get servos' current angle");
	UART_send("get pos                      - Get robot arm's current xyz coordinates");
	UART_send("get manual                   - Get current manual control status");
	UART_send("get display                  - Get current status of LCD data display");
	UART_send("");
	UART_send("set pulse <servo> <value>    - Set servo pulse width");
	UART_send("set angle <servo> <value>    - Set servo angle");
	UART_send("set position <x,y,z>         - Set robot arm xyz coordinates");
	UART_send("set manual <0|1>             - Turn on or off manual control");
	UART_send("set display <0|1>            - Turn on data display on LCD");
	UART_send("");
	UART_send("Always terminate commands with LF!");
	UART_send("");

	return;
}

void UART_rx_thread(void const * argument)
{
	uint32_t timeout = 100;

	uart_init();
	UART_send_help();

	while(1) {

		// UART RX polling mode
		if(HAL_UART_Receive(&uart_handle, RX_buffer, RXBUFFERSIZE, timeout) != HAL_OK) {
			UART_Error_Handler();
		}

		for (int i = 0; i < RXBUFFERSIZE; i++) {
			if ((RX_buffer[i] == '\r') || (RX_buffer[i]) == '\n') {
				RX_buffer[i] = '\0';
				break;
			}
		}

		// Process command
		if (RX_buffer[0] != '\0') {

			// Log to screen
			if (debug) {
				LCD_UsrLog((char*) "UART RX:");
				LCD_UsrLog((char*) RX_buffer);
				LCD_UsrLog((char*) "\n");
			}

			// Process command
			process_command();
			execute_command();

			// Clear buffer
			RX_buffer[0] = '\0';
		}

	}

	while (1) {
		if (debug) {
			LCD_ErrLog((char*) "UART RX thread terminating\n");
		}
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
	strcpy(received, (char*) RX_buffer);

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
		c_params.error = 1;
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
	} else {
		c_params.error = 1;
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
			c_params.error = 1;
		}
	}

	// Value
	if ((c_params.command == SET_VALUE) &&
		((c_params.attrib == PULSE) || (c_params.attrib == ANGLE) || (c_params.attrib == MANUAL_CONTROL))) {
		char* s = strtok(NULL, " ");

		// Convert ASCII to integer
		c_params.value = atoi(s);
	}

	// XYZ value
	if ((c_params.command == SET_VALUE) && (c_params.attrib == POSITION)) {
		char* s = strtok(NULL, " ");

		char* coord = strtok(s, ",");
		c_params.value_x = atoi(coord);

		coord = strtok(NULL, ",");
		c_params.value_y = atoi(coord);

		coord = strtok(NULL, ",");
		c_params.value_y = atoi(coord);

		c_params.error = verify_coordinates(c_params.value_x, c_params.value_y, c_params.value_z);
	}
	return;
}

void execute_command(void)
{
	// Send error message
	if (c_params.error) {
		UART_send("Unrecognized command or value");
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
			osMutexWait(servo_pos_mutex, osWaitForever);
			uint32_t pulse = servo_pos[i].pulse;
			osMutexRelease(servo_pos_mutex);
			// Send value
			sprintf(TX_buffer, "servo%d pulse: %d", i, pulse);
			UART_send((char*) TX_buffer);
		}
		break;
	case ANGLE:
		for (int i = 0; i < SERVOS; i++) {
			// Get value
			osMutexWait(servo_pos_mutex, osWaitForever);
			uint8_t angle = servo_pos[i].angle;
			osMutexRelease(servo_pos_mutex);
			// Send value
			sprintf(TX_buffer, "servo%d angle: %4d degrees", i, angle);
			UART_send((char*)TX_buffer);
		}
		break;
	case POSITION:
		// Get value
		osMutexWait(servo_pos_mutex, osWaitForever);
		uint32_t x = arm_position.x;
		uint32_t y = arm_position.y;
		uint32_t z = arm_position.z;
		osMutexRelease(servo_pos_mutex);

		// Send value
		sprintf(TX_buffer, "arm position: x:%d y:%d z:%d", x, y, z);
		UART_send((char*) TX_buffer);
		break;
	case MANUAL_CONTROL:
		sprintf(TX_buffer, "Manual control is %s", adc_on ? "on" : "off");
		UART_send((char*) TX_buffer);
		break;
	case DATA_DISP:
		sprintf(TX_buffer, "LCD data display is %s", lcd_data_display_on ? "on" : "off");
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
		osMutexWait(servo_pos_mutex, osWaitForever);
		servo_pos[c_params.device_id].pulse = c_params.value;
		osMutexRelease(servo_pos_mutex);
		UART_send("Set pulse done.");
		break;
	case ANGLE:
		osMutexWait(servo_pos_mutex, osWaitForever);
		servo_pos[c_params.device_id].angle = c_params.value;
		osMutexRelease(servo_pos_mutex);
		UART_send("Set angle done.");
		break;
	case POSITION:
		osMutexWait(servo_pos_mutex, osWaitForever);
		arm_position.x = c_params.value_x;
		arm_position.y = c_params.value_y;
		arm_position.z = c_params.value_z;
		osMutexRelease(servo_pos_mutex);
		UART_send("Set position done.");
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
	case NO_ATTRIB:
		break;
	}
	return;
}

uint8_t verify_coordinates(uint16_t x, uint16_t y, uint16_t z) {
	return 0;
}

uint8_t verify_pulse(uint32_t pulse) {
	return 0;
}

uint8_t verify_angle(uint32_t angle) {
	return 0;
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
