#ifndef __ROBOT_ARM_CONF_H_
#define __ROBOT_ARM_CONF_H_

#define BUTTONS		10


typedef struct {
	uint16_t x;
	uint16_t y;
	uint8_t width;
	uint8_t height;
	uint32_t btn_color0;
	uint32_t btn_color1;
	uint32_t text_color0;
	uint32_t text_color1;
	uint16_t text_x0;
	uint16_t text_x1;
	uint16_t text_y;
	char text0[10];
	char text1[10];
	uint8_t touchable;
	uint8_t state;
} button_t;

#endif /* __ROBOT_ARM_CONF_H_ */
