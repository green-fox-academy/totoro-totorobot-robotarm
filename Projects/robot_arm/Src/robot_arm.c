/* Includes ------------------------------------------------------------------*/
#include "robot_arm.h"



/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


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

	sockfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP); // Create a UDP socket.

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
	sprintf(text,"%.2d:%.2d:%.2d %.4d", timeStruct.Hours, timeStruct.Minutes, timeStruct.Seconds, timeStruct.SubSeconds);
	sprintf(text_1,"%.4d-%.2d-%.2d", dateStruct.Year + 1900, dateStruct.Month + 1, dateStruct.Date);
	LCD_UsrLog("Time: %s %s\n", text_1, text);
	osDelay(800);
	}
	while (1) {
		/* Delete the Init Thread */
		osThreadTerminate(NULL);
}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
