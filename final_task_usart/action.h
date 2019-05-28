
#ifndef ACTION_H
#define ACTION_H

#include <stm32f4xx.h>
#include "general_types.h"
#include "periph.h"
#include "MT_USART.h"

typedef struct {
  uint8_t  status;
  uint32_t bright;
} led_status_t;

typedef struct {
  uint8_t  status;
  uint32_t interval;
  uint32_t duration;
} tim_status_t;

typedef struct {
  led_status_t led1;
  led_status_t led2;
  led_status_t led3;
  tim_status_t tim;
} sys_status_t;

sys_status_t get_status (void);
void init_system (void);
bool set_bright (led_t led, uint8_t val);
void tim_start (void);
void tim_stop (void);
void tim_set_interval (uint16_t val);
void tim_set_duration (uint8_t val);

#endif
