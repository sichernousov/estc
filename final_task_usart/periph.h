#ifndef PERIPH_H
#define PERIPH_H

#include <stm32f4xx.h>
#include "general_types.h"

#define TIM_PERIOD_1_SEC 500
#define MAX_DURATION     15

void init_leds(void);
void init_btns(void);
void init_tim(void);

bool set_pulse(led_t led, uint16_t value);
void set_duration(uint8_t val);
void set_interval(uint16_t val);
uint16_t perc_to_pulse(uint8_t percents);

void pwm_tim_enable (void);
void pwm_tim_disable (void);
void interval_tim_enable (void);
void interval_tim_disable (void);
void duration_tim_enable (void);
void duration_tim_disable (void);

#endif
