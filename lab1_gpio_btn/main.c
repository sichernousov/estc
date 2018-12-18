#include "main.h"

int main(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* LEDs array to toggle between them */
  /* LED to toggle during iteration */
  uint8_t  current_led = 0;

  /* Enable peripheral clock for LEDs port */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

  /* Init LEDs */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  /* Turn all the leds off */
  GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
  GPIO_SetBits(GPIOA, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10); //red, green, blue

  int GREEN_FLAG = 0;
  int RED_FLAG = 0;
 
  while (1)
  {
    if (GREEN_FLAG)	
       GPIO_ResetBits(GPIOA, GPIO_Pin_9);
    else
       GPIO_SetBits(GPIOA, GPIO_Pin_9);

    if (RED_FLAG)	
       GPIO_ResetBits(GPIOA, GPIO_Pin_8);
    else
       GPIO_SetBits(GPIOA, GPIO_Pin_8);

    if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_0) == 0)
    {
       while (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_0) == 0);
       GREEN_FLAG = !!(GREEN_FLAG != 1);
    }

    if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_1) == 0)
    {
       while (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_1) == 0);
       RED_FLAG = !!(RED_FLAG != 1);
    }
  }
}
