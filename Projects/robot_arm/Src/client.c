#include "client.h"
/* Includes ------------------------------------------------------------------*/
#include "lcd_log.h"
#include "cmsis_os.h"
#include "app_ethernet.h"
#include "lwip/sockets.h"
#include "stm32746g_discovery_ts.h"
#include "stm32f7xx_hal_gpio.h"
#include "stm32f7xx_hal_adc.h"
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CLIENT_SERVER_IP			"10.27.6.105"
#define SERVER_PORT					8888

int connect_to_server(int *client_sock, uint16_t server_port, char *server_ip)
{
	// Creating client socket
	(*client_sock) = socket(AF_INET, SOCK_STREAM, 0);
	if (*client_sock < 0) {
		LCD_ErrLog("Socket client - can't create socket\n");
		return -1;
	}

	// Creating server address structure
	struct sockaddr_in addr_in;
	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons(server_port);
	addr_in.sin_addr.s_addr = inet_addr(server_ip);

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

void socket_client_thread(void const *argument)
{
	char buff[128];
	uint8_t message[] = {"asdasd"};
	LCD_UsrLog("%s\n", message);

	LCD_UsrLog("Socket client - startup...\n");
	LCD_UsrLog("Socket client - waiting for IP address...\n");

	// Wait for an IP address
	while (!is_ip_ok())
		osDelay(10);

	int client_socket;

	// Try to connect to the server
	if (connect_to_server(&client_socket, SERVER_PORT, CLIENT_SERVER_IP) == 0)
	{
		sprintf(buff, "%s", message);
		int sent_bytes = send(client_socket, buff, strlen(buff), 0);
		if (sent_bytes == 1) {

			LCD_UsrLog("Socket client - data sent\n");

			int recv_bytes = recv(client_socket, buff, 127, 0);
			if (recv_bytes >= 0) {
				LCD_UsrLog("Socket client - data received: ");
				buff[recv_bytes] = 0;
				LCD_UsrLog(buff);
				LCD_UsrLog("\n");
			}
		}
		closesocket(client_socket);
	}

	LCD_UsrLog("Socket client - terminating...\n");

	while (1) {
		osThreadTerminate(NULL);
	}
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
