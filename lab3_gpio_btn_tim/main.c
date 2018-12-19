#include "main.h"

#define NUM_LEDS 3

static uint8_t WAIT_FLAG   = 0;
static uint8_t WAY_FLAG    = 1; //1 = 1,2,3 ; 0 = 3,2,1
static uint8_t current_led = 1;

void switch_led (uint8_t way)
{
  if (way == 0)
  	current_led--;
  else
    current_led++;

  if (current_led > NUM_LEDS) 
    current_led = 1;
  if (current_led == 0) 
    current_led = NUM_LEDS;

  GPIO_SetBits(GPIOA, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10); //all leds off
  switch (current_led)
  {
    case 1: GPIO_ResetBits (GPIOA, GPIO_Pin_8); break;
    case 2: GPIO_ResetBits (GPIOA, GPIO_Pin_9); break;
    case 3: GPIO_ResetBits (GPIOA, GPIO_Pin_10); break;
    default:;
  }
}

void TIM3_IRQHandler (void)
{
  if (TIM_GetITStatus (TIM3, TIM_IT_Update) != RESET)
  {
    WAIT_FLAG = 0;
    TIM_ClearITPendingBit (TIM3, TIM_IT_Update);
    TIM_Cmd (TIM3, DISABLE);
  }
}

void TIM2_IRQHandler (void)
{
  if (TIM_GetITStatus (TIM2, TIM_IT_Update) != RESET)
  {
    switch_led(WAY_FLAG);
    TIM_ClearITPendingBit (TIM2, TIM_IT_Update);
  }
}

//change direction
void EXTI0_IRQHandler (void)
{
  if (EXTI_GetITStatus (EXTI_Line0) != RESET)
  {
	if (WAIT_FLAG == 0)
	{
      WAY_FLAG = !!(WAY_FLAG != 1);
      WAIT_FLAG = 1;
      TIM_Cmd (TIM3, ENABLE);
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

  RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM3, ENABLE);
  TIM_InitStructure.TIM_Period = 200 - 1; //100 ms
  TIM_InitStructure.TIM_Prescaler = 42000 - 1; //500us
  TIM_InitStructure.TIM_ClockDivision = 0;
  TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit (TIM3, &TIM_InitStructure);

  /* Setup TIM2 interrupt */
  NVIC_InitTypeDef NVIC_InitStructure;

  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
 
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
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
  TIM_Cmd (TIM3, ENABLE);

  while (1)
  {

  }
}
