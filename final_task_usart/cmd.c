#include "cmd.h"

//функция возвращает следующее (с индекса i) числовое значение из символьного буфера
uint32_t get_next_param(char * pbuf, uint8_t * i)
{
  char tmp_buf[MAX_LEN_BUF+1];
  char * tmp_ptr = pbuf + (*i);
  uint32_t res = 0;

  //если индекс некорректный
  if (*i >= MAX_LEN_BUF)
    return res;

  //пропускаем символы не цифры
  while ( (*i < MAX_LEN_BUF) && ((*tmp_ptr < '0') || (*tmp_ptr > '9')) )
  {
      tmp_ptr++;
      (*i)++;
  }

  //если цифр не обнаружили
  if (*i >= MAX_LEN_BUF)
    return res;

  //копируем последовательность цифр в tmp_buf
  uint8_t j = 0;
  while ((*i < MAX_LEN_BUF) && (*tmp_ptr >= '0') && (*tmp_ptr <= '9'))
  {
    tmp_buf[j] = *tmp_ptr;
    (*i)++;
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
  //выполняем команду из символьного буфера
  bool res = TRUE;
  switch (*pbuf)
  {
    case CMD_LedBrightSet:
    {
      uint8_t ind = 1;
      uint32_t led_num = get_next_param(pbuf, &ind);
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

    case CMD_GetStatus:
    {
     //ToDo
    }
    break;

    case CMD_RecieveStatus:
    {
     //ToDo
    }
    break;

    case CMD_WrongCmd:
    {
     //если вторая плата просигнализировала об ошибке:
     while (1)
     {
       set_bright(led1, 100);
       set_bright(led2, 100);
       set_bright(led3, 100);
      
       for (int i = 0; i < 500000; i++);

       set_bright(led1, 0);
       set_bright(led2, 0);
       set_bright(led3, 0);

       for (int i = 0; i < 500000; i++);
     }
    }
    break;

    default:
      set_bright(led1, 100);
      res = FALSE;
    break;
  }
  return res;
}

void send_cmd_set_bright (uint8_t led_num, uint8_t value)
{
  char buf_out[MAX_LEN_BUF];
  clear_buf(buf_out);
  uint8_t i = 0; 
  
  buf_out[i] = CMD_LedBrightSet; 
  i++;
  
  buf_out[i] = ' '; 
  i++;
   
  itoa(led_num, &(buf_out[i]), 10);
  while ((buf_out[i] >= '0') && (buf_out[i] <= '9')) i++;

  buf_out[i] = ' ';
  i++;

  itoa(value, &(buf_out[i]), 10);
  while ((buf_out[i] >= '0') && (buf_out[i] <= '9')) i++;

  buf_out[i] = END_CMD;

  USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
  MT_USART_SendData((uint8_t *)buf_out, END_CMD);
  while(MT_USART_WaitToTransmit);
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}

void send_cmd_tim_start(void)
{
  char buf_out[2];
  buf_out[0] = CMD_TimStart;
  buf_out[1] = END_CMD;

  USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
  MT_USART_SendData((uint8_t *)buf_out, END_CMD);
  while(MT_USART_WaitToTransmit);
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}

void send_cmd_tim_stop(void)
{
  char buf_out[2];
  buf_out[0] = CMD_TimStop;
  buf_out[1] = END_CMD;

  USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
  MT_USART_SendData((uint8_t *)buf_out, END_CMD);
  while(MT_USART_WaitToTransmit);
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}

void send_cmd_set_interval (uint16_t value)
{
  char buf_out[MAX_LEN_BUF];
  clear_buf(buf_out);
  uint8_t i = 0; 

  buf_out[i] = CMD_TimIntervalSet;
  i++;

  buf_out[i] = ' ';
  i++;

  itoa(value, &(buf_out[i]), 10);
  while ((buf_out[i] >= '0') && (buf_out[i] <= '9')) i++;
  buf_out[i] = END_CMD;

  USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
  MT_USART_SendData((uint8_t *)buf_out, END_CMD);
  while(MT_USART_WaitToTransmit);
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}

void send_cmd_set_duration(uint8_t value)
{
  char buf_out[MAX_LEN_BUF+1];
  clear_buf(buf_out);
  uint8_t i = 0; 

  buf_out[i] = CMD_TimDurationSet;
  i++;

  buf_out[i] = ' ';
  i++; 

  itoa(value, &(buf_out[i]), 10);
  while ((buf_out[i] >= '0') && (buf_out[i] <= '9')) i++;
  buf_out[i] = END_CMD;

  USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
  MT_USART_SendData((uint8_t *)buf_out, END_CMD);
  while(MT_USART_WaitToTransmit);
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}

void send_cmd_get_status(void)
{
  char buf_out[2];
  buf_out[0] = CMD_GetStatus;
  buf_out[1] = END_CMD; 
  
  USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
  MT_USART_SendData((uint8_t *)buf_out, END_CMD);
  while(MT_USART_WaitToTransmit);
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
  
}

void send_cmd_wrong_cmd(void)
{
  char buf_out[2];
  buf_out[0] = CMD_WrongCmd;
  buf_out[1] = END_CMD; 
  
  MT_USART_SendData((uint8_t *)buf_out, END_CMD);
  while(MT_USART_WaitToTransmit);
}

void clear_buf (char * buf)
{
    for (int i = 0; i < MAX_LEN_BUF; i++)
        *(buf+i) = '\0';
    *(buf+MAX_LEN_BUF) = END_CMD;
}
