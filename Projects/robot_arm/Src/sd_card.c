#include "sd_card.h"

void log_msg(uint8_t log_level, char* message)
{
	// If allowed print to LCD screen
	if (lcd_logger_on && log_level && (lcd_log_level >= log_level)) {
		LCD_UsrLog(message);
	}

	// If allowed, write to log file
	if (sd_logger_on && log_level && (file_log_level >= log_level)) {

		// Because safe file operations takes more time than only writing
		// to memory, writing to a log file is handled by a separate thread.
		// Here we send the log message via a MailQueue to the file logging thread.

		// Create data structure in memory and a pointer to it
		msg_log_t* log_data;
		log_data = (msg_log_t*) osMailAlloc(msg_log_q, osWaitForever);

		// Fill in data
		// TODO implement time stamp
		strcpy(log_data->string, message);
		log_data->log_level = log_level;

		// Send the pointer through the mail queue to the loggre thread
		osMailPut(msg_log_q, log_data);
	}

	// TODO implement UART and HTTP logging if needed

	return;
}

void sd_logger_thread(void const * argument)
{
	// Init FAT file system
	if (FAT_fs_init() == 0) {
		// Enable logging
		// TODO enable/disable from UART
		sd_logger_on = 1;
	} else {
		sd_logger_on = 0;
	}

	// Create log file name
	// TODO append date and time to file name
	sprintf(logfile_name, "totorobot.log");

	while (sd_logger_on) {

		// Wait for log messages in the MailQueue
		osEvent event = osMailGet(msg_log_q, osWaitForever);

		// Collect incoming mail. ".p" indicates that the message is a pointer
		msg_log_t* received = (msg_log_t*) event.value.p;       //

		// Process the message
		char line[120];

		switch (received->log_level) {
		case NONE:
			// Nothing to do
			break;
		case ERROR:
			sprintf(line, "ERROR %s", received->string);
			break;
		case USER:
			sprintf(line, "USER %s", received->string);
			break;
		case DEBUG:
			sprintf(line, "DEBUG %s", received->string);
			break;
		}

		// Free up memory block, so it can be reused
		osMailFree(msg_log_q, received);

		// Write to file
		write_sd_card(logfile_name, line);

		osDelay(100);
	}

	while (1) {
		// Terminate thread
		log_msg(USER, "SD card logging thread terminated\n");
		osThreadTerminate(NULL);
	}
}

uint8_t FAT_fs_init(void)
{
	// Link the micro SD disk I/O driver
	if (FATFS_LinkDriver(&SD_Driver, sd_path) != 0) {
		log_msg(ERROR, "Linking the micro SD disk I/O driver has failed.\n");
		return 1;
	} else {
		log_msg(DEBUG, "Linking the micro SD disk I/O driver is successful.\n");
	}

	// Register the file system object to the FatFs module
	if (f_mount(&SDFatFs, (TCHAR const*) sd_path, 0) != FR_OK) {
		log_msg(ERROR, "Mounting the SD drive has failed .\n");
		return 2;
	} else {
		log_msg(DEBUG, "SD driver mounted.\n");
	}
	return 0;
}

void read_sd_card(char* file_name)
{

	// Create file pointer
	FIL file;

	// Open the text file object with read access
	if (f_open(&file, file_name, FA_READ) != FR_OK) {
		LCD_ErrLog((char*) "Open file has failed.\n");
	}


	/*## Read data from the text file ###########################*/
	f_read(&file, rtext, sizeof(rtext), (UINT*)&bytesread);
	LCD_UsrLog(rtext);


	/*##-9- Close the open text file #############################*/
	f_close(&file);
}

void write_sd_card(char* file_name, char* line_to_write)
{
	// Create file pointer
	FIL file;

	// Open a new or existing text file object with write access
	f_open(&file, file_name, FA_OPEN_EXISTING | FA_WRITE);

	// Append data as ASCII text to the file
	uint32_t size = (&file)->fsize;
	f_lseek(&file, size);
	f_printf(&file, line_to_write);

	// Close the file
	f_close(&file);

	return;
}
