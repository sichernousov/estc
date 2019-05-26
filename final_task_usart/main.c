#include "main.h"

char buf_in [MAX_LEN_BUF+1];

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

uint32_t get_next_param(char * pbuf, uint8_t * i)
{
  char tmp_buf[MAX_LEN_BUF+1];
  char * tmp_ptr = pbuf + (*i);
  uint8_t ind = *i;
  uint32_t res = 0;

  //если индекс некорректный
  if (ind >= MAX_LEN_BUF)
    return res;

  //пропускаем символы не цифры
  while ( (ind < MAX_LEN_BUF) && ((*tmp_ptr < '0') || (*tmp_ptr > '9')) )
  {
      tmp_ptr++;
      ind++;
  }

  //если цифр не обнаружили
  if (ind >= MAX_LEN_BUF)
    return res;

  //копируем последовательность цифр в tmp_buf
  uint8_t j = 0;
  while ((ind < MAX_LEN_BUF) && (*tmp_ptr >= '0') && (*tmp_ptr <= '9'))
  {
    tmp_buf[j] = *tmp_ptr;
    ind++;
    j++;
    tmp_ptr++;
  }
  //преобразуем последовательность цифр в число
  tmp_buf[j] = '\0';
  res = atoi (tmp_buf);
  return res;
}

bool do_cmd (char * pbuf)
{
  bool res = TRUE;
  switch (*pbuf)
  {
    case CMD_LedBrightSet:
    {
      uint8_t ind = 1;
      uint32_t led_num = get_next_param(pbuf, &ind);
      if (led_num == 0) GPIO_SetBits(GPIOD, GPIO_Pin_15);
      uint32_t led_bright = get_next_param(pbuf, &ind);
      if ((led_num > 0) && (led_num <= 3) && (led_bright <= 100))
        set_bright ((uint8_t) led_num, (uint8_t) led_bright);
      else
        return FALSE;
    }
    break;

    case CMD_TimStart:
      tim_start();
    break;

    case CMD_TimStop:
      tim_stop();
    break;

    case CMD_TimIntervalSet:
    {
      uint8_t ind = 1;
      uint32_t interval = get_next_param(pbuf, &ind);
      if (interval <= 0xFFFF)
        tim_set_interval((uint16_t)interval);
      else return FALSE;
    }
    break;

    case CMD_TimDurationSet:
    {
      uint8_t ind = 1;
      uint32_t duration = get_next_param(pbuf, &ind);
      if (duration <= MAX_DURATION)
        tim_set_duration ((uint8_t)duration);
      else return FALSE;
    }
    break;

    default:
      set_bright(led1, 100);
      res = FALSE;
    break;
  }
  return res;
}

void clear_buf (char * buf) //because memset doesnt work
{
    for (int i = 0; i < MAX_LEN_BUF; i++)
        *(buf+i) = '\0';
    *(buf+MAX_LEN_BUF) = END_CMD;
}

int main(void)
{
  init_system();

  clear_buf(buf_in);

  //test arrays
  char buf_out[] = "2\n";
  //char buf_out[] = "1 1 99\n";
  //char buf_out[] = "1 2 99\n";
  //char buf_out[] = "1 3 99\n";

  MT_USART_ReceiveData(&buf_in, END_CMD);
  while (1)
  {
    while(MT_USART_WaitToReceive && (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0));

    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1)
    {
      while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1);

      USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
      MT_USART_SendData(&buf_out, END_CMD);
      while(MT_USART_WaitToTransmit);
      USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

      for (int j = 0; j < 100000; j++); //debounce
    }

    if (!MT_USART_WaitToReceive)
    {
        do_cmd(buf_in);
        clear_buf (buf_in);
        MT_USART_ReceiveData(&buf_in, END_CMD);
    }
  }
}
