#include "main.h"

void send_packet(packet_t* p)
{
  g_usart_output_buf[g_cur_out_index] = p;
  g_cur_out_index++;
  g_packs_for_send++;
  if (g_packs_for_send == 1) USART_ITConfig(USART1, USART_IT_TC, ENABLE);
};

void EXTI0_IRQHandler (void)
{
  //если нажата кнопка
  if (EXTI_GetITStatus (EXTI_Line0) != RESET)
  {
    static uint8_t debounce = 0;
    if (debounce == 0)
    {
      debounce = 1;
      packet_t * p = (packet_t *) malloc(LEN_DATA_PACKET);
      p->cmd = CMD_TimStart;
      memset(p->params, 0, LEN_DATA_PARAM);
      send_packet(p);
    }
    //USART_ITConfig (USART1, USART_IT_TC, ENABLE);
    EXTI_ClearITPendingBit (EXTI_Line0);
  }
}

//interval timer handler
void TIM2_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    GPIO_ToggleBits(GPIOD, GPIO_Pin_15);
  }
}

//duration timer handler
void TIM3_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    tim_stop();
  }
}

void USART1_IRQHandler()
{
  if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)
  {
      static uint8_t SEND_PROGRESS = 0;
      static uint8_t send_data_cnt = 0;
      static uint8_t * p_out = NULL;
      static uint8_t out_index = 0;

      if (g_packs_for_send > 0)
      {
        //начало отправки нового пакета
        if (SEND_PROGRESS == 0) 
        {
          send_data_cnt = 0;
	  SEND_PROGRESS = 1;

          p_out = (uint8_t *) malloc(LEN_DATA_PACKET);
          memcpy(p_out, g_usart_output_buf[out_index], LEN_DATA_PACKET);
          free(g_usart_output_buf[out_index]);
          out_index++;
          if (out_index == MAX_NUM_PACKETS) out_index = 0;

          USART_SendData(USART1, *(p_out + send_data_cnt));
          send_data_cnt++;
        }
        else 
        {
          //доотправка
          USART_SendData(USART1, *(p_out + send_data_cnt));
          send_data_cnt++;
          if (send_data_cnt == LEN_DATA_PACKET)
          {
            free(p_out);
            SEND_PROGRESS = 0;
            g_packs_for_send--;
          }         
        }
      }
      else USART_ITConfig(USART1, USART_IT_TC, DISABLE);//???hz

      USART_ClearITPendingBit(USART1, USART_IT_TC);
  }

  if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
      static uint8_t REC_PROGRESS = 0;
      static uint8_t rec_data_cnt = 0;
      static uint8_t * p_in = NULL;
      static uint8_t in_index = 0;

      //начинаем принимать новый пакет
      if (REC_PROGRESS == 0)
      {
        rec_data_cnt = 0;
	REC_PROGRESS = 1;

        p_in = (uint8_t *) malloc(LEN_DATA_PACKET);

        *(p_in + rec_data_cnt) = USART_ReceiveData(USART1);
        rec_data_cnt++;
      }
      else 
      {
        //дополучаем
        *(p_in + rec_data_cnt) = USART_ReceiveData(USART1);
        rec_data_cnt++;
        if (rec_data_cnt == LEN_DATA_PACKET)
        {
          g_usart_input_buf[in_index] = (packet_t*) p_in;
          in_index++;
          if (in_index == MAX_NUM_PACKETS) in_index = 0;
          g_packs_for_proc++;
          REC_PROGRESS = 0;
        }
      }
	
      USART_ClearITPendingBit(USART1, USART_IT_RXNE);
  }
}

uint8_t do_cmd (packet_t * packet)
{ 
  uint8_t res = OK_CONST; 
  switch (packet->cmd)
  {
    case CMD_LedBrightSet:
      res = set_bright (packet->params[0], packet->params[1]);
    break;

    case CMD_LedBrightInc:
      ;//ToDo
    break;

    case CMD_LedBrightDec:
      ;//ToDo
    break;

    case CMD_TimStart:
      tim_start();
    break;

    case CMD_TimStop:
      tim_stop();
    break;

    case CMD_TimIntervalSet:
      tim_set_interval(*((uint16_t *) packet->params));
    break;

    case CMD_TimIntervalInc:
      ;//ToDo
    break;

    case CMD_TimIntervalDec:
      ;//ToDo
    break;

    case CMD_TimDurationSet:
      tim_set_duration (packet->params[0]);
    break;

    case CMD_TimDurationInc:
      ;//ToDo
    break;

    case CMD_TimDurationDec:
      ;//ToDo
    break;

    default:
      res = ERROR_CONST;
    break;
  }
  free(packet);

  return res;
}

packet_t * get_next_packet ()
{
  while (g_packs_for_proc == 0);
  g_packs_for_proc--;

  packet_t * res = (packet_t *) malloc(LEN_DATA_PACKET);
  memcpy(res, g_usart_input_buf[g_cur_in_index], LEN_DATA_PACKET);
 
  g_cur_in_index++;
  if (g_cur_in_index == MAX_NUM_PACKETS) g_cur_in_index = 0;

  return res;
}

int main(void)
{
  init_system();

  //USART_ITConfig (USART1, USART_IT_RXNE, ENABLE);
  while (1)
  {
    do_cmd (get_next_packet ());
  }

}
