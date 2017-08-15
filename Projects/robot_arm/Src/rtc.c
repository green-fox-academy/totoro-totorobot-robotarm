#include "rtc.h"

void ntp_client_thread(void const *argument)
{
	typedef short WORD;

	time_t ntp_time;

	RTC_DateTypeDef rtc_date;
	RTC_TimeTypeDef rtc_time;

	int sockfd;
	int n;             // Socket file descriptor and the n return result from writing/reading from the socket.
	int portno = 123;  // NTP UDP port number.

   // Structure that defines the 48 byte NTP packet protocol.
	typedef struct
	{
		unsigned li   : 2;          // Only two bits. Leap indicator.
		unsigned vn   : 3;          // Only three bits. Version number of the protocol.
		unsigned mode : 3;          // Only three bits. Mode. Client will pick mode 3 for client.

		uint8_t stratum;            // Eight bits. Stratum level of the local clock.
		uint8_t poll;               // Eight bits. Maximum interval between successive messages.
		uint8_t precision;          // Eight bits. Precision of the local clock.

		uint32_t root_delay;        // 32 bits. Total round trip delay time.
		uint32_t root_dispersion;   // 32 bits. Max error aloud from primary clock source.
		uint32_t ref_id;            // 32 bits. Reference clock identifier.

		uint32_t ref_tm_s;          // 32 bits. Reference time-stamp seconds.
		uint32_t ref_tm_f;          // 32 bits. Reference time-stamp fraction of a second.

		uint32_t orig_tm_s;         // 32 bits. Originate time-stamp seconds.
		uint32_t orig_tm_f;         // 32 bits. Originate time-stamp fraction of a second.

		uint32_t rx_tm_s;           // 32 bits. Received time-stamp seconds.
		uint32_t rx_tm_f;           // 32 bits. Received time-stamp fraction of a second.

		uint32_t tx_tm_s;           // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
		uint32_t tx_tm_f;           // 32 bits. Transmit time-stamp fraction of a second.

	} ntp_packet;                   // Total: 384 bits or 48 bytes.

	// Create and zero out the packet. All 48 bytes worth.
	ntp_packet packet = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	// Set the first byte's bits to 00,011,011 for li = 0,vn = 3,and mode = 3. The rest will be left set to zero.
	*((char*) &packet + 0) = 0x1B; // Represents 27 in base 10 or 00011011 in base 2.

	// Wait for IP address
	while (!is_ip_ok()) {
		osDelay(100);
	}

	// Create a UDP socket, set IP address, set the port number,
	// connect to the server, send the packet, and then read in the return packet.
	struct sockaddr_in  serv_addr;  // Server address data structure.
	sockfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);

	if (sockfd < 0) {
		log_msg(ERROR, "UDP Socket error in ntp_clien_thread\n");
	}

	// Zero out the server address structure.
	memset(&serv_addr,0,sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;

	// Convert the port number integer to network big-endian style and save it to the server address structure.
	serv_addr.sin_port = htons((unsigned short)portno);

	// Call up the server using its IP address and port number.
	if (connect(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0) {
		log_msg(ERROR, "NTP server unreachable.\n");
	}

	serv_addr.sin_addr.s_addr = inet_addr(NTP_SERVER_IP);

	// Convert the port number integer to network big-endian style and save it to the server address structure.
	serv_addr.sin_port = htons((WORD)portno);

	// Send it the NTP packet it wants. If n == -1, it failed.
	n = sendto(sockfd,(char*)&packet,sizeof(ntp_packet),0,(struct sockaddr*)&serv_addr,sizeof(serv_addr));

	if (n < 0) {
	   log_msg(ERROR, "Could not send NTP packet to server.\n");
	}

	// Wait and receive the packet back from the server. If n == -1, it failed.
	n = recv(sockfd,(char*)&packet,sizeof(ntp_packet),0);

	if (n < 0) {
	   log_msg(ERROR, "Did not receive NTP packet back from server.\n");
	}

	// These two fields contain the time-stamp seconds as the packet left the NTP server.
	// The number of seconds correspond to the seconds passed since 1900.
	// ntohl() converts the bit/byte order from the network's to host's "endianness".
	packet.tx_tm_s = ntohl(packet.tx_tm_s); // Time-stamp seconds.
	packet.tx_tm_f = ntohl(packet.tx_tm_f); // Time-stamp fraction of a second.

	// Extract the 32 bits that represent the time-stamp seconds (since NTP epoch) from when the packet left the server.
	// Subtract 70 years worth of seconds from the seconds since 1900.
	// This leaves the seconds since the UNIX epoch of 1970.
	// (1900)------------------(1970)**************************************(Time Packet Left the Server)
	ntp_time = (time_t)(packet.tx_tm_s - NTP_TIMESTAMP_DELTA);

	// Print the time we got from the server,accounting for local timezone and conversion from UTC time.
	char tmp[100];
	sprintf(tmp, "NTP Time: %s", ctime((const time_t*)&ntp_time));
	log_msg(DEBUG, tmp);

	// Close UDP socket
	closesocket(sockfd);

	// Initialize the on-board Real Time Clock
	rtc_init();

	// Set the RTC time
	rtc_set(&ntp_time);

	// Get the current RTC time
	HAL_RTC_GetTime(&rtc, &rtc_time, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&rtc, &rtc_date, RTC_FORMAT_BIN);

	// Print the current time
	sprintf(tmp, "RTC time: %d-%02d-%02d %02d:%02d:%02d.%04lu\n",
			rtc_date.Year + 1900, rtc_date.Month + 1, rtc_date.Date,
			rtc_time.Hours, rtc_time.Minutes, rtc_time.Seconds, rtc_time.SubSeconds);
	log_msg(USER, tmp);

	// Exit thread
	while (1) {
		log_msg(USER, "NTP client thread terminated.\n");
		osThreadTerminate(NULL);
	}
}

void rtc_set(time_t* ntp_time)
{
	RTC_DateTypeDef rtc_in_date;
	RTC_TimeTypeDef rtc_in_time;

	// Convert seconds data to time structure
	struct tm in_time = *(localtime(ntp_time));

	// Feed time structure into RTC time structure
	rtc_in_time.Hours = in_time.tm_hour;
	rtc_in_time.Minutes = in_time.tm_min;
	rtc_in_time.Seconds = in_time.tm_sec;
	rtc_in_time.TimeFormat = RTC_HOURFORMAT_24;
	rtc_in_time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	rtc_in_time.StoreOperation = RTC_STOREOPERATION_RESET;

	rtc_in_date.WeekDay = in_time.tm_wday;
	rtc_in_date.Month = in_time.tm_mon;
	rtc_in_date.Date = in_time.tm_mday;
	rtc_in_date.Year = in_time.tm_year;

	// Set RTC time
	HAL_RTC_SetTime(&rtc, &rtc_in_time, RTC_FORMAT_BIN);
	HAL_RTC_SetDate(&rtc, &rtc_in_date, RTC_FORMAT_BIN);

	return;
}

void rtc_init(void)
{
	// Enable RTC clock power
	__HAL_RCC_PWR_CLK_ENABLE();

	// Enable RTC backup register
	HAL_PWR_EnableBkUpAccess();

	// Set RTC clock source
	__HAL_RCC_RTC_CONFIG(RCC_RTCCLKSOURCE_HSE_DIV25);

	// Enable RTC clock
	__HAL_RCC_RTC_ENABLE();

	// Configure the RTC peripheral
	rtc.Instance = RTC;
	rtc.Init.HourFormat = RTC_HOURFORMAT_24;
	rtc.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
	rtc.Init.SynchPrediv = RTC_SYNCH_PREDIV;
	rtc.Init.OutPut = RTC_OUTPUT_DISABLE;
	rtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	rtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

	if (HAL_RTC_Init(&rtc) != HAL_OK) {
		log_msg(ERROR, "RTC initialization failed.\n");
	} else {
		log_msg(USER, "RTC initialization done.\n");
	}
}
