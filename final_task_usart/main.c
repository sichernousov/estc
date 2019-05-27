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

//функция возвращает следующее (с индекса i) числовое значение из символьного буфера
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
  //выполняем команду из символьного буфера
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

void send_cmd_set_bright (uint8_t led_num, uint8_t value)
{
  char buf_out[16];
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
  char buf_out[16];
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
  char buf_out[16];
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

  MT_USART_ReceiveData((uint8_t *)buf_in, END_CMD);
  while (1)
  {
    //ждем приема пакета или нажатия любой из кнопки
    while (MT_USART_WaitToReceive && (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0) && (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_1) == 1) 
            && (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_0) == 1));

    //обработчики кнопок 
    //chip btn
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1)
    {
      while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1);
      send_cmd_tim_start();
      for (int j = 0; j < 100000; j++); //debounce
    }

    //advance btn#0
    if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_0) == 0)
    {
      while (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_0) == 0);   
      send_cmd_set_bright (led1, 100);
      for (int j = 0; j < 100000; j++); //debounce
    }

    //advance btn#1
    if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_1) == 0)
    {
      while (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_1) == 0);
      send_cmd_set_bright (led3, 100);
      for (int j = 0; j < 100000; j++); //debounce
    }

    //если пришла команда, то обрабатываем её и ожидаем новую
    if (!MT_USART_WaitToReceive)
    {
        do_cmd(buf_in);
        clear_buf (buf_in);
        MT_USART_ReceiveData((uint8_t *)buf_in, END_CMD);
    }
  }
}
