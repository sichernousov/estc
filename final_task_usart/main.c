#include "main.h"
#include <action.h>

//const uint8_t transmit_data_byte = 'A';
//volatile uint8_t recieve_data_byte = 0;

void EXTI0_IRQHandler (void)
{
  //если нажата кнопка
  if (EXTI_GetITStatus (EXTI_Line0) != RESET)
  {
    //USART_ITConfig (USART1, USART_IT_TC, ENABLE);
    EXTI_ClearITPendingBit (EXTI_Line0);
  }
}

/*
void USART1_IRQHandler()
{
  if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)
  {
      GPIO_SetBits(GPIOD, GPIO_Pin_13);
      USART_ClearITPendingBit(USART1, USART_IT_TC);
      USART_SendData(USART1, transmit_data_byte);
      USART_ITConfig (USART1, USART_IT_TC, DISABLE);
  }

  if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
      USART_ClearITPendingBit(USART1, USART_IT_RXNE);
      GPIO_SetBits(GPIOD, GPIO_Pin_15);
      recieve_data_byte = USART_ReceiveData(USART1);
      if (recieve_data_byte == transmit_data_byte) GPIO_SetBits(GPIOD, GPIO_Pin_14);
      USART_ITConfig (USART1, USART_IT_RXNE, DISABLE);
  }
}
*/

int main(void)
{
  init_system();
  (void)set_bright(led1, 50);
  //set_bright(led2, 35);
  //set_bright(led3, 100);

  GPIO_SetBits(GPIOD, GPIO_Pin_15);
  //USART_ITConfig (USART1, USART_IT_RXNE, ENABLE);

  while (1)
  {

  }

}
