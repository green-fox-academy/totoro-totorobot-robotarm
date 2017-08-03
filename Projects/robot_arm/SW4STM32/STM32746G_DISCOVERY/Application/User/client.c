#include "client.h"
/* Includes ------------------------------------------------------------------*/
#include "stm32746g_discovery_ts.h"
#include "lcd_log.h"
#include "cmsis_os.h"
#include "app_ethernet.h"
#include "lwip/sockets.h"
#include "stm32746g_discovery_ts.h"
#include <string.h>
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CLIENT_SERVER_IP			"216.239.32.15"
#define UDP_SERVER_PORT					80

typedef struct
	{

		unsigned li   : 2;       // Only two bits. Leap indicator.
		unsigned vn   : 3;       // Only three bits. Version number of the protocol.
		unsigned mode : 3;       // Only three bits. Mode. Client will pick mode 3 for client.

		uint8_t stratum;         // Eight bits. Stratum level of the local clock.
		uint8_t poll;            // Eight bits. Maximum interval between successive messages.
		uint8_t precision;       // Eight bits. Precision of the local clock.

		uint32_t rootDelay;      // 32 bits. Total round trip delay time.
		uint32_t rootDispersion; // 32 bits. Max error aloud from primary clock source.
		uint32_t refId;          // 32 bits. Reference clock identifier.

		uint32_t refTm_s;        // 32 bits. Reference time-stamp seconds.
		uint32_t refTm_f;        // 32 bits. Reference time-stamp fraction of a second.

		uint32_t origTm_s;       // 32 bits. Originate time-stamp seconds.
		uint32_t origTm_f;       // 32 bits. Originate time-stamp fraction of a second.

		uint32_t rxTm_s;         // 32 bits. Received time-stamp seconds.
		uint32_t rxTm_f;         // 32 bits. Received time-stamp fraction of a second.

		uint32_t txTm_s;         // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
		uint32_t txTm_f;         // 32 bits. Transmit time-stamp fraction of a second.

	} ntp_packet;                 // Total: 384 bits or 48 bytes.

	// Create and zero out the packet. All 48 bytes worth.

	ntp_packet packet = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

void udp_client_thread(void const *argument)
{
	udp_client_ready = 0;
	udp_send_allowed = 1;

	// Create a new socket to listen for client connections.
	int udp_client_socket;

	udp_client_socket = lwip_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (udp_client_socket < 0) {
		LCD_ErrLog((char*) "Cannot create UDP client socket.\n");
		LCD_ErrLog((char*) "Closing application.\n");
		osThreadTerminate(NULL);
	} else {
		LCD_UsrLog((char*) "UDP client socket is up.\n");
	}

	// Create client address structure
		struct sockaddr_in udp_client_addr;
		udp_client_addr.sin_family = AF_INET;
		int udp_client_addr_size = sizeof(udp_client_addr);

		// Create remote server address structure
		struct sockaddr_in udp_remote_addr;
		udp_remote_addr.sin_family = AF_INET;
		udp_remote_addr.sin_addr.s_addr = INADDR_BROADCAST;
		udp_remote_addr.sin_port = htons(UDP_SERVER_PORT);

		memset( &packet, 0, sizeof( ntp_packet ) );

		 // Set socket to broadcast
		int opt_val = 1;
		int opt_len = sizeof(int);
		int set_broadcast = lwip_setsockopt(udp_client_socket, SOL_SOCKET,
										    SO_BROADCAST, &opt_val, (socklen_t) opt_len);
		if (set_broadcast < 0) {
				LCD_ErrLog((char*) "Cannot set UDP client to broadcast.\n");
				LCD_ErrLog((char*) "Closing application.\n");
				lwip_close(udp_client_socket);
				osThreadTerminate(NULL);
			} else {
				LCD_UsrLog((char*) "UDP client is ready to broadcast.\n");
			}

		udp_client_ready = 1;

		LCD_UsrLog((char*) "UDP client is ready.\n");
		char buff[128];
		char message[] = {010,0,0,0,0,0,0,0,0};

		LCD_UsrLog("%s\n", message);

		LCD_UsrLog("Socket client - startup...\n");
		LCD_UsrLog("Socket client - waiting for IP address...\n");

		// Wait for an IP address
		while (!is_ip_ok())
			osDelay(10);

		// Try to connect to the server
		// Send udp package
		sendto(udp_client_socket, message, strlen(message),
								 0, (struct sockaddr*) &udp_remote_addr, sizeof(udp_remote_addr));
		LCD_UsrLog("Socket client - data sent\n");

		int recv_bytes = recvfrom(udp_client_socket, buff, sizeof(buff),
								   0, (struct sockaddr*) &udp_client_addr,
								   (socklen_t*) &udp_client_addr_size);
		if (recv_bytes > 0) {
			LCD_UsrLog("Socket client - data received: \n");
			buff[recv_bytes] = '\0';
			LCD_UsrLog(buff);
			LCD_UsrLog("\n");
		}


	    // Close socket
	    lwip_close(udp_client_socket);

	    // Close thread
	    while (1) {
	    	osThreadTerminate(NULL);
	    }
}
