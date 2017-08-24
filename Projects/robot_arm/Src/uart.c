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

	// Init command queue length counter
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
	/*
	 * WARNING: Heavy LCD screen logging will break the UART interface.
	 * Keep logging lines commented out unless you need them for debug.
	 * In that case make sure commands are supplied at a slow rate.
	 */

	uint16_t buffer_len = strlen(buffer);
	uint32_t timeout = 500;

	// Send buffer content
	HAL_UART_Transmit(&uart_handle, (uint8_t*) buffer, buffer_len, timeout);

	// Send new line
	HAL_UART_Transmit(&uart_handle, (uint8_t*) "\r\n", 2, timeout);

	// Uncomment for debug
	//
	// char tmp[TXBUFFERSIZE + 20] = "UART TX: ";
	// strcat(tmp, buffer);
	// strcat(tmp, "\n");
	// log_msg(DEBUG, buffer);

	return;
}

void UART_send_help(void)
{
	UART_send("*** Greetings from TotoRobot! ***");
	UART_send("Commands:");
	UART_send(" get pulse | set pulse <servo> <value>  : Get/set PWM pulse width");
	UART_send(" get angle | set angle <servo> <value>  : Get/set servo angle");
	UART_send(" get position | set position <x,y,z>    : Get/set arm xyz coordinates");
	UART_send(" get manual | set manual <0|1>          : Get/set manual control");
	UART_send(" get display | set display <0|1>        : Get/set LCD data display");
	UART_send(" get demo | set demo <0|1>              : Get/set demo playing");
	UART_send(" exec file <file_name.g>                : Execute G code from SD card");
	UART_send("*** Always terminate commands with LF! ***");

	return;
}

void UART_rx_thread(void const * argument)
{

	/*
	 * WARNING: Heavy LCD screen logging will break the UART interface.
	 * Keep logging lines commented out unless you need them for debug.
	 * In that case make sure commands are supplied at a slow rate.
	 */

	uint8_t char_to_copy;

	uart_init();
	UART_send_help();

	while (1) {

		if (command_in) {

			uint16_t i = 0;

			// Copy data from circular buffer to command buffer
			// between current position of the read pointer and the next LF
			do {
				// Copy one char and move the read pointer to the next one
				char_to_copy = *(RX_buffer.read_p++);

				// Loop read pointer at the end of the circular buffer
				if (RX_buffer.read_p > RX_buffer.tail_p) {
					RX_buffer.read_p = RX_buffer.head_p;
				}

				// Add char to command buffer and move pointer by one
				command_buffer[i++]= char_to_copy;

			} while (char_to_copy != '\n');

			// Remove LF char from the last written position
			command_buffer[--i] = 0;

			if ((i > 0) && (command_buffer[--i] == '\r')) {
				command_buffer[i] = 0;
			}

			// Uncomment for debug
			//
			// char tmp[RXBUFFERSIZE + 20] = "command_buffer: ";
			// strcat(tmp, (char*) command_buffer);
			// strcat(tmp, "\n");
			// log_msg(DEBUG, tmp);

			// Process command
			process_command();
			execute_command();

			// Decrease command counter
			command_in--;
		}

		osDelay(10);
	}

	while (1) {
		log_msg(USER, "UART RX thread terminating\n");
		osThreadTerminate(NULL);
	}
}

void process_command(void)
{

	/*
	 * WARNING: Heavy LCD screen logging will break the UART interface.
	 * Keep logging lines commented out unless you need them for debug.
	 * In that case make sure commands are supplied at a slow rate.
	 */

	// Clear command structure
	c_params.attrib = NO_ATTRIB;
	c_params.command = NO_COMMAND;
	c_params.device_id = 255;
	c_params.value = 0;
	c_params.value_x = 65535;
	c_params.value_y = 65535;
	c_params.value_z = 65535;
	c_params.error = 0;
	c_params.file_name[0] = 0;

	// Copy command from command buffer
	char received[RXBUFFERSIZE];
	strcpy(received, (char*) command_buffer);

	// Command
	char* s = strtok(received, " ");

	if ((strcmp(s, "set") == 0) || (strcmp(s, "s") == 0)) {
		c_params.command = SET_VALUE;
	} else if ((strcmp(s, "get") == 0) || (strcmp(s, "g") == 0)) {
		c_params.command = GET_VALUE;
	} else if ((strcmp(s, "help") == 0) || (strcmp(s, "h") == 0)) {
		c_params.command = HELP;
		return;
	} else if ((strcmp(s, "exec") == 0) || (strcmp(s, "e") == 0)) {
		c_params.command = EXECUTE;
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
	} else if ((strcmp(s, "file") == 0) || (strcmp(s, "fil") == 0)) {
		c_params.attrib = FILE_NAME;
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

		// Verify if pulse is within allowed range
		if ((c_params.command == SET_VALUE) && (c_params.attrib == PULSE)) {
			c_params.error = verify_pulse(c_params.device_id, c_params.value);
		}

		// Verify if angles are within allowed range
		// Angles are in degrees, theta1 and theta2 are measured to the horizon
		if ((c_params.command == SET_VALUE) && (c_params.attrib == ANGLE)) {

			angles_t ang_deg;
			pulse_to_ang_abs(&ang_deg);

			switch (c_params.device_id) {
			case 0:
				ang_deg.theta0 = (double) c_params.value;
				break;
			case 1:
				ang_deg.theta1 = (double) c_params.value;
				break;
			case 2:
				ang_deg.theta2 = (double) c_params.value;
				break;
			}

			c_params.error = verify_angle(&ang_deg);
		}
	}

	// XYZ value
	if ((c_params.command == SET_VALUE) && (c_params.attrib == POSITION)) {
		char* s = strtok(NULL, " ");

		char* coord = strtok(s, ",");
		if (coord != NULL) {
			c_params.value_x = atoi(coord);
		} else {
			c_params.error = 5;
			return;
		}

		coord = strtok(NULL, ",");
		if (coord != NULL) {
			c_params.value_y = atoi(coord);
		} else {
			c_params.error = 5;
			return;
		}

		coord = strtok(NULL, ",");
		if (coord != NULL) {
			c_params.value_z = atoi(coord);
		} else {
			c_params.error = 5;
			return;
		}

		// Verify if xyz coordinates are within working area
		coord_cart_t xyz_coord;
		xyz_coord.x = (double) c_params.value_x;
		xyz_coord.y = (double) c_params.value_y;
		xyz_coord.z = (double) c_params.value_z;
		c_params.error = verify_xyz(&xyz_coord);
	}

	// File name
	if ((c_params.command == EXECUTE) && (c_params.attrib == FILE_NAME)) {
		char* s = strtok(NULL, " ");

		log_msg(DEBUG, s);
		log_msg(DEBUG, "\n");

		// Get file name
		strcpy(c_params.file_name, s);

		// Check if file exists
		// c_params.error = verify_file(c_params.file_name);
	}

	// Uncomment for debug

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
		sprintf((char*) TX_buffer, "Unrecognized command or value: %s", (char*) command_buffer);
		UART_send((char*) TX_buffer);
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

	// Execute file
	case EXECUTE:
		execute_file();
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
			// Read value from global storage
			osMutexWait(servo_pulse_mutex, osWaitForever);
			uint32_t pulse = servo_pulse[i];
			osMutexRelease(servo_pulse_mutex);
			// Send value to user
			sprintf((char*) TX_buffer, "Servo%d pulse: %lu", i, pulse);
			UART_send((char*) TX_buffer);
		}
		break;

	case ANGLE:

		// A block statement is needed for the declaration
		{
			angles_t servo_angles;
			double angles[SERVOS - 1];

			// Convert pulse values to absolute angle
			pulse_to_ang_abs(&servo_angles);

			// Feed angles into list
			angles[0] = servo_angles.theta0;
			angles[1] = servo_angles.theta1;
			angles[2] = servo_angles.theta2;

			// Convert angles from radian to degrees and print them
			for (int i = 0; i < SERVOS - 1; i++) {
				sprintf((char*) TX_buffer, "Servo%d angle: %4d degrees", i, rad_to_deg(angles[i]));
				UART_send((char*)TX_buffer);
			}
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
		{
			uint32_t targ_pulse[SERVOS];

			// If manual control is on, turn it off
			if (adc_on) {
				stop_adc_thread();
				UART_send("Manual control ended, ADC terminated.");
			}

			// Pulse value has been checked in process_command()

			// Get current pulse
			osMutexWait(servo_pulse_mutex, osWaitForever);
			for (int i = 0; i < SERVOS; i++) {
				targ_pulse[i] = servo_pulse[i];
			}
			osMutexRelease(servo_pulse_mutex);

			// Set target pulse
			targ_pulse[c_params.device_id] = c_params.value;

			// Send target pulse
			while (1) {
				osMutexWait(arm_coord_mutex, osWaitForever);
				if (!next_coord_set) {

					// Set pulse
					for (int i = 0; i < SERVOS; i++) {
						target_pulse[i] = targ_pulse[i];
					}

					// Set display message
					sprintf(target_display, "s%d pulse %d          ", c_params.device_id, c_params.value);

					// Reset next coordinate flag, so that other processes can use it
					next_coord_set = 1;

					// Release mutex and break out of loop
					osMutexRelease(arm_coord_mutex);
					break;
				}
				osMutexRelease(arm_coord_mutex);
				osDelay(10);
			}

			// Set flag to one movement only
			end_moving = 1;

			// Launch process to set pulse
		    osThreadDef(SET_PULSE, set_pulse_thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 10);
		    osThreadCreate (osThread(SET_PULSE), NULL);

			UART_send("Target pulse sent.");
		}
		break;

	case ANGLE:

		// A block statement is needed for declaration
		{
			angles_t targ_ang_rad;

			// If manual control is on, turn it off
			if (adc_on) {
				stop_adc_thread();
				UART_send("Manual control ended, ADC terminated.");
			}

			// Get current angles
			pulse_to_ang_abs(&targ_ang_rad);

			// Update with user data
			switch (c_params.device_id) {
			case 0:
				targ_ang_rad.theta0 = deg_to_rad(c_params.value);
				break;
			case 1:
				targ_ang_rad.theta1 = deg_to_rad(c_params.value);
				break;
			case 2:
				targ_ang_rad.theta2 = deg_to_rad(c_params.value);
				break;
			default:
				UART_send("Unrecognized command or value.");
				break;
			}

			// Send target angle
			while (1) {
				osMutexWait(arm_coord_mutex, osWaitForever);
				if (!next_coord_set) {

					// Set angle
					target_angles.theta0 = targ_ang_rad.theta0;
					target_angles.theta1 = targ_ang_rad.theta1;
					target_angles.theta2 = targ_ang_rad.theta2;

					// Set display message
					sprintf(target_display, "s%d angle %d deg          ", c_params.device_id, (int16_t) c_params.value);

					// Reset next coordinate flag, so that other processes can use it
					next_coord_set = 1;

					// Release mutex and break out of loop
					osMutexRelease(arm_coord_mutex);
					break;
				}
				osMutexRelease(arm_coord_mutex);
				osDelay(10);
			}

			// Set flag to one movement only
			end_moving = 1;

			// Launch process to set pulse
		    osThreadDef(SET_ANGLE, set_angle_thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 10);
		    osThreadCreate (osThread(SET_ANGLE), NULL);

			UART_send("Target angle sent.");

		}
		break;

	case POSITION:

		// If manual control is on, turn it off
		if (adc_on) {
			stop_adc_thread();
			UART_send("Manual control ended, ADC terminated.");
		}

		// Read in xyz values and set pwm pulse
		while(1) {
			osMutexWait(arm_coord_mutex, osWaitForever);
			if (!next_coord_set) {

				// Set xyz coordinates
				target_xyz.x = (double) c_params.value_x;
				target_xyz.y = (double) c_params.value_y;
				target_xyz.z = (double) c_params.value_z;

				// Set display message
				sprintf(target_display, "X: %3d  Y: %3d  Z: %3d   ", c_params.value_x, c_params.value_y, c_params.value_z);

				next_coord_set = 1;
				osMutexRelease(arm_coord_mutex);
				break;
			}
			osMutexRelease(arm_coord_mutex);
			osDelay(10);
		}

		// Set flag to one movement only
		end_moving = 1;

		// Launch process to set pulse
	    osThreadDef(SET_POSITION, set_position_thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 10);
	    osThreadCreate (osThread(SET_POSITION), NULL);

		UART_send("Target position sent.");
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

		// If manual control is on, turn it off
		if (adc_on) {
			stop_adc_thread();
			UART_send("Manual control ended, ADC terminated.");
		}

		if (c_params.value > 0) {
			start_demo();
			UART_send("Demo is on.");
		} else {
			stop_demo();
			UART_send("Demo is turned off.");
		}
		break;

	case FILE_NAME:
		break;

	case NO_ATTRIB:
		break;
	}
	return;
}

void execute_file(void)
{
	// If manual control is on, turn it off
	if (adc_on) {
		stop_adc_thread();
		UART_send("Manual control ended, ADC terminated.");
	}

	log_msg(DEBUG, "execute file: ");
	log_msg(DEBUG, c_params.file_name);
	log_msg(DEBUG, "\n");

	// Launch G-code reader with the given file name
	osThreadDef(FILE_READ, file_reader_thread, osPriorityAboveNormal, 0, configMINIMAL_STACK_SIZE * 10);
	osThreadCreate (osThread(FILE_READ), c_params.file_name);

	UART_send("G-code reader started.");
	log_msg(USER, "G-code reader started.\n");
	return;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	// This interrupt is fired when we receive a single character via USART1

	// Copy character into the circular buffer
	*(RX_buffer.write_p) = char_buff;

	// Increase write pointer value;
	RX_buffer.write_p++;

	// Loop write pointer at the end of buffer
	if (RX_buffer.write_p > RX_buffer.tail_p) {
		RX_buffer.write_p = RX_buffer.head_p;
	}

	// Increase commands in queue counter when we received LF character
	if (char_buff == '\n') {
		command_in++;
	}

	// Re-enable the interrupt
	HAL_UART_Receive_IT(huart, &char_buff, 1);

	return;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
