/* Includes ------------------------------------------------------------------*/
#include "robot_arm.h"
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_ts.h"
#include "stm32746g_discovery_lcd.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
#define TS_CLICK_THRESHOLD	3
uint8_t HID_Buffer[4];

typedef struct {
	int32_t x;
	int32_t y;
} coordinate_t;

uint16_t test_port = 54545;
char *test_ip = "10.27.6.52";

void servo_control_thread(void const * argument)
{
	debug = 1;

	LCD_UsrLog((char*) "Servo control thread started\n");

	pwm_init();
	adc_init();
	position = 0;

	while (1) {
		pwm_set_duty_from_adc();
		osDelay(10);
	}

	while (1) {
		osThreadTerminate(NULL);
    }
}

/**
  * @brief  Initializes TIM1 as PWM source
  * @param  None
  * @retval None
  */
void pwm_init(void)
{
	// TIM3 init as PWM
	pwm_handle.Instance = TIM3;
	pwm_handle.State = HAL_TIM_STATE_RESET;
	pwm_handle.Channel = HAL_TIM_ACTIVE_CHANNEL_1;
	pwm_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	pwm_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	pwm_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
	pwm_handle.Init.Period = 0xFFFF;
	pwm_handle.Init.Prescaler = 30;
	HAL_TIM_PWM_Init(&pwm_handle);

	pwm_oc_init.OCFastMode = TIM_OCFAST_DISABLE;
	pwm_oc_init.OCIdleState = TIM_OCIDLESTATE_RESET;
	pwm_oc_init.OCMode = TIM_OCMODE_PWM1;
	pwm_oc_init.OCPolarity = TIM_OCPOLARITY_LOW;
	pwm_oc_init.Pulse = 0x7FFF;
	HAL_TIM_PWM_ConfigChannel(&pwm_handle, &pwm_oc_init, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&pwm_handle, TIM_CHANNEL_1);
}

/**
  * @brief  Sets the duty cycle of TIM3 CH1
  * @param  duty - duty cycle to set (5.0-10.0)
  * @retval None
  */

void pwm_set_duty_from_adc(void)
{
	pwm_set_duty(get_degrees());

	return;
}


void pwm_set_duty(uint8_t rot_degree)
{
	// Calculate pulse width
	int min_duty = (65536 * MIN_POS_DUTY_CYCLE) / 100;
	int max_duty = (65536 * MAX_POS_DUTY_CYCLE) / 100;

	// Set pulse width
	uint16_t pulse = min_duty + ((max_duty - min_duty) * rot_degree) / 180;
	pwm_oc_init.Pulse = pulse;
	HAL_TIM_PWM_ConfigChannel(&pwm_handle, &pwm_oc_init, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&pwm_handle, TIM_CHANNEL_1);

	if (debug) {
		char tmp[20];
		sprintf(tmp, "Angle: %2d - Pulse: %5d \n", rot_degree, pulse);
		LCD_UsrLog(tmp);
	}

	return;
}

/**
  * @brief  Initializes ADC3 to measure voltage on CH0
  * @param  None
  * @retval None
  */
void adc_init()
{
	adc_handle.State = HAL_ADC_STATE_RESET;
	adc_handle.Instance = ADC3;
	adc_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	adc_handle.Init.Resolution = ADC_RESOLUTION_12B;
	adc_handle.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	adc_handle.Init.DMAContinuousRequests = DISABLE;
	adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	adc_handle.Init.ContinuousConvMode = DISABLE;
	adc_handle.Init.DiscontinuousConvMode = DISABLE;
	adc_handle.Init.ScanConvMode = DISABLE;
	HAL_ADC_Init(&adc_handle);

	adc_ch_conf.Channel = ADC_CHANNEL_0;
	adc_ch_conf.Offset = 0;
	adc_ch_conf.Rank = 1;
	adc_ch_conf.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	HAL_ADC_ConfigChannel(&adc_handle, &adc_ch_conf);
}

/**
  * @brief  Measures the voltage with ADC3 on CH0
  * @param  None
  * @retval Measured value (0-4095)
  */
uint16_t adc_measure(void)
{
	HAL_ADC_Start(&adc_handle);
	HAL_ADC_PollForConversion(&adc_handle, HAL_MAX_DELAY);
	uint16_t value = HAL_ADC_GetValue(&adc_handle);
	HAL_ADC_Stop(&adc_handle);

	if (debug) {
		char tmp[20];
		sprintf(tmp, "ADC value: %d\n", value);
		LCD_UsrLog(tmp);
	}

	return value;
}

/**
  * @brief  Calculates degree of rotation from ADC measurement
  * @param  None
  * @retval Degree (0-180)
  */
uint8_t get_degrees(void)
{
	uint16_t adc_value = adc_measure();
	uint8_t degrees = (uint32_t) ( adc_value * (MAX_DEGREE - MIN_DEGREE)) / (MAX_ADC_VALUE - MIN_ADC_VALUE);

	return degrees;
}


void udp_client_thread(void const *argument)
{
   int      sockfd;
   int      n;             // Socket file descriptor and the n return result from writing/reading from the socket.
   int      portno = 123;  // NTP UDP port number.

   // Structure that defines the 48 byte NTP packet protocol.
   // Check TWICE size of fields !!
typedef struct
{
	unsigned li   : 2;          // Only two bits. Leap indicator.
	unsigned vn   : 3;          // Only three bits. Version number of the protocol.
	unsigned mode : 3;          // Only three bits. Mode. Client will pick mode 3 for client.

	uint8_t stratum;           // Eight bits. Stratum level of the local clock.
	uint8_t poll;              // Eight bits. Maximum interval between successive messages.
	uint8_t precision;         // Eight bits. Precision of the local clock.

	uint32_t rootDelay;        // 32 bits. Total round trip delay time.
	uint32_t rootDispersion;   // 32 bits. Max error aloud from primary clock source.
	uint32_t refId;            // 32 bits. Reference clock identifier.

	uint32_t refTm_s;          // 32 bits. Reference time-stamp seconds.
	uint32_t refTm_f;          // 32 bits. Reference time-stamp fraction of a second.

	uint32_t origTm_s;         // 32 bits. Originate time-stamp seconds.
	uint32_t origTm_f;         // 32 bits. Originate time-stamp fraction of a second.

	uint32_t rxTm_s;           // 32 bits. Received time-stamp seconds.
	uint32_t rxTm_f;           // 32 bits. Received time-stamp fraction of a second.

	uint32_t txTm_s;           // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
	uint32_t txTm_f;           // 32 bits. Transmit time-stamp fraction of a second.

}ntp_packet;                         // Total: 384 bits or 48 bytes.

	// Create and zero out the packet. All 48 bytes worth.
	ntp_packet packet = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	// Set the first byte's bits to 00,011,011 for li = 0,vn = 3,and mode = 3. The rest will be left set to zero.
	*((char*)&packet + 0) = 0x1B; // Represents 27 in base 10 or 00011011 in base 2.

	// Create a UDP socket, convert the host-name to an IP address, set the port number,
	// connect to the server,send the packet,and then read in the return packet.
	struct sockaddr_in  serv_addr;  // Server address data structure.
	//struct hostent *server;     // Server data structure.

	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // Create a UDP socket.

	if (sockfd < 0) {
		LCD_UsrLog("UDP Socket error");
	}

	// Zero out the server address structure.
	memset(&serv_addr,0,sizeof(serv_addr));

	typedef short WORD;

	serv_addr.sin_family = AF_INET;

	// Convert the port number integer to network big-endian style and save it to the server address structure.
	serv_addr.sin_port = htons((unsigned short)portno);

	// Call up the server using its IP address and port number.
	if (connect(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0) {
		LCD_UsrLog("error");
	}

	serv_addr.sin_addr.s_addr = inet_addr(server_ip);

	// Convert the port number integer to network big-endian style and save it to the server address structure.
	serv_addr.sin_port = htons((WORD)portno);

	// Send it the NTP packet it wants. If n == -1, it failed.
	n = sendto(sockfd,(char*)&packet,sizeof(ntp_packet),0,(struct sockaddr*)&serv_addr,sizeof(serv_addr));

	if (n < 0) {
	   LCD_UsrLog("error");
	}

	// Wait and receive the packet back from the server. If n == -1, it failed.
	n = recv(sockfd,(char*)&packet,sizeof(ntp_packet),0);

	if (n < 0) {
	   LCD_UsrLog("error");
	}

	// These two fields contain the time-stamp seconds as the packet left the NTP server.
	// The number of seconds correspond to the seconds passed since 1900.
	// ntohl() converts the bit/byte order from the network's to host's "endianness".
	packet.txTm_s = ntohl(packet.txTm_s); // Time-stamp seconds.
	packet.txTm_f = ntohl(packet.txTm_f); // Time-stamp fraction of a second.

	// Extract the 32 bits that represent the time-stamp seconds (since NTP epoch) from when the packet left the server.
	// Subtract 70 years worth of seconds from the seconds since 1900.
	// This leaves the seconds since the UNIX epoch of 1970.
	// (1900)------------------(1970)**************************************(Time Packet Left the Server)
	txTm = (time_t)(packet.txTm_s - NTP_TIMESTAMP_DELTA);

	// Print the time we got from the server,accounting for local timezone and conversion from UTC time.
	LCD_UsrLog("Time: %s",ctime((const time_t*)&txTm));

	closesocket(sockfd);

	while (1) {
		/* Delete the Init Thread */
		osThreadTerminate(NULL);
	}
}




/**
  * @brief  Configure the current time and date.
  * @param  None
  * @retval None
  */
void rtc_init(void)
{

	__HAL_RCC_PWR_CLK_ENABLE();
	 HAL_PWR_EnableBkUpAccess();


	 __HAL_RCC_RTC_CONFIG(RCC_RTCCLKSOURCE_HSE_DIV25);

	// Enable RTC //
	__HAL_RCC_RTC_ENABLE();

	/*##-1- Configure the RTC peripheral #######################################*/
	/* Configure RTC prescaler and RTC data registers */
	/* RTC configured as follow:
	- Hour Format    = Format 24
	- Asynch Prediv  = Value according to source clock
	- Synch Prediv   = Value according to source clock
	- OutPut         = Output Disable
	- OutPutPolarity = High Polarity
	- OutPutType     = Open Drain */
	RtcHandle.Instance = RTC;
	RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
	RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
	RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
	RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
	RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

	if(HAL_RTC_Init(&RtcHandle) != HAL_OK) {
		LCD_UsrLog("RTC error: RTC initialization failed.");
	}
}


void rtc_set(void)
{
	struct tm input_time = *localtime(&txTm);

	RTC_DateTypeDef dateStruct;
	RTC_TimeTypeDef timeStruct;

	timeStruct.Hours = input_time.tm_hour;
    timeStruct.Minutes = input_time.tm_min;
    timeStruct.Seconds = input_time.tm_sec;
    timeStruct.TimeFormat = RTC_HOURFORMAT_24;
    timeStruct.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    timeStruct.StoreOperation = RTC_STOREOPERATION_RESET;

    dateStruct.WeekDay = input_time.tm_wday;
    dateStruct.Month = input_time.tm_mon;
	dateStruct.Date = input_time.tm_mday;
	dateStruct.Year = input_time.tm_year;

	HAL_RTC_SetTime(&RtcHandle, &timeStruct, RTC_FORMAT_BIN);
	HAL_RTC_SetDate(&RtcHandle, &dateStruct, RTC_FORMAT_BIN);

}

void rtc_get_time_thread(void const * argument)
{
    rtc_init();

	rtc_set();

	while(1){

	printf("RTC TIME:\n");

	RTC_DateTypeDef dateStruct;
	RTC_TimeTypeDef timeStruct;

	HAL_RTC_GetTime(&RtcHandle, &timeStruct, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&RtcHandle, &dateStruct, RTC_FORMAT_BIN);
	char text[16] = {0};
	char text_1[16] = {0};
	sprintf(text,"%.2d:%.2d:%.2d %.4lu", timeStruct.Hours, timeStruct.Minutes, timeStruct.Seconds, timeStruct.SubSeconds);
	sprintf(text_1,"%.4d-%.2d-%.2d", dateStruct.Year + 1900, dateStruct.Month + 1, dateStruct.Date);
	LCD_UsrLog("Time: %s %s\n", text_1, text);
	osDelay(30);
	}
	while (1) {
		/* Delete the Init Thread */
		osThreadTerminate(NULL);
	}
}

void touch_screen_thread(void const *argument)
{
	TS_StateTypeDef ts_state;

	uint16_t PointCount;
	pPoint Points[] = {
	{390, 120},
	{360, 105},
	{330, 90},
	{300, 75},
	{270, 60},
	{240, 45},
	};

	BSP_LCD_DisplayOn();

	coordinate_t last_ts_coord;
	last_ts_coord.x = 0;
	last_ts_coord.y = 0;

	coordinate_t first_ts_coord;
	first_ts_coord.x = 0;
	first_ts_coord.y = 0;

	uint8_t first_touch_detected_flag = 0;
	uint8_t possible_click_event = 0;
	uint8_t once_push = 0;

	/* Run Application (Interrupt mode) */
	while (1) {
		// Get touch screen state
		BSP_TS_GetState(&ts_state);

		// Reset USB HID buffer
		HID_Buffer[0] = 0;
		HID_Buffer[1] = 0;
		HID_Buffer[2] = 0;

		osDelay(10);

		if (BSP_PB_GetState(BUTTON_KEY)) {
			BSP_LCD_Clear(LCD_LOG_BACKGROUND_COLOR);
			BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
			BSP_LCD_FillRect(396, 208, 70, 50);
			BSP_LCD_SetTextColor(LCD_COLOR_DARKGREEN);
			BSP_LCD_FillRect(396, 144, 70, 50);
			BSP_LCD_SetTextColor(LCD_COLOR_ORANGE);
			BSP_LCD_FillRect(396, 80, 70, 50);
			BSP_LCD_SetTextColor(LCD_COLOR_DARKRED);
			BSP_LCD_FillRect(396, 14, 70, 50);
		}

		if (ts_state.touchDetected == 1) {

			BSP_LED_On(LED1);

			if ((396 < ts_state.touchX[0]) && (208 < ts_state.touchY[0]) && (466 > ts_state.touchX[0]) && (258 > ts_state.touchY[0])) {

				if (!once_push) {
					once_push = 1;
					// WRITE HERE THE BUTTON FUNCTION!!!
					LCD_UsrLog("4\n");
				} else {
					once_push = 0;
					osDelay(500);
				}
			}

			if ((396 < ts_state.touchX[0]) && (144 < ts_state.touchY[0]) && (466 > ts_state.touchX[0]) && (194 > ts_state.touchY[0])) {

				if (!once_push) {
					once_push = 1;
					// WRITE HERE THE BUTTON FUNCTION!!!
					LCD_UsrLog("3\n");
					BSP_LCD_Clear(LCD_LOG_BACKGROUND_COLOR);
					BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
					BSP_LCD_FillRect(396, 208, 70, 50);
					BSP_LCD_SetTextColor(LCD_COLOR_DARKGREEN);
					BSP_LCD_FillRect(396, 144, 70, 50);
					BSP_LCD_SetTextColor(LCD_COLOR_ORANGE);
					BSP_LCD_FillRect(396, 80, 70, 50);
					BSP_LCD_SetTextColor(LCD_COLOR_DARKRED);
					BSP_LCD_FillRect(396, 14, 70, 50);
				} else {
					once_push = 0;
					osDelay(500);
				}
			}

			if ((396 < ts_state.touchX[0]) && (80 < ts_state.touchY[0]) && (466 > ts_state.touchX[0]) && (130 > ts_state.touchY[0])) {

				if (!once_push) {
					once_push = 1;
					// WRITE HERE THE BUTTON FUNCTION!!!
					LCD_UsrLog("2\n");
				} else {
					once_push = 0;
					osDelay(500);
				}
			}

			if ((396 < ts_state.touchX[0]) && (14 < ts_state.touchY[0]) && (466 > ts_state.touchX[0]) && (64 > ts_state.touchY[0])) {

				if (!once_push) {
					once_push = 1;
					// WRITE HERE THE BUTTON FUNCTION!!!
					LCD_UsrLog("1\n");
				} else {
					once_push = 0;
					osDelay(500);
				}
			}

			if ((7 < ts_state.touchX[0]) && (7 < ts_state.touchY[0]) && (390 > ts_state.touchX[0]) && (265 > ts_state.touchY[0])) {
				BSP_LCD_FillCircle(ts_state.touchX[0], ts_state.touchY[0], 4);
			}

			if (!first_touch_detected_flag) {
				first_touch_detected_flag = 1;
				possible_click_event = 1;
				last_ts_coord.x = ts_state.touchX[0];
				last_ts_coord.y = ts_state.touchY[0];
				first_ts_coord.x = ts_state.touchX[0];
				first_ts_coord.y = ts_state.touchY[0];
			} else {
				int8_t diff_x = ts_state.touchX[0] - last_ts_coord.x;
				int8_t diff_y = ts_state.touchY[0] - last_ts_coord.y;

				HID_Buffer[1] = diff_x * 3;
				HID_Buffer[2] = diff_y * 3;

				last_ts_coord.x = ts_state.touchX[0];
				last_ts_coord.y = ts_state.touchY[0];

				// Check if the user finger left a predefined area
				// This means that this is not a clicking, just a cursor movement
				int32_t click_diff_x = ts_state.touchX[0] - first_ts_coord.x;
				int32_t click_diff_y = ts_state.touchY[0] - first_ts_coord.y;
				if (abs(click_diff_x) > TS_CLICK_THRESHOLD || abs(click_diff_y) > TS_CLICK_THRESHOLD)
					possible_click_event = 0;

				/*char position[100];
				int16_t cor_x = ts_state.touchX[0];
				int16_t cor_y = abs(ts_state.touchY[0] - 272);
				sprintf(position,"%3d - %3d", cor_x, cor_y);
				BSP_LCD_DisplayStringAtLine(1, (uint8_t *)position);*/
				//send(c_socket, &ts_state, sizeof(TS_StateTypeDef), 0);
			}
		} else {
			BSP_LED_Off(LED1);
			//first_touch_detected_flag = 0;
			if (possible_click_event) {
				HAL_Delay(10);
				HID_Buffer[0] = 0b001;
				HID_Buffer[0] = 0;
				possible_click_event = 0;
			}
		}
	}
	while (1) {
		/* Delete the Init Thread */
		osThreadTerminate(NULL);
	}
}

int connect_to_s(int *client_sock, uint16_t test_port, char *test_ip)
{
	// Creating client socket
	(*client_sock) = socket(PF_INET, SOCK_STREAM, IPPROTO_IP);
	if (*client_sock < 0) {
		LCD_ErrLog("Socket client - can't create socket\n");
		return -1;
	}

	// Creating server address structure
	struct sockaddr_in addr_in;
	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons(test_port);
	addr_in.sin_addr.s_addr = inet_addr(test_ip);

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
	//char buff[128];


	LCD_UsrLog("Socket client - startup...\n");
	LCD_UsrLog("Socket client - waiting for IP address...\n");

	// Wait for an IP address
	while (!is_ip_ok())
		osDelay(10);

	int c_socket;

	// Try to connect to the server
	if (connect_to_s(&c_socket, test_port, test_ip) == 0)
	{
		touch_screen(c_socket);

		closesocket(c_socket);
	}

	LCD_UsrLog("Socket client - terminating...\n");

	while (1) {
		osThreadTerminate(NULL);
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
