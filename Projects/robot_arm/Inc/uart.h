#ifndef __UART_H_
#define __UART_H_

#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "lcd_log.h"
#include <string.h>

/* Definition for USARTx clock resources */
#define USARTx                           USART6
#define USARTx_CLK_ENABLE()              __USART6_CLK_ENABLE()
#define USARTx_RX_GPIO_CLK_ENABLE()      __GPIOC_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __GPIOC_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __USART6_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __USART6_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_6
#define USARTx_TX_GPIO_PORT              GPIOC
#define USARTx_TX_AF                     GPIO_AF8_USART6
#define USARTx_RX_PIN                    GPIO_PIN_7
#define USARTx_RX_GPIO_PORT              GPIOC
#define USARTx_RX_AF                     GPIO_AF8_USART6

/* Size of buffers */
#define TXBUFFERSIZE                      100				// transmit
#define RXBUFFERSIZE                      TXBUFFERSIZE		// reveive

UART_HandleTypeDef uart_handle;
__IO ITStatus uart_ready;


uint8_t TX_buffer[TXBUFFERSIZE];
uint8_t RX_buffer[RXBUFFERSIZE];

uint8_t rx_index;
uint8_t rx_char[2];
uint8_t rx_complete;



extern uint8_t debug;

void uart_init(void);
void send_string(void);
void UART_Error_Handler(void);
void UART_send(char* buffer, uint16_t buffer_len);


#endif /* __UART_H_ */
