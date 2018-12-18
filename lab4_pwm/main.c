#include "main.h"

#define T_PERIOD 1000
#define T_PULSE  999

#define NUM_CHANNELS  3
#define MIN_BRIGHT    T_PULSE
#define MAX_BRIGHT    1
#define MAX_STEPS	  5

uint32_t current_channel = 1;
uint32_t current_bright  = MIN_BRIGHT; 
uint32_t step = (uint32_t) (T_PULSE / MAX_STEPS);

void set_color_bright (uint32_t channel, uint16_t value)
{
	switch (channel)
	{
	  case 1: TIM_SetCompare1 (TIM1, value); 
		break;
	  case 2: TIM_SetCompare2 (TIM1, value); 	
		break;
	  case 3: TIM_SetCompare3 (TIM1, value); 
		break;
	  default:;
	}
}

void clear_all_channels (void)
{
	for (int i = 1; i <= NUM_CHANNELS; i++)
	  set_color_bright (i, MIN_BRIGHT);
}

//btn0: change bright
void EXTI0_IRQHandler (void) 
{
  if (EXTI_GetITStatus (EXTI_Line0) != RESET)
  {
	if (current_bright > step)  
	  current_bright -= step;
	else
	  current_bright = T_PULSE;

	set_color_bright (current_channel, current_bright);

    EXTI_ClearITPendingBit (EXTI_Line0);
  }
}

//btn1: switch led
void EXTI1_IRQHandler (void) 
{
  if (EXTI_GetITStatus (EXTI_Line1) != RESET)
  {
	clear_all_channels ();

	current_channel++;
	if (current_channel > NUM_CHANNELS) current_channel = 1;

	set_color_bright (current_channel, current_bright);

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
	
  TIM_OCStructInit(TIM_OCInitStructure);

  // TIM1
  RCC_APB2PeriphClockCmd (RCC_APB2Periph_TIM1, ENABLE);
  TIM_InitStructure.TIM_Period = T_PERIOD;
  TIM_InitStructure.TIM_Prescaler = 168 - 1;
  TIM_InitStructure.TIM_ClockDivision = 0;
  TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit (TIM1, &TIM_InitStructure);
 
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCNPolarity_Low;
  TIM_OCInitStructure.TIM_Pulse = T_PULSE;

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

  TIM_Cmd (TIM1, ENABLE);

  while (1)
  {

  }
}
