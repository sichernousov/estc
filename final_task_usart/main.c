#include "main.h"

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

int main(void)
{
  //инициализация переферии
  init_system();

  //создания и инициализация входного буффера
  CREATE_INPUT_BUF(buf_in);
  clear_buf(buf_in);

  //включение приёма по USART
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
      send_cmd_set_bright (led2, 100);
      send_cmd_set_bright (led3, 100);
      for (int j = 0; j < 100000; j++); //debounce
    }

    //advance btn#1
    if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_1) == 0)
    {
      while (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_1) == 0);
      send_cmd_set_interval (TIM_PERIOD_1_SEC*2);
      send_cmd_set_duration (6);
      for (int j = 0; j < 100000; j++); //debounce
    }

    //если пришла команда, то обрабатываем её и ожидаем новую
    if (!MT_USART_WaitToReceive)
    {
        if (!do_cmd(buf_in)) send_cmd_wrong_cmd();
        clear_buf (buf_in);
        MT_USART_ReceiveData((uint8_t *)buf_in, END_CMD);
    }
  }
}
