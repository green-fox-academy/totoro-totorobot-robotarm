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
	FIL file_p;

	// Open the text file object with read access
	if (f_open(&file_p, file_name, FA_READ) != FR_OK) {
		LCD_ErrLog((char*) "Open file has failed.\n");
	}


	/*## Read data from the text file ###########################*/
	f_read(&file_p, rtext, sizeof(rtext), (UINT*)&bytesread);
	LCD_UsrLog(rtext);


	/*##-9- Close the open text file #############################*/
	f_close(&file_p);
}

void write_sd_card(char* file_name, char* line_to_write)
{
	// Create file pointer
	FIL file_p;

	// Open a new or existing text file object with write access
	f_open(&file_p, file_name, FA_OPEN_EXISTING | FA_WRITE | FA_CREATE_NEW);

	// Append data as ASCII text to the file
	uint32_t size = (&file_p)->fsize;
	f_lseek(&file_p, size);
	f_printf(&file_p, line_to_write);

	// Close the file
	f_close(&file_p);

	return;
}

uint8_t verify_file(char* file_name){
	// TODO: write function
	return 0;
}

void read_G_code(char* file_name, uint32_t* read_pos, G_code_t* G_code)
{
	// TODO how to handle file end?
	// TODO how to handle if there is no G-code in file?

	// Create file pointer
	FIL file_p;
	char line_buffer[255];

	// Open the G-code file with read-only access
	if (f_open(&file_p, file_name, FA_READ) != FR_OK)
		log_msg(ERROR, "Failed to open G-code file.\n");

	// Read text
	uint8_t is_G_code = 0;
	char* pch;

	while (!is_G_code) {

		f_lseek(&file_p, *read_pos);
		f_gets(line_buffer, sizeof(line_buffer), &file_p);
		*read_pos += strlen(line_buffer) + 1;

		// Process line if it starts with G
		if (line_buffer[0] == 'G') {

			pch = strtok(line_buffer, " ");

			while (pch != NULL) {

				/*
				 * We cannot use nested strtok, so we skip the first letter
				 * by manipulating the pch pointer. After encapsulating data
				 * pch must be set to original value to avoid crash
				 */

				switch(pch[0]) {
				case 'G':
				    pch++;
					G_code->g = atoi(pch);
                    pch--;
					break;
				case 'X':
				    pch++;
					G_code->x = atof(pch);
                    pch--;
					break;
				case 'Y':
				    pch++;
				    G_code->y = atof(pch);
                    pch--;
					break;
				case 'Z':
                    pch++;
                    G_code->z = atof(pch);
                    pch--;
					break;
				case 'E':
				    pch++;
				    G_code->e = atof(pch);
                    pch--;
					break;
				case 'F':
				    pch++;
				    G_code->f = atof(pch);
				    pch--;
					break;
				}

				pch = strtok(NULL, " ");
			}

			is_G_code = 1;
		}
	}

	// Close file
	f_close(&file_p);

	return;
}

void file_reader_thread(void const * argument)
{
	char* file_name_p = (char*) argument;
	char file_name[100];
	strcpy(file_name, file_name_p);

	log_msg(DEBUG, "inside file reader\n");
	LCD_UsrLog((char*) argument);
	LCD_UsrLog("\n");

	file_reader_on = 1;

	// Check other processes if they are running when needed

	// Read in G-code
	// Loop
	uint32_t read_pos;
	G_code_t G_code;

	// read_G_code(file_name, &read_pos, &G_code);

	while (1) {
		// Terminate thread
		file_reader_on = 0;
		log_msg(USER, "File reader thread terminated\n");
		osThreadTerminate(NULL);
	}
}
