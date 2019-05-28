#include "periph.h"

static const uint16_t g_pwm_tim_period = 40000;
static uint16_t g_interval_tim_period = TIM_PERIOD_1_SEC;
static uint8_t g_dur_tim_sec = 10;

void init_leds(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_StructInit(&GPIO_InitStructure);

    //PWM leds
    GPIO_PinAFConfig (GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
    GPIO_PinAFConfig (GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);
    GPIO_PinAFConfig (GPIOD, GPIO_PinSource14, GPIO_AF_TIM4);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    //TIM led
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOD, GPIO_Pin_15);
}

void init_btns(void)
{ 
    GPIO_InitTypeDef GPIO_InitStructure;

    //Init BTNS
    //CHIP
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //ADVANCE
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    GPIO_StructInit(&GPIO_InitStructure);   
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void init_tim(void)
{
    TIM_TimeBaseInitTypeDef TIM_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    //Interval Timer (TIM2)
    RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM2, ENABLE);
    TIM_InitStructure.TIM_Period = g_interval_tim_period - 1;
    TIM_InitStructure.TIM_Prescaler = 42000 - 1; //500us
    TIM_InitStructure.TIM_ClockDivision = 0;
    TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit (TIM2, &TIM_InitStructure);
    TIM2->CR1 = TIM2->CR1 & ~(1 << 7); 

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    //Duration Timer (TIM3)
    RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM3, ENABLE);
    TIM_InitStructure.TIM_Period = (g_dur_tim_sec * TIM_PERIOD_1_SEC) - 1;
    TIM_InitStructure.TIM_Prescaler = 42000 - 1; //500us
    TIM_InitStructure.TIM_ClockDivision = 0;
    TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit (TIM3, &TIM_InitStructure);
    TIM3->CR1 = TIM3->CR1 & ~(1 << 7);

    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
     
    //PWM Timer (TIM4)
    RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM4, ENABLE);

    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);

    TIM_InitStructure.TIM_Period = g_pwm_tim_period + 1;
    TIM_InitStructure.TIM_Prescaler = 4 - 1; //168-1
    TIM_InitStructure.TIM_ClockDivision = 0;
    TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit (TIM4, &TIM_InitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCNPolarity_Low;
    TIM_OCInitStructure.TIM_Pulse = g_pwm_tim_period;

    TIM_OC1Init(TIM4, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

    TIM_OC2Init(TIM4, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);

    TIM_OC3Init(TIM4, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);

    TIM_CtrlPWMOutputs (TIM4, ENABLE);
    TIM_Cmd (TIM4, ENABLE);
}

bool set_pulse(led_t led, uint16_t value)
{
    if (value <= g_pwm_tim_period)
    {
        switch (led)
        {
          case led1:
            TIM_SetCompare1 (TIM4, value);
          break;

          case led2:
            TIM_SetCompare2 (TIM4, value);
          break;

          case led3:
            TIM_SetCompare3 (TIM4, value);
          break;
       }
    }
    else return FALSE;

    return TRUE;
}

uint16_t perc_to_pulse(uint8_t percents)
{
    return (g_pwm_tim_period / 100 * percents);
}

void set_interval(uint16_t val)
{
    g_interval_tim_period = val;
    TIM2->ARR = g_interval_tim_period;
    TIM2->CNT = 0;
}

void set_duration(uint8_t val)
{
   g_dur_tim_sec = val;
   if (val > 0) {
       TIM3->ARR = (g_dur_tim_sec * TIM_PERIOD_1_SEC) - 1;
       TIM3->CNT = 0;
   }
}

void pwm_tim_enable (void) {
    TIM_Cmd (TIM4, ENABLE);
}

void pwm_tim_disable (void) {
    TIM_Cmd (TIM4, DISABLE);
}

void interval_tim_enable (void) {
    //TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM2->CNT = 0;
    TIM_Cmd (TIM2, ENABLE);
}

void interval_tim_disable (void) {
    //TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
    TIM_Cmd (TIM2, DISABLE);
}

void duration_tim_enable (void) {
    //TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    TIM3->CNT = 0;
    TIM_Cmd (TIM3, ENABLE);
}

void duration_tim_disable (void) {
    //TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
    TIM_Cmd (TIM3, DISABLE);
}

