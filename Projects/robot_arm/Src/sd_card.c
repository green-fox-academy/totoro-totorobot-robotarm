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

		// Send the pointer through the mail queue to the logger thread
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

void write_sd_card(char* file_name, char* line_to_write)
{
	// Create file pointer
	FIL file_p;

	// Open a new or existing text file object with write access
	f_open(&file_p, file_name, FA_WRITE | FA_CREATE_NEW);

	// Append data as ASCII text to the file
	uint32_t size = (&file_p)->fsize;
	f_lseek(&file_p, size);
	f_printf(&file_p, line_to_write);

	// Close the file
	f_close(&file_p);

	return;
}

uint8_t verify_file(char* file_name){

	// Create file pointer
	FIL file_p;

	// Open the file with read access
	if (f_open(&file_p, file_name, FA_READ) != FR_OK) {
		return 1;
	}

	// Close file
	f_close(&file_p);

	return 0;
}

uint8_t read_G_code(FIL* file_o, G_code_t* G_code)
{
	char line_buffer[255];

	// Read text
	uint8_t is_G_code = 0;
	char* pch;

	while (!is_G_code) {

		// Read one line of text
		f_gets(line_buffer, sizeof(line_buffer), file_o);

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

		// Exit from function when we reach file end
		if (file_o->fptr >= file_o->fsize) {

			if (is_G_code) {
				return 1;
			} else {
				return 2;
			}
		}
	}

	return 0;
}

void file_reader_thread(void const * argument)
{
	// Create file object and G_code struct
	FIL file_o;
	G_code_t G_code;

	// Set flags
	file_reader_on = 1;
	end_moving = 0;

	// Let other processes to complete
	osDelay(500);

	// Wait for set position thread to start
	while (!set_position_on) {
		osDelay(100);
	}

	// Open the file with read-only access
	if (f_open(&file_o, (char*) argument, FA_READ) != FR_OK) {
		log_msg(ERROR, "Failed to open G-code file.\n");
	} else {

		uint8_t file_end = 0;

		// Read lines one-by-one
		while(file_reader_on) {

			// Read one line
			file_end = read_G_code(&file_o, &G_code);

			// Process G-code data
			if (file_end < 2) {

				// Debug
				// printf("G: %d, X: %d, Y: %d Z: %d\n", G_code.g, (int) G_code.x, (int) G_code.y, (int) G_code.z);
				// printf("file_end: %d\n", file_end);

				// Send G_code to set_position process and wait for arm movement
				while(1) {
					osMutexWait(arm_coord_mutex, osWaitForever);
					if (!next_coord_set) {

						// Set xyz coordinates
						target_xyz.x = G_code.x;
						target_xyz.y = G_code.y;
						target_xyz.z = G_code.z;

						// Set display message
						sprintf(target_display, "%3d  %3d  %3d   ", (int16_t) G_code.x, (int16_t) G_code.y, (int16_t) G_code.z);

						// If last line, signal end of movement
						if (file_end) {
							end_moving = 1;
							file_reader_on = 0;
						} else {
							end_moving = 0;
						}

						next_coord_set = 1;
						osMutexRelease(arm_coord_mutex);
						break;
					}
					osMutexRelease(arm_coord_mutex);
					osDelay(10);
				}

			} else {
				// file reached end + last line is not G-code
				while(1) {
					osMutexWait(arm_coord_mutex, osWaitForever);
					if (!next_coord_set) {

						// Signal end of movement
						end_moving = 1;

						next_coord_set = 1;
						osMutexRelease(arm_coord_mutex);
						break;
					}
					osMutexRelease(arm_coord_mutex);
					osDelay(10);
				}
				file_reader_on = 0;
			}
		}
	}

	while (1) {
		// Terminate thread

		// Reset G-code button on the display
		buttons[1].state = 0;
		buttons[1].touchable = 1;

		log_msg(USER, "File reader thread terminated\n");
		osThreadTerminate(NULL);
	}
}
