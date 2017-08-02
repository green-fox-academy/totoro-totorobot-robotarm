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
	uart_handle.Instance        = USARTx;
	uart_handle.Init.BaudRate   = 115200;
	uart_handle.Init.WordLength = UART_WORDLENGTH_8B;
	uart_handle.Init.StopBits   = UART_STOPBITS_1;
	uart_handle.Init.Parity     = UART_PARITY_NONE;
	uart_handle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	uart_handle.Init.Mode       = UART_MODE_TX_RX;


	// Configure COM1 as USART
	BSP_COM_Init(COM1, &uart_handle);

/*
	// Some error handling
	if (HAL_UART_DeInit(&uart_handle) != HAL_OK) {
		UART_Error_Handler();


	}

	if (HAL_UART_Init(&uart_handle) != HAL_OK) {
		UART_Error_Handler();
	}
*/
	rx_complete = 0;
	rx_index = 0;

	uart_ready = RESET;

	return;



}

void send_string(void)
{
	// String to send
	char string[] = "Hello through UART function!\r\n";
	int string_length = strlen(string);

	if (HAL_UART_Transmit(&uart_handle, (uint8_t*) string, string_length, 100)!= HAL_OK) {
		UART_Error_Handler();
	}

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

//Interrupt callback routine
/*
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	// Check for current UART
	if (huart->Instance == USARTx) {

		if (rx_index == 0) {
			// TODO zero out buffer if needed
		}

		// Unless we receive an LF save the received char in the receive buffer
		if (rx_char[0] != '\n') {
			RX_buffer[rx_index++] = rx_char[0];

		// Receive is complete, data is ready to read
		} else {
			RX_buffer[rx_index] = 0;	// add string terminator
			rx_index = 0;
			rx_complete = 1;

			if (debug) {
				LCD_UsrLog((char*) "UART RX: ");
				LCD_UsrLog((char*) RX_buffer);
				LCD_UsrLog((char*) "\n");
			}
		}

		// Activate UART receive interrupt every time
		HAL_UART_Receive_IT(&uart_handle, rx_char, 1);
	}
	return;
}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
