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


	// Some error handling
	if (HAL_UART_DeInit(&uart_handle) != HAL_OK) {
		UART_Error_Handler();


	}

	if (HAL_UART_Init(&uart_handle) != HAL_OK) {
		UART_Error_Handler();
	}




	return;



}

void send_string(void)
{
	// String to send
	char string[] = "Hello through UART function!\r\n";
	int string_length = strlen(string);

	if(HAL_UART_Transmit(&uart_handle, (uint8_t*) string, string_length, 100)!= HAL_OK) {
		UART_Error_Handler();
	}

	return;
}

void UART_Error_Handler(void){

}

void UART_send(char* buffer, uint32_t buffer_len)
{
	uint32_t timeout = 100;
	HAL_UART_Transmit(&uart_handle, buffer, buffer_len, timeout);

	return;
}


/**
* @brief  Retargets the C library printf function to the USART.
* @param  None
* @retval None
*/
/*
PUTCHAR_PROTOTYPE
{
*/
	/* Place your implementation of fputc here */
	/* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */

/*HAL_UART_Transmit(&uart_handle, (uint8_t *)&ch, 1, 0xFFFF);

	return ch;
}

*/


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
