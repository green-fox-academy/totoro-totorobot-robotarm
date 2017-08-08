#ifndef __UART_H_
#define __UART_H_

#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "lcd_log.h"
#include "servo_control.h"
#include <string.h>
#include <stdlib.h>

#define USARTx				USART1
#define TXBUFFERSIZE        100				// transmit
#define RXBUFFERSIZE        TXBUFFERSIZE	// receive

typedef enum {NO_COMMAND, HELP, GET_VALUE, SET_VALUE} uart_command_t;
typedef enum {NO_ATTRIB, PULSE, ANGLE, POSITION, MANUAL_CONTROL} uart_attrib_t;

typedef struct {
	uart_command_t command;
	uart_attrib_t attrib;
	uint8_t device_id;
	uint16_t value;
	uint16_t value_x;
	uint16_t value_y;
	uint16_t value_z;
	uint8_t error;
} uart_command_struct_t;

uart_command_struct_t c_params;
UART_HandleTypeDef uart_handle;
extern uint8_t adc_on;

uint8_t TX_buffer[TXBUFFERSIZE];
uint8_t RX_buffer[RXBUFFERSIZE];

extern uint8_t debug;

void uart_init(void);
void UART_Error_Handler(void);
void UART_send(char* buffer);
void UART_send_help(void);
void UART_rx_thread(void const * argument);
void process_command(void);
void execute_command(void);
uint8_t verify_coordinates(uint16_t x, uint16_t y, uint16_t z);
uint8_t verify_pulse(uint32_t pulse);
uint8_t verify_angle(uint32_t angle);
void set_value(void);
void UART_send_settings(void);
extern void start_adc_thread(void);
extern void stop_adc_thread(void);
void lcd_data_display_start(void);
void lcd_data_display_stop(void);


#endif /* __UART_H_ */
