#include "main.h"

static uint8_t TIM_FLAG = 1;
static uint8_t current_led = 0;

void switch_led (void)
{
  current_led++;
  if (current_led >= 3) current_led = 0;
  GPIO_SetBits(GPIOA, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10);
  switch (current_led)
  {
    case 0: GPIO_ResetBits (GPIOA, GPIO_Pin_8); break;
    case 1: GPIO_ResetBits (GPIOA, GPIO_Pin_9); break;
    case 2: GPIO_ResetBits (GPIOA, GPIO_Pin_10); break;
  }
}

void TIM2_IRQHandler (void)
{
  if (TIM_GetITStatus (TIM2, TIM_IT_Update) != RESET)
  {
    switch_led();
    TIM_ClearITPendingBit (TIM2, TIM_IT_Update);
  }
}

void EXTI0_IRQHandler (void)
{
  if (EXTI_GetITStatus (EXTI_Line0) != RESET)
  {
    TIM_FLAG = !!(TIM_FLAG != 1);
    if (TIM_FLAG) 
    {
      switch_led();
      TIM2->CNT = 0;
      TIM_Cmd (TIM2, ENABLE);
    }
    else 
    {
      TIM_Cmd (TIM2, DISABLE);
      GPIO_SetBits(GPIOA, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10);
    }
    EXTI_ClearITPendingBit (EXTI_Line0);
  }
}

int main(void)
{
  /* Init GPIO */
  GPIO_InitTypeDef GPIO_InitStructure;

  //GPIOD, chip led
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  //GPIOA, advance led
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  //GPIOE, 2 buttons
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
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

  /* Setup TIM2 interrupt */
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
 
  /* Setup ext interrupts for button */
  RCC_APB2PeriphClockCmd (RCC_APB2Periph_SYSCFG, ENABLE);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource0);

  EXTI_InitTypeDef EXTI_InitStructure;
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Turn all the leds off */
  GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
  GPIO_SetBits(GPIOA, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10); //red, green, blue

  TIM_Cmd (TIM2, ENABLE);
  while (1)
  {

  }
}
