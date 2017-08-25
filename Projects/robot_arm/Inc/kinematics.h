#ifndef __KINEMATICS_H_
#define __KINEMATICS_H_

#include "stm32f7xx_hal.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

/* Robot arm geometry */

#define VERTICAL_ARM_SIZE		134	// mm
#define HORIZONTAL_ARM_SIZE		147	// mm
#define PLATFORM_HEIGHT			98	// mm
#define GRIPPER_Z_DIST			10	// mm
#define GRIPPER_R_DIST			100	// mm

/* Allowed work area */
#define WORK_AREA_MIN_X	-100
#define	WORK_AREA_MAX_X	300
#define WORK_AREA_MIN_Y	-100
#define	WORK_AREA_MAX_Y	300
#define WORK_AREA_MIN_Z	-100
#define	WORK_AREA_MAX_Z	300

/* MINIMUM MOVEMENT RESOLUTION */
#define MIN_X_RES	1 // mm
#define MIN_Y_RES	1 // mm
#define MIN_Z_RES	1 // mm

#define MIN_THETA0_RES	1 // deg
#define MIN_THETA1_RES	1 // deg
#define MIN_THETA2_RES	1 // deg

#define MIN_PULSE_RES	50 // pulse

/* MAXIMUM ALLOWED SPEEDS */
#define DEFAULT_SPEED			1  // mm/sec
#define DEFAULT_ANG_SPEED		2  // deg/sec
#define DEFAULT_PULSE_SPEED		60 // pulse/sec

#define DEFAULT_STEP		1  // mm
#define DEFAULT_ANG_STEP	2  // deg
#define DEFAULT_PULSE_STEP	60 // pulse

typedef struct {
	double x;		// mm
	double y;		// mm
	double z;		// mm
} coord_cart_t;

typedef struct {
	double angle;	// radian
	double r;		// mm
	double z;		// mm
} coord_polar_t;

typedef struct {
	double theta0;	// radian or deg
	double theta1;	// radian or deg
	double theta2;	// radian or deg
} angles_t;

double l1;	// Length of vertical arm between two joints
double l2;	// Length of horizontal arm between two joints
double z0;	// Elevation of joint1 from reference plane
double z3;	// Distance between joint3 and base Z point of gripper
double r3;	// Distance between joint3 and base R point of gripper

void kinematics_conf(void);
double deg_to_rad(int16_t deg);
int16_t rad_to_deg(double rad);
void abs_to_rel_angle(angles_t* joint_angles);
void rel_to_abs_angle(angles_t* joint_angles);
void polar_to_cart(coord_polar_t* pos_polar, coord_cart_t* pos_cart);
void cart_to_polar(coord_cart_t* pos_cart, coord_polar_t* pos_polar);
void calc_forward_kinematics(angles_t* joint_angles, coord_polar_t* pos_polar);
void calc_inverse_kinematics(coord_polar_t* pos_polar, angles_t* joint_angles);

#endif /* __KINEMATICS_H_ */
