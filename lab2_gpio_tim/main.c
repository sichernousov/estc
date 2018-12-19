#include "main.h"

#define NUM_LED 4

static uint8_t current_led = 1;
static uint8_t WAY_FLAG = 1;

static void switch_led(uint8_t direction)
{
	GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
	if (direction == 1)
	{
		current_led++;
		if (current_led > NUM_LED) current_led = 1;
	}
	else
	{
		current_led--;
		if (current_led == 0) current_led = NUM_LED;
	}
	switch (current_led)
	{
	  case 1: GPIO_SetBits(GPIOD, GPIO_Pin_12); break;
	  case 2: GPIO_SetBits(GPIOD, GPIO_Pin_13); break;
	  case 3: GPIO_SetBits(GPIOD, GPIO_Pin_14); break;
	  case 4: GPIO_SetBits(GPIOD, GPIO_Pin_15); break;
	}
}

static void wait_sec(void)
{
    while (!TIM_GetFlagStatus (TIM2, TIM_FLAG_Update) && (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_0) != 0));
	if (TIM_GetFlagStatus (TIM2, TIM_FLAG_Update))
	{
      TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	}
	else
	{
	  while (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_0) == 0);
	  WAY_FLAG = !!(WAY_FLAG != 1);
	  TIM_SetCounter(TIM2, 0);
	}

}

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

  //Init buttons
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  /* Init Timers */
  TIM_TimeBaseInitTypeDef TIM_InitStructure;

  // TIM2
  RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM2, ENABLE);
  TIM_InitStructure.TIM_Period = 2000 - 1; //1sec
  TIM_InitStructure.TIM_Prescaler = 42000 - 1; //500us
  TIM_InitStructure.TIM_ClockDivision = 0;
  TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit (TIM2, &TIM_InitStructure);

  /* Turn all the leds off */
  GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
  GPIO_SetBits(GPIOA, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10); //red, green, blue

  TIM_Cmd (TIM2, ENABLE);

  while (1)
  {
    //если нажата кнопка
    switch_led(WAY_FLAG);
    wait_sec(); //and btn handler
  }
}
