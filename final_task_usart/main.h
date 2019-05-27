#ifndef LEDS_AND_BUTTON_H
#define LEDS_AND_BUTTON_H

#include <stm32f4xx.h>
#include <stdlib.h>

#include "general_types.h"

#include <periph.c> //ToDo
#include <MT_USART.c> //ToDo
#include <action.c> //ToDo

#define MAX_LEN_BUF  64
#define END_CMD      '\n'

typedef enum cmd_t{
  CMD_LedBrightSet   = '1',
  CMD_TimStart       = '2',
  CMD_TimStop        = '3',
  CMD_TimIntervalSet = '4',
  CMD_TimDurationSet = '5',
  CMD_GetStatus      = '6',
  CMD_RecieveStatus  = '7'
} cmd_t;

extern void TIM2_IRQHandler(void);
extern void TIM3_IRQHandler(void);

bool do_cmd (char * pbuf);
uint32_t get_next_param(char * pbuf, uint8_t * i);
void clear_buf (char * buf);

void send_cmd_set_bright (uint8_t led_num, uint8_t value);
void send_cmd_tim_start(void);
void send_cmd_tim_stop(void);
void send_cmd_set_interval (uint16_t value);
void send_cmd_set_duration(uint8_t value);
void send_cmd_get_status(void);

#endif  /* LEDS_AND_BUTTON_H */
