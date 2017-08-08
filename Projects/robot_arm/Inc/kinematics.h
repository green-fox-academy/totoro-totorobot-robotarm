#ifndef __KINEMATICS_H_
#define __KINEMATICS_H_

#include "stm32f7xx_hal.h"
//#include "cmsis_os.h"
#include "lcd_log.h"
//#include "uart.h"
//#include "servo_control.h"
//#include "robot_arm_conf.h"
#include <math.h>

/* Robot arm geometry */

#define VERTICAL_ARM_SIZE		134	// mm
#define HORIZONTAL_ARM_SIZE		147	// mm
#define PLATFORM_HEIGHT			95	// mm
#define GRIPPER_Z_DIST			17	// mm
#define GRIPPER_R_DIST			96	// mm

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
	double theta0;	// radian
	double theta1;	// radian
	double theta2;	// radian
} angles_t;

double l1;	// Length of vertical arm between two joints
double l2;	// Length of horizontal arm between two joints
double z0;	// Elevation of joint1 from reference plane
double z3;	// Distance between joint3 and base Z point of gripper
double r3;	// Distance between joint3 and base R point of gripper

void kinmatics_conf(void);
double deg_to_rad(int16_t deg);
int16_t rad_to_deg(double rad);
double abs_to_rel_angle(double angle);
double rel_to_abs_angle(double angle);
void polar_to_cart(coord_polar_t* pos_polar, coord_cart_t* pos_cart);
void cart_to_polar(coord_cart_t* pos_cart, coord_polar_t* pos_polar);
void calc_forward_kinematics(angles_t* joint_angles, coord_polar_t* pos_polar);
void calc_inverse_kinematics(coord_polar_t* pos_polar, angles_t* joint_angles);

#endif /* __KINEMATICS_H_ */
