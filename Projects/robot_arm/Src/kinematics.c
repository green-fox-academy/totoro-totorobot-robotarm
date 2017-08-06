#include "kinematics.h"
#include <math.h>

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

double l1;	// Length of vertical arm
double l2;	// Length of horizontal arm
double z0;	// Elevation of joint1 from reference plane
double z3;	// Distance between joint3 and base Z point of gripper
double r3;	// Distance between joint3 and base R point of gripper

#define VERTICAL_ARM_SIZE 100
#define HORIZONTAL_ARM_SIZE 100
#define PLATFORM_HEIGHT 100
#define GRIPPER_Z_DIST	100
#define GRIPPER_R_DIST	100

// Set up basic parameters
void kinmatics_conf(void)
{
	l1 = (double) VERTICAL_ARM_SIZE;
	l2 = (double) HORIZONTAL_ARM_SIZE;
	z0 = (double) PLATFORM_HEIGHT;
	z3 = (double) GRIPPER_Z_DIST;
	r3 = (double) GRIPPER_R_DIST;

	return;
}


// Convert from degrees to radians
double deg_to_rad(int16_t deg)
{
	return ((double) deg / 360.0) * 2.0 * M_PI;
}

// Convert from radians to degrees
int16_t deg_to_rad(double rad)
{
	return (rad / (2.0 * M_PI)) * 360.0;
}





// Convert from polar to carthesian coordinates
void polar_to_cart(coord_polar_t* pos_polar, coord_cart_t* pos_cart)
{
	// Calculate coord X
	pos_cart->x = (cos(pos_polar->angle)) * pos_polar->r;

	// Calculate coord Y
	pos_cart->y = (sin(pos_polar->angle)) * pos_polar->r;

	// Calculate coord Z
	pos_cart->z = pos_polar->z;

	return;
}

// Convert from carthesian to polar coordinates
void cart_to_polar(coord_cart_t* pos_cart, coord_polar_t* pos_polar)
{
	// Calculate coord R
	pos_polar->r = sqrt(pow(pos_cart->x, 2.0) + pow(pos_cart->y, 2.0));

	// Calculate angle
	pos_polar->angle = atan2(pos_cart->x, pos_cart->y);

	// Calculate coord Z
	pos_polar->z = pos_cart->z;

	return;
}

// Calculate forward kinematics: joint angles -> polar coordinates
void calc_forward_kinematics(angles_t* joint_angles, coord_polar_t* pos_polar)
{

	double r;
	double z;

	// Calculate coord R based on the elbows' joint angles
	r = l1 * cos(joint_angles->theta1) + l2 * cos(joint_angles->theta2);

	// Calculate coord Z based on the elbows' joint angle
	z = l1 * sin(joint_angles->theta1) + l2 * sin(joint_angles->theta2);

	// Calculate angle based on given rotation around axis Z
	pos_polar->angle = joint_angles->theta0;

	// Correct gripper displacement and joint0 elevation, save values
	pos_polar->r = r + r3;
	pos_polar->z = pos_polar->z + z0 - z3;

	return;
}

// Calculate inverse kinematics: polar coordinates -> joint angles;
void calc_inverse_kinematics(coord_polar_t* pos_polar, angles_t* joint_angles)
{
	// Correct gripper displacement and joint0 elevation
	double r = pos_polar->r - r3;
	double z = pos_polar->z - z0 + z3;

	// Calculate theta2
	double cos_theta2 = (pow(r, 2.0) + pow(z, 2.0) - pow(l1, 2.0) - pow(l2, 2.0)) / (2.0 * l1 * l2);

	// TODO: check if we can do elbow up at all
	uint8_t elbow_dir = -1; // elbow down, +1 for elbow up
	joint_angles->theta2 = atan2(elbow_dir * sqrt(1.0 - pow(cos_theta2, 2.0)), cos_theta2);

	// Calculate theta1
	double k1 = l1 + l2 * cos_theta2;
	double k2 = l2 * sin(joint_angles->theta2);

	joint_angles->theta1 = atan2(z, r) - atan2(k2, k1);

	// Calculate theta0
	joint_angles->theta0 = pos_polar->angle;

	return;
}



