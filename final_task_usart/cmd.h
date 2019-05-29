#ifndef CMD_H
#define CMD_H

#include <stm32f4xx.h>
#include <stdlib.h>

#include "general_types.h"

#include "periph.h" 
#include "MT_USART.h" 
#include "action.h" 

#define MAX_LEN_BUF  64
#define END_CMD      '\n'

#define CREATE_INPUT_BUF(NAME) char NAME[MAX_LEN_BUF+1]

typedef enum cmd_t{
  CMD_LedBrightSet   = '1',
  CMD_TimStart       = '2',
  CMD_TimStop        = '3',
  CMD_TimIntervalSet = '4',
  CMD_TimDurationSet = '5',
  CMD_GetStatus      = '6',
  CMD_RecieveStatus  = '7',
  CMD_WrongCmd       = '8'
} cmd_t;


bool do_cmd (char * pbuf);

void send_cmd_set_bright (uint8_t led_num, uint8_t value);
void send_cmd_tim_start(void);
void send_cmd_tim_stop(void);
void send_cmd_set_interval (uint16_t value);
void send_cmd_set_duration(uint8_t value);
void send_cmd_get_status(void);
void send_status(sys_status_t * pstat);
void send_cmd_wrong_cmd(void);

void clear_buf (char * buf);
uint32_t get_next_param(char * pbuf, uint8_t * i);

#endif
