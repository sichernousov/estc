#include "main.h"

void EXTI0_IRQHandler (void)
{
  //если нажата кнопка
  if (EXTI_GetITStatus (EXTI_Line0) != RESET)
  {
    GPIO_ToggleBits(GPIOD, GPIO_Pin_15);
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
    TIM_Cmd(TIM2, DISABLE);
    TIM_Cmd(TIM3, DISABLE);  
  }
}

void USART1_IRQHandler()
{
  if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)
  {
      USART_ClearITPendingBit(USART1, USART_IT_TC);
  }

  if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
      USART_ClearITPendingBit(USART1, USART_IT_RXNE);
   }
}


int main(void)
{
  init_system();

  //USART_ITConfig (USART1, USART_IT_RXNE, ENABLE);
  uint8_t perc = 0;
  while (1)
  {
    (void)set_bright(led1, perc);
    (void)set_bright(led2, perc);
    (void)set_bright(led3, perc);

    for (volatile int i = 0; i<350000; i++);
    perc += 3;
    if (perc >= 100) perc = 0;
    (void)set_bright(led1, perc);
    (void)set_bright(led2, perc);
    (void)set_bright(led3, perc);

    for (volatile int i = 0; i<350000; i++);
  }

}
