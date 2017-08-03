/* Includes ------------------------------------------------------------------*/
#include "uart.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


// Init the UART peripherial
/* Put the USART peripheral in the Asynchronous mode (UART Mode) */
/* UART configured as follows:
  - Word Length = 8 Bits
  - Stop Bit = One Stop bit
  - Parity = None
  - BaudRate = 115200 baud
  - Hardware flow control disabled (RTS and CTS signals) */

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
	HAL_UART_Transmit(&uart_handle, (uint8_t*) buffer, buffer_len, timeout);

	if (debug) {
		LCD_UsrLog((char*) "UART TX: ");
		LCD_UsrLog((char*) buffer);
		LCD_UsrLog((char*) "\n");
	}

	return;
}

void UART_send_help(void)
{
	UART_send("*** Greetings from TotoRobot! ***\r\n");
	UART_send("\r\n");
	UART_send("Commands:\r\n");
	UART_send("get s<X> pwm            - Get servo current pwm\r\n");
	UART_send("get s<X> angle          - Get servo current angle\r\n");
	UART_send("get r pos               - Get robot arm current xyz coordinates\r\n");
	UART_send("set s<X> pwm <value>    - Get servo current pwm\r\n");
	UART_send("set s<X> angle <value>  - Get servo current angle\r\n");
	UART_send("set r pos <x,y,z>       - Get robot arm current xyz coordinates\r\n");
	UART_send("\r\n");
	UART_send("Always terminate commands with LF!\r\n");

	return;
}

void mail_rx_msg(void)
{
	LCD_UsrLog((char*) RX_buffer);
	LCD_UsrLog((char*) "\n");

	return;
}

void UART_rx_thread(void const * argument)
{
	uart_init();

	uint32_t timeout = 100;
	uint8_t rx_data[10];
	uint8_t rx_index = 0;

	rx_complete = 0;

	strcpy((char*) RX_buffer, "hello world!");

	mail_rx_msg();
	// send_help();

	while(1) {

		// UART polling mode
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
				LCD_UsrLog((char*) "RX:");
				LCD_UsrLog((char*) RX_buffer);
				LCD_UsrLog((char*) "\n");
			}

			// Process command
			process_command();

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
	// Copy command from UART RX buffer
	char command[RXBUFFERSIZE];
	strcpy(command, (char*) RX_buffer);

	char* s = strtok(command, " ");

	// Help
	if ((s[0] == 'h') || (s[0] == 'H')) {
		UART_send_help();

	// Get
	} else if (strcmp(s, "get") == 0) {

		s = strtok(NULL, " ");
		switch (s[0]) {

		// Servo
		case 's':
			if (strlen(s) == 2) {
				uint8_t servo_id = s[1] - 48;
				if (servo_id < SERVOS) {
					UART_send("s0 pwm: 2000\r\n");
				} else {
					UART_send("Unrecognized servo id\r\n");
				}
			} else {
				UART_send("Unrecognized command\r\n");
			}
			break;

		// Robot arm
		case 'r':
			if (strlen(s) == 1) {
				// TODO execute set command

			} else {
				UART_send("Unrecognized command\r\n");
			}
			break;



			break;

		// Error
		default:
			UART_send("Unrecognized command\r\n");
			break;
		}


	// Set
	} else if (strcmp(s, "set") == 0) {

		s = strtok(NULL, " ");
		switch (s[0]) {

		// Servo
		case 's':
			if (strlen(s) == 2) {
				uint8_t servo_id = s[1];
				if (servo_id < SERVOS) {
					// TODO execute set command
				} else {
					UART_send("Unrecognized servo id\r\n");
				}
			} else {
				UART_send("Unrecognized command\r\n");
			}
			break;

		// Robot arm
		case 'r':
			if (strlen(s) == 1) {
				// TODO execute set command

			} else {
				UART_send("Unrecognized command\r\n");
			}
			break;



			break;

		// Error
		default:
			UART_send("Unrecognized command\r\n");
			break;
		}


	// Error message
	} else {
		UART_send("Unrecognized command\r\n");
	}
	return;
}




/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
