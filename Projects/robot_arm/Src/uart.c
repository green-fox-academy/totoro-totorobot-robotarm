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

void UART_send(char* buffer, uint16_t buffer_len)
{
	uint32_t timeout = 100;
	HAL_UART_Transmit(&uart_handle, (uint8_t*) buffer, buffer_len, timeout);

	if (debug) {
		LCD_UsrLog((char*) "UART TX: ");
		LCD_UsrLog((char*) buffer);
		LCD_UsrLog((char*) "\n");
	}

	return;
}

void send_help(void)
{
	UART_send("Greetings from TotoRobot!\r\n", 28);
	UART_send("\r\n", 2);
	UART_send("Available commands:\r\n", 21);
	UART_send("set -c <x, y, z>         Set xyz position\r\n", 44);
	UART_send("get -c                   Get current xyz position\r\n", 52);
	UART_send("set -s <servo_no> <pwm>  Set pwm value\r\n", 40);
	UART_send("get -s <servo_no>        Get current pwm value\r\n", 49);
	UART_send("Always terminate commands with LF\r\n", 35);

	return;
}

void mail_rx_msg(void)
{

}

void UART_rx_thread(void const * argument)
{
	uart_init();

	uint32_t timeout = 1000;
	uint8_t rx_data[1];
	uint8_t rx_index = 0;

	rx_complete = 0;

	send_help();

	while(1) {

		// UART polling mode receives one character at a time
		if(HAL_UART_Receive(&uart_handle, rx_data, 1, timeout) != HAL_OK) {
			UART_Error_Handler();
		}


		// Add character to RX_buffer, if LF, then close string and use it
		if (rx_data[0] == '\n') {
			RX_buffer[rx_index] = '\0';
			rx_index = 0;
			rx_complete = 1;
			mail_rx_msg();
		} else {
			RX_buffer[rx_index++] = rx_data;
		}

		osDelay(10);
	}

	while (1) {
		if (debug) {
			LCD_ErrLog((char*) "UART RX thread terminating\n");
		}
		osThreadTerminate(NULL);
	}

}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
