#include "servo_control.h"

void servo_config(void)
{
	// Load PWM configuration data
	pwm_conf[0].instance = SERVO0_INST;
	pwm_conf[0].period = SERVO0_PERIOD;
	pwm_conf[0].prescaler = SERVO0_PRESCALER;
	pwm_conf[0].pulse = (SERVO0_MIN_PULSE + SERVO0_MAX_PULSE) / 2;

	pwm_conf[1].instance = SERVO1_INST;
	pwm_conf[1].period = SERVO1_PERIOD;
	pwm_conf[1].prescaler = SERVO1_PRESCALER;
	pwm_conf[1].pulse = (SERVO1_MIN_PULSE + SERVO1_MAX_PULSE) / 2;

	pwm_conf[2].instance = SERVO2_INST;
	pwm_conf[2].period = SERVO2_PERIOD;
	pwm_conf[2].prescaler = SERVO2_PRESCALER;
	pwm_conf[2].pulse = (SERVO2_MIN_PULSE + SERVO2_MAX_PULSE) / 2;

	pwm_conf[3].instance = SERVO3_INST;
	pwm_conf[3].period = SERVO3_PERIOD;
	pwm_conf[3].prescaler = SERVO3_PRESCALER;
	pwm_conf[3].pulse = (SERVO3_MIN_PULSE + SERVO3_MAX_PULSE) / 2;

	adc_ch_conf[0] = SERVO0_ADC_CHANNEL;
	adc_ch_conf[1] = SERVO1_ADC_CHANNEL;
	adc_ch_conf[2] = SERVO2_ADC_CHANNEL;
	adc_ch_conf[3] = SERVO3_ADC_CHANNEL;

	// Load servo configuration data
	servo_conf[0].min_angle_deg = SERVO0_MIN_ANGLE;
	servo_conf[0].max_angle_deg = SERVO0_MAX_ANGLE;
	servo_conf[0].min_angle_rad = deg_to_rad(SERVO0_MIN_ANGLE);
	servo_conf[0].max_angle_rad = deg_to_rad(SERVO0_MAX_ANGLE);
	servo_conf[0].min_pulse = SERVO0_MIN_PULSE;
	servo_conf[0].max_pulse = SERVO0_MAX_PULSE;

	servo_conf[1].min_angle_deg = SERVO1_MIN_ANGLE;
	servo_conf[1].max_angle_deg = SERVO1_MAX_ANGLE;
	servo_conf[1].min_angle_rad = deg_to_rad(SERVO1_MIN_ANGLE);
	servo_conf[1].max_angle_rad = deg_to_rad(SERVO1_MAX_ANGLE);
	servo_conf[1].min_pulse = SERVO1_MIN_PULSE;
	servo_conf[1].max_pulse = SERVO1_MAX_PULSE;

	servo_conf[2].min_angle_deg = SERVO2_MIN_ANGLE;
	servo_conf[2].max_angle_deg = SERVO2_MAX_ANGLE;
	servo_conf[2].min_angle_rad = deg_to_rad(SERVO2_MIN_ANGLE);
	servo_conf[2].max_angle_rad = deg_to_rad(SERVO2_MAX_ANGLE);
	servo_conf[2].min_pulse = SERVO2_MIN_PULSE;
	servo_conf[2].max_pulse = SERVO2_MAX_PULSE;

	servo_conf[3].min_angle_deg = SERVO3_MIN_ANGLE;
	servo_conf[3].max_angle_deg = SERVO3_MAX_ANGLE;
	servo_conf[3].min_angle_rad = deg_to_rad(SERVO3_MIN_ANGLE);
	servo_conf[3].max_angle_rad = deg_to_rad(SERVO3_MAX_ANGLE);
	servo_conf[3].min_pulse = SERVO3_MIN_PULSE;
	servo_conf[3].max_pulse = SERVO3_MAX_PULSE;

	// Position arm to middle position
	for (int i = 0; i < SERVOS; i++) {
		servo_pulse[i] = pwm_conf[i].pulse;
	}

	// Load data for kinematics calculation
	kinematics_conf();

	return;
}

void pwm_init(void)
{
	for (int i = 0; i < SERVOS; i++) {
		pwm[i].Instance = pwm_conf[i].instance;
		pwm[i].State = HAL_TIM_STATE_RESET;
		pwm[i].Channel = HAL_TIM_ACTIVE_CHANNEL_1;
		pwm[i].Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
		pwm[i].Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		pwm[i].Init.CounterMode = TIM_COUNTERMODE_UP;
		pwm[i].Init.Period = pwm_conf[i].period;
		pwm[i].Init.Prescaler = pwm_conf[i].prescaler;
		HAL_TIM_PWM_Init(&pwm[i]);

		char tmp[50];
		sprintf(tmp, "Servo%d init done\n", i);
		log_msg(DEBUG, tmp);

		pwm_oc_init[i].OCFastMode = TIM_OCFAST_DISABLE;
		pwm_oc_init[i].OCIdleState = TIM_OCIDLESTATE_RESET;
		pwm_oc_init[i].OCMode = TIM_OCMODE_PWM1;
		pwm_oc_init[i].OCPolarity = TIM_OCPOLARITY_HIGH;
		pwm_oc_init[i].Pulse = pwm_conf[i].pulse;
		HAL_TIM_PWM_ConfigChannel(&pwm[i], &pwm_oc_init[i], TIM_CHANNEL_1);

		sprintf(tmp, "Servo%d config channel done\n", i);
		log_msg(DEBUG, tmp);

		HAL_TIM_PWM_Start(&pwm[i], TIM_CHANNEL_1);

		sprintf(tmp, "Servo%d started\n", i);
		log_msg(DEBUG, tmp);
	}
	return;
}

void pwm_set_pulse(uint8_t servo, uint32_t pulse)
{
	// Set pulse width
	pwm_oc_init[servo].Pulse = pulse;

	HAL_TIM_PWM_ConfigChannel(&pwm[servo], &pwm_oc_init[servo], TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&pwm[servo], TIM_CHANNEL_1);

//	char tmp[30];
//	sprintf(tmp, "Servo: %d pulse: %5lu\n", servo, pulse);
//	log_msg(DEBUG, tmp);

	return;
}

void adc_init(void)
{
	// General ADC init
	adc.State = HAL_ADC_STATE_RESET;
	adc.Instance = ADC3;
	adc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	adc.Init.Resolution = ADC_RESOLUTION_12B;
	adc.Init.EOCSelection = DISABLE;
	adc.Init.DMAContinuousRequests = DISABLE;
	adc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	adc.Init.ContinuousConvMode = DISABLE;
	adc.Init.DiscontinuousConvMode = DISABLE;
	adc.Init.ScanConvMode = DISABLE;
	adc.Init.NbrOfConversion = 4;
	HAL_ADC_Init(&adc);

	// ADC channel general init
	// Channel selection will be set in adc_measure
	adc_ch.Channel = adc_ch_conf[0];
	adc_ch.Offset = 0;
	adc_ch.Rank = 1;
	adc_ch.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	HAL_ADC_ConfigChannel(&adc, &adc_ch);

	return;
}

void adc_deinit(void)
{
	HAL_ADC_Stop(&adc);
	return;
}

void adc_measure(void)
{
	for (int i = 0; i < SERVOS; i++) {

		// Select channel
		adc_ch.Channel = adc_ch_conf[i];
		HAL_ADC_ConfigChannel(&adc, &adc_ch);

		// Measure
		HAL_ADC_Start(&adc);
		HAL_ADC_PollForConversion(&adc, HAL_MAX_DELAY);
		uint32_t adc_measurement = HAL_ADC_GetValue(&adc);

		// Update global storage of ADC data
		osMutexWait(servo_adc_mutex, osWaitForever);
		adc_values[i] = adc_measurement;
		osMutexRelease(servo_adc_mutex);

		osDelay(5);
	}

	// Log values
	if (lcd_logger_on || sd_logger_on) {
		char tmp[90];

		osMutexWait(servo_adc_mutex, osWaitForever);
		sprintf(tmp, "ADC0: %4lu  ADC1: %4lu  ADC2: %4lu  ADC3: %4lu\n", adc_values[0], adc_values[1], adc_values[2], adc_values[3]);
		osMutexRelease(servo_adc_mutex);
		log_msg(DEBUG, tmp);
	}

	return;
}

void start_adc_thread(void)
{
	adc_init();
	adc_on = 1;

	log_msg(USER, "ADC thread started\n");

    osThreadDef(ADC_MEASURE, adc_thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 5);
    osThreadCreate (osThread(ADC_MEASURE), NULL);

	return;
}

void stop_adc_thread(void)
{
	adc_on = 0;
	osDelay(100);
	adc_deinit();

	return;
}

void pwm_thread(void const * argument)
{
	pwm_ready = 0;

	log_msg(USER, "PWM thread started\n");

	// Initialize all PWM channels
	pwm_init();
	pwm_ready = 1;



	log_msg(DEBUG, "PWM ready\n");

	// Set servo positions
	while (1) {
		for (int i = 0; i < SERVOS; i++) {

			// Lock mutex
			osMutexWait(servo_pulse_mutex, osWaitForever);

			// Get pulse value
			uint32_t pulse = servo_pulse[i];

			// Release mutex
			osMutexRelease(servo_pulse_mutex);

			// Set PWM pulse width
			pwm_set_pulse(i, pulse);
		}
		osDelay(100);
	}

    while (1) {
        // Terminate thread
    	log_msg(USER, "PWM thread terminated\n");
    	pwm_ready = 0;
        osThreadTerminate(NULL);
    }
}

void adc_thread(void const * argument)
{

	log_msg(USER, "ADC thread started\n");

	while (adc_on) {

		// Get ADC values and convert to pulse width
		adc_measure();

		// Update servo PWM pulse widths
		for (int i = 0; i < SERVOS; i++) {

			// Calculate PMW pulse width
			osMutexWait(servo_adc_mutex, osWaitForever);
			uint32_t adc_pulse = (uint32_t) map((double) adc_values[i], (double) MIN_ADC_VALUE, (double) MAX_ADC_VALUE,
							                    (double) servo_conf[i].min_pulse, (double) servo_conf[i].max_pulse);
			osMutexRelease(servo_adc_mutex);

			// Write pulse values to global storage
			osMutexWait(servo_pulse_mutex, osWaitForever);
			servo_pulse[i] = adc_pulse;
			osMutexRelease(servo_pulse_mutex);
		}

		osDelay(100);
	}

	while (1) {
		// Terminate thread
		log_msg(USER, "ADC thread terminated\n");
		osThreadTerminate(NULL);
	}
}

double map(double input, double min_in, double max_in, double min_out, double max_out)
{
	double ratio =  (input - min_in) / (max_in - min_in);
	double output = (max_out - min_out) * ratio + min_out;

	return output;
}

uint8_t xyz_to_pulse(coord_cart_t* pos_cart)
{
	coord_polar_t pos_polar;
	angles_t joint_angles;

	// Verify if xyz coordinates are within work area
	if (verify_xyz(&pos_cart) != 0) {
		log_msg(ERROR, "XYZ coordinates are out of allowed area!\n");
		return 1;
	}

	// Convert xyz to polar coordinates
	cart_to_polar(pos_cart, &pos_polar);

	// Calculate relative servo angles
	calc_inverse_kinematics(&pos_polar, &joint_angles);

	// Calculate and set pulse
	if (ang_rel_to_pulse(&joint_angles) != 0) {
		return 1;
	}

	return 0;
}

void pulse_to_xyz(coord_cart_t* pos_cart)
{
	angles_t joint_angles;

	pulse_to_ang_abs(&joint_angles);
	ang_abs_to_xyz(&joint_angles, pos_cart);

	return;
}

uint8_t ang_abs_to_pulse(angles_t* joint_angles)
{
	uint32_t pulse_width[SERVOS - 1];
	double angles[SERVOS - 1];
	angles_t joint_angles_deg;

	// Convert angles to degrees
	joint_angles_deg.theta0 = rad_to_deg(joint_angles->theta0);
	joint_angles_deg.theta1 = rad_to_deg(joint_angles->theta1);
	joint_angles_deg.theta2 = rad_to_deg(joint_angles->theta2);

	// Check if angles are in the allowed range
	if (verify_angle(&joint_angles_deg) != 0) {
		return 1;
	}

	// Feed angles into array
	angles[0] = joint_angles->theta0;
	angles[1] = joint_angles->theta1;
	angles[2] = joint_angles->theta2;

	// Calculate pulse width values
	for (int i = 0; i < SERVOS - 1; i++) {
		pulse_width[i] = (uint32_t) map(angles[i], servo_conf[i].min_angle_rad, servo_conf[i].max_angle_rad,
										(double) servo_conf[i].min_pulse, (double) servo_conf[i].max_pulse);
	}

	// Verify if pulse width values are within allowed range
	for (int i = 0; i < SERVOS - 1; i++) {
		if (verify_pulse(i, pulse_width[i]) != 0) {
			log_msg(ERROR, "Pulse width is out of allowed range!\n");
			return 1;
		}
	}

	// Update servo pulse values
	osMutexWait(servo_pulse_mutex, osWaitForever);
	for (int i = 0; i < SERVOS - 1; i++) {
		servo_pulse[i] = pulse_width[i];
	}
	osMutexRelease(servo_pulse_mutex);

	return 0;
}

uint8_t ang_rel_to_pulse(angles_t* joint_angles)
{
	// Convert angles from relative to absolute values
	rel_to_abs_angle(joint_angles);

	// Calculate and set pulse
	if (ang_abs_to_pulse(joint_angles) != 0) {
		return 1;
	}

	return 0;
}

void pulse_to_ang_abs(angles_t* joint_angles)
{
	/*
	 * This function calculates absolute angles, i.e.
	 * they are measured to the XY plane
	 */

	uint32_t pulse_width[SERVOS - 1];
	double ang_rad[SERVOS - 1];

	// Get actual pulse values
	osMutexWait(servo_pulse_mutex, osWaitForever);
	for (int i = 0; i < SERVOS - 1; i++) {
		pulse_width[i] = servo_pulse[i];
	}
	osMutexRelease(servo_pulse_mutex);

	// Calculate angles in radian from pulse width
	for (int i = 0; i < SERVOS - 1; i++) {
		ang_rad[i] = map(pulse_width[i], (double) servo_conf[i].min_pulse, (double) servo_conf[i].max_pulse,
						 servo_conf[i].min_angle_rad, servo_conf[i].max_angle_rad);
	}

	// Feed angles into structure
	joint_angles->theta0 = ang_rad[0];
	joint_angles->theta1 = ang_rad[1];
	joint_angles->theta2 = ang_rad[2];

	return;
}

void pulse_to_ang_rel(angles_t* joint_angles)
{
	/*
	 * This function calculates the relative angles
	 * i.e. measured to the previous link
	 */

	// Calculate absolute angles
	pulse_to_ang_abs(joint_angles);

	// Convert angles to relative
	abs_to_rel_angle(joint_angles);

	return;
}

void ang_rel_to_xyz(angles_t* joint_angles, coord_cart_t* pos_cart)
{
	coord_polar_t pos_polar;

	// Calculate position in polar coordinates
	calc_forward_kinematics(joint_angles, &pos_polar);

	// Convert polar coordinates to xyz
	polar_to_cart(&pos_polar, pos_cart);

	return;
}

void ang_abs_to_xyz(angles_t* joint_angles, coord_cart_t* pos_cart)
{
	// Convert absolute angles to relative
	abs_to_rel_angle(joint_angles);

	// Calculate xyz from relative angles
	ang_rel_to_xyz(joint_angles, pos_cart);

	return;
}

uint8_t verify_xyz(coord_cart_t* coord) {

	if ((coord->x > WORK_AREA_MAX_X) || (coord->x < WORK_AREA_MIN_X)) {
		return 1;	// Flag error
	}

	if ((coord->y > WORK_AREA_MAX_Y) || (coord->y < WORK_AREA_MIN_Y)) {
		return 1;	// Flag error
	}

	if ((coord->z > WORK_AREA_MAX_Z) || (coord->z < WORK_AREA_MIN_Z)) {
		return 1;	// Flag error
	}

	return 0;
}

uint8_t verify_pulse(uint8_t servo, uint32_t pulse) {

	// Check if within min and max values
	if ((pulse > servo_conf[servo].max_pulse) || (pulse < servo_conf[servo].min_pulse)) {
		return 1; // Flag error
	}

	return 0;
}

uint8_t verify_angle(angles_t* ang_deg) {

	// 1.) Check if inside min and max range
	if ((ang_deg->theta0 > servo_conf[0].max_angle_deg) ||
		(ang_deg->theta0 < servo_conf[0].min_angle_deg)) {
		log_msg(ERROR, "Theta0 is out of allowed range!\n");
		return 1;
	}

	if ((ang_deg->theta1 > servo_conf[1].max_angle_deg) ||
		(ang_deg->theta1 < servo_conf[1].min_angle_deg)) {
		log_msg(ERROR, "Theta1 is out of allowed range!\n");
		return 1;
	}

	if ((ang_deg->theta2 > servo_conf[2].max_angle_deg) ||
		(ang_deg->theta2 < servo_conf[2].min_angle_deg)) {
		log_msg(ERROR, "Theta2 is out of allowed range!\n");
		return 1;
	}

	// 2.) 180 deg - (theta1 + theta2) > 35 deg
	// where theta2 is the angle of link2 to horizon
	if (180 - (ang_deg->theta1 + ang_deg->theta2) <= 35) {
		log_msg(ERROR, "Angle between link1 and link2 is less than 35 deg!\n");
		return 1;
	}

	// 3.) theta2 <= 0.035416 * theta1^2 - 2.51917 theta1 + 48
	// where theta2 is the angle of link2 to horizon. Angles in deg.
	if (ang_deg->theta2 > 0.035416 * pow(ang_deg->theta1, 2.0) - 2.51917 * ang_deg->theta1 + 48) {
		log_msg(ERROR, "Link1 and link2 angles are outside of safe range!\n");
		return 1;
	}

	return 0;
}

void set_position_thread(void const * argument)
{
	// Define variables
	coord_cart_t current_pos;
	coord_cart_t target_pos;
	double step_size = DEFAULT_STEP;
	double speed = DEFAULT_SPEED;
	uint32_t wait_time = (step_size * 1000) / speed;

	// Set thread flag to ready
	set_position_on = 1;

	while (1) {

		uint8_t new_coord_ready = 0;

		// Loop until a new target coordinate appears
		while (!new_coord_ready) {
			osMutexWait(arm_coord_mutex, osWaitForever);
			if (next_coord_set) {

				// Read in target position
				target_pos.x = arm_pos_c.x;
				target_pos.y = arm_pos_c.y;
				target_pos.z = arm_pos_c.z;

				// Reset next coordinate flag, so that other processes can use it
				next_coord_set = 0;
			}

			osMutexRelease(arm_coord_mutex);
			new_coord_ready = 1;
			osDelay(5);
		}

		// Get current position based on servo PWM parameters
		pulse_to_xyz(&current_pos);

		// If target differs from current...
		if ((abs(target_pos.x - current_pos.x) > MIN_X_RES) ||
			(abs(target_pos.y - current_pos.y) > MIN_Y_RES) ||
			(abs(target_pos.z - current_pos.z) > MIN_Z_RES)) {

			// Verify if target position is in the work area ,if not send error
			if (verify_coordinates((int16_t) target_pos.x, (int16_t) target_pos.y,
					               (int16_t) target_pos.z) != 0) {
				log_msg(ERROR, "Target xyz values are outside of working area!\n");

				// TODO: check what is the appropriate action here
				// break will terminate the thread
				break;
			}

			// Calculate 3d distance
			double dist = sqrt(pow(target_pos.x - current_pos.x, 2.0) +
					           pow(target_pos.y - current_pos.y, 2.0) +
						       pow(target_pos.z - current_pos.z, 2.0));

			// Break down distance to steps.
			uint16_t steps = dist / step_size;
			double step_x = (target_pos.x - current_pos.x) / steps;
			double step_y = (target_pos.y - current_pos.y) / steps;
			double step_z = (target_pos.z - current_pos.z) / steps;

			// Calculate step sizes along axes and execute movement
			for (uint16_t i = 0; i < steps; i++) {
				coord_cart_t interm_pos;
				interm_pos.x = current_pos.x + step_x * (i + 1);
				interm_pos.y = current_pos.y + step_y * (i + 1);
				interm_pos.z = current_pos.z + step_z * (i + 1);

				// Convert steps to motor pulse
				if (xyz_to_pulse(&interm_pos) != 0) {
					log_msg(ERROR, "Pulse out of range, set_position_thread will terminate.\n");
					break;
				} else {
					char tmp[100];
					sprintf(tmp, "movement to x:%d, y:%d, z:%d\n", (int16_t) interm_pos.x, (int16_t) interm_pos.y, (int16_t) interm_pos.z);
					log_msg(DEBUG, tmp);
				}
				osDelay(wait_time);
			}
		}
	}

	while (1) {
		// Terminate thread
		log_msg(USER, "set_position_thread terminated\n");
		osThreadTerminate(NULL);
	}
}

