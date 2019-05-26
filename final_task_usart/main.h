#ifndef LEDS_AND_BUTTON_H
#define LEDS_AND_BUTTON_H

#include <stm32f4xx.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "general_types.h"

#include <periph.c> //ToDo
#include <MT_STM32F4xx_USART.c> //ToDo
#include <action.c> //ToDo

#define MAX_LEN_BUF  64
#define END_CMD      '\n'

typedef enum cmd_t{
  CMD_LedBrightSet   = '1',
  CMD_TimStart       = '2',
  CMD_TimStop        = '3',
  CMD_TimIntervalSet = '4',
  CMD_TimDurationSet = '5'
} cmd_t;

extern void EXTI0_IRQHandler(void);
extern void TIM2_IRQHandler(void);
extern void TIM3_IRQHandler(void);

void send_cmd(char* pbuf);
void recieve_cmd_from_usart(char* pbuf);
bool do_cmd (char * pbuf);
uint32_t get_next_param(char * pbuf, uint8_t * i);

#endif  /* LEDS_AND_BUTTON_H */
