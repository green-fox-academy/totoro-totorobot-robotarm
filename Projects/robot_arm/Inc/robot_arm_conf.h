#ifndef __ROBOT_ARM_CONF_H_
#define __ROBOT_ARM_CONF_H_

#define BUTTONS			10

#define DRAW_X_ZERO_RECV		20
#define DRAW_Y_ZERO_RECV		30
#define DRAW_Z_ZERO_RECV		0
#define DRAW_X_MAX_RECV			376
#define DRAW_Y_MAX_RECV			260
#define DRAW_Z_MAX_RECV			0

#define DRAW_X_ZERO_CALC		120
#define DRAW_Y_ZERO_CALC		-60
#define DRAW_Z_ZERO_CALC		0
#define DRAW_X_MAX_CALC			180
#define DRAW_Y_MAX_CALC			29
#define DRAW_Z_MAX_CALC			0

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
