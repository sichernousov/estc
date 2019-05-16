#ifndef LEDS_AND_BUTTON_H
#define LEDS_AND_BUTTON_H

#include <stm32f4xx.h>
#include <stdlib.h>
#include <string.h>

#include <periph.c>

#include <action.c>

#define LEN_DATA_CMD    1
#define LEN_DATA_PARAM  3
#define MAX_NUM_PACKETS	16

typedef enum cmd_t{
  CMD_LedBrightSet   = 0x01,
  CMD_LedBrightInc   = 0x02,
  CMD_LedBrightDec   = 0x03,
  CMD_TimStart       = 0x04,
  CMD_TimStop        = 0x05,
  CMD_TimIntervalSet = 0x06,
  CMD_TimIntervalInc = 0x07,
  CMD_TimIntervalDec = 0x08,
  CMD_TimDurationSet = 0x09,
  CMD_TimDurationInc = 0x0A,
  CMD_TimDurationDec = 0x0B
} cmd_t;

typedef struct {
  uint8_t  cmd;
  uint8_t  params[LEN_DATA_PARAM];
} packet_t;
#define LEN_DATA_PACKET (sizeof(packet_t))

//input bufs and vars
static packet_t * g_usart_input_buf[MAX_NUM_PACKETS] = {0};
static uint8_t g_cur_in_index = 0;
static uint8_t g_packs_for_proc = 0;

//output bufs and vars
static packet_t * g_usart_output_buf[MAX_NUM_PACKETS] = {0};
static uint8_t g_cur_out_index = 0;
static uint8_t g_packs_for_send = 0;

//funcs
uint8_t   do_cmd (packet_t * packet);
packet_t* get_next_packet (void);
void send_packet(packet_t* p);

/* #if defined (USE_STM324xG_EVAL) */
/*   #include "stm324xg_eval.h" */

/* #elif defined (USE_STM32F4DISC_EVAL) */
/*   #include "stm32f4disc_eval.h" */

/* #else */
/*  #error "Please select first the Evaluation board used in your application (in Project Options)" */
/* #endif */

#include <math.h>

#endif  /* LEDS_AND_BUTTON_H */
