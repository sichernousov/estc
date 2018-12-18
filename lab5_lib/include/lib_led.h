#ifndef LIB_LED_H
#define LIB_LED_H

#include <stm32f4xx.h>

typedef enum {
	eRed   = 0xFF0000,
	eGreen = 0x00FF00,
	eBlue  = 0x0000FF
} t_led_color;

void LedInit(void);
void LedSetColor (uint32_t rgb);
void LedTurnOff (void);

#endif
