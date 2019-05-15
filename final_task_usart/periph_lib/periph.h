#ifndef PERIPH_H
#define PERIPH_H

#include <stm32f4xx.h>

#define OK_CONST    0
#define ERROR_CONST 1
#define TIM_PERIOD_1_SEC 2000

typedef enum led_t{
  led1 = 1,
  led2 = 2,
  led3 = 3
} led_t;

void init_leds(void);
void init_btns(void);
void init_tim(void);
void init_usart(void);
uint8_t set_pulse(led_t led, uint16_t value);
void set_duration(uint8_t val);
void set_interval(uint16_t val);
uint16_t perc_to_pulse(uint8_t percents);

void pwm_tim_enable (void);
void pwm_tim_disable (void);
void simple_tim_enable (void);
void simple_tim_disable (void);
void duration_tim_enable (void);
void duration_tim_disable (void);

#endif
