#include "kinematics.h"

// Set up basic parameters
void kinematics_conf(void)
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
	return ((double) deg / 180.0) * M_PI;
}

// Convert from radians to degrees
int16_t rad_to_deg(double rad)
{
	return (rad / M_PI) * 180.0;
}

void abs_to_rel_angle(angles_t* joint_angles)
{
	double gamma = joint_angles->theta2;
	joint_angles->theta2 = gamma - joint_angles->theta1;

	return;
}

void rel_to_abs_angle(angles_t* joint_angles)
{
	double gamma = joint_angles->theta1 + joint_angles->theta2;
	joint_angles->theta2 = gamma;

	return;
}

// Convert from polar to cartesian coordinates
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

// Convert from cartesian to polar coordinates
void cart_to_polar(coord_cart_t* pos_cart, coord_polar_t* pos_polar)
{
	// Calculate coord R
	pos_polar->r = sqrt(pow(pos_cart->x, 2.0) + pow(pos_cart->y, 2.0));

	// Calculate angle
	pos_polar->angle = atan2(pos_cart->y, pos_cart->x);

	// Calculate coord Z
	pos_polar->z = pos_cart->z;

	return;
}

// Calculate forward kinematics: joint angles -> polar coordinates
void calc_forward_kinematics(angles_t* joint_angles, coord_polar_t* pos_polar)
{
	/*
	 * Must use relative angles!
	 */

	// Calculate coord R based on the elbows' joint angles
	double r = l1 * cos(joint_angles->theta1) + l2 * cos(joint_angles->theta1 + joint_angles->theta2);

	// Calculate coord Z based on the elbows' joint angle
	double z = l1 * sin(joint_angles->theta1) + l2 * sin(joint_angles->theta1 + joint_angles->theta2);

	// Calculate angle based on given rotation around axis Z
	pos_polar->angle = joint_angles->theta0;

	// Correct gripper displacement and joint0 elevation, save values
	pos_polar->r = r + r3;
	pos_polar->z = z + z0 - z3;

	return;
}

// Calculate inverse kinematics: polar coordinates -> joint angles;
void calc_inverse_kinematics(coord_polar_t* pos_polar, angles_t* joint_angles)
{
	/*
	 * Returns with relative angles in radians
	 */

	// Correct gripper displacement and joint0 elevation
	double r = pos_polar->r - r3;
	double z = pos_polar->z - z0 + z3;

	// Calculate theta2
	double cos_theta2 = (pow(r, 2.0) + pow(z, 2.0)
			             - pow(l1, 2.0) - pow(l2, 2.0))
			             / (2.0 * l1 * l2);
	joint_angles->theta2 = atan2(sqrt(1.0 - pow(cos_theta2, 2.0)),
			                     cos_theta2);

	// Calculate theta1
	double k1 = l1 + l2 * cos_theta2;
	double k2 = l2 * sin(joint_angles->theta2);

	joint_angles->theta1 = atan2(z, r) - atan2(k2, k1);

	// Calculate theta0
	joint_angles->theta0 = pos_polar->angle;

	return;
}
