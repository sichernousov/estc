#include "main.h"

#define T_PERIOD 1000
#define T_PULSE  990

uint32_t cur_ch = 1;
uint32_t val = T_PERIOD - 2;

void EXTI0_IRQHandler (void)
{
  if (EXTI_GetITStatus (EXTI_Line0) != RESET)
  {
	if (val > 200)  val -= 200;
	else
		{
		  if (val > 10) val = 10;
		  else val = T_PERIOD - 2;
		}

	switch (cur_ch)
	{
	  case 1: TIM_SetCompare1 (TIM1, val); break;
	  case 2: TIM_SetCompare2 (TIM1, val); break;
	  case 3: TIM_SetCompare3 (TIM1, val); break;
	}

    EXTI_ClearITPendingBit (EXTI_Line0);
  }
}

void EXTI1_IRQHandler (void)
{
  if (EXTI_GetITStatus (EXTI_Line1) != RESET)
  {
	switch (cur_ch)
	{
	  case 1: TIM_SetCompare1 (TIM1, T_PULSE); break;
	  case 2: TIM_SetCompare2 (TIM1, T_PULSE); break;
	  case 3: TIM_SetCompare3 (TIM1, T_PULSE); break;
	}

	cur_ch++;
	if (cur_ch > 3) cur_ch = 1;

	switch (cur_ch)
	{
	  case 1: TIM_SetCompare1 (TIM1, val); break;
	  case 2: TIM_SetCompare2 (TIM1, val); break;
	  case 3: TIM_SetCompare3 (TIM1, val); break;
	}

    EXTI_ClearITPendingBit (EXTI_Line1);
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

  GPIO_PinAFConfig (GPIOA, GPIO_PinSource8, GPIO_AF_TIM1);
  GPIO_PinAFConfig (GPIOA, GPIO_PinSource9, GPIO_AF_TIM1);
  GPIO_PinAFConfig (GPIOA, GPIO_PinSource10, GPIO_AF_TIM1);

  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
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
  TIM_OCInitTypeDef TIM_OCInitStructure;

  // TIM1
  RCC_APB2PeriphClockCmd (RCC_APB2Periph_TIM1, ENABLE);
  TIM_InitStructure.TIM_Period = T_PERIOD - 1;
  TIM_InitStructure.TIM_Prescaler = 168 - 1;
  TIM_InitStructure.TIM_ClockDivision = 0;
  TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit (TIM1, &TIM_InitStructure);
 
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCNPolarity_Low;
  TIM_OCInitStructure.TIM_Pulse = T_PULSE - 1;

  TIM_OC1Init(TIM1, &TIM_OCInitStructure);
  TIM_OC2Init(TIM1, &TIM_OCInitStructure);
  TIM_OC3Init(TIM1, &TIM_OCInitStructure);

  TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
  TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
  TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);

  TIM_CtrlPWMOutputs (TIM1, ENABLE);

  /* Setup ext interrupts for button */
  RCC_APB2PeriphClockCmd (RCC_APB2Periph_SYSCFG, ENABLE);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource0);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource1);

  EXTI_InitTypeDef EXTI_InitStructure;
  EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line1;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Turn all the leds off */
  //GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
  //GPIO_SetBits(GPIOA, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10); //red, green, blue

  TIM_Cmd (TIM1, ENABLE);

  while (1)
  {
/*
	  val = 1;
	  while (val < 995)
	  {
		  for (i = 0; i < 2 *168000; i++);
		  val += 10;
		  TIM_SetCompare1 (TIM1, val);
		  TIM_SetCompare2 (TIM1, val);
		  TIM_SetCompare3 (TIM1, val);
	  }
*/
  }
}
