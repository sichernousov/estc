#ifndef PERIPH_C
#define PERIPH_C

#include <periph.h>

static const uint16_t g_pwm_tim_period = 300;
static uint16_t g_simple_tim_period = TIM_PERIOD_1_SEC;
static uint8_t g_dur_tim_sec = 1;

void init_leds(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_StructInit(&GPIO_InitStructure);

    //PWM leds
    GPIO_PinAFConfig (GPIOD, GPIO_PinSource12, GPIO_AF_TIM1);
    GPIO_PinAFConfig (GPIOD, GPIO_PinSource13, GPIO_AF_TIM1);
    GPIO_PinAFConfig (GPIOD, GPIO_PinSource14, GPIO_AF_TIM1);

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
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_StructInit(&GPIO_InitStructure);

    //Init BTN
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Setup ext interrupts for button */
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_SYSCFG, ENABLE);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void init_tim(void)
{
    //PWM Timer (T1)
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_TIM1, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_InitStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);

    TIM_InitStructure.TIM_Period = g_pwm_tim_period;
    TIM_InitStructure.TIM_Prescaler = 168 - 1;
    TIM_InitStructure.TIM_ClockDivision = 0;
    TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit (TIM1, &TIM_InitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCNPolarity_Low;
    TIM_OCInitStructure.TIM_Pulse = 0;

    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    TIM_OC2Init(TIM1, &TIM_OCInitStructure);
    TIM_OC3Init(TIM1, &TIM_OCInitStructure);

    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);

    TIM_CtrlPWMOutputs (TIM1, ENABLE);
    TIM_Cmd (TIM1, ENABLE);

/*
    //Simple Timer (T2)
    // TIM2
    RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM2, ENABLE);
    TIM_InitStructure.TIM_Period = g_simple_tim_period - 1;
    TIM_InitStructure.TIM_Prescaler = 42000 - 1; //500us
    TIM_InitStructure.TIM_ClockDivision = 0;
    TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit (TIM2, &TIM_InitStructure);

    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //Duration Timer (T3)
    RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM3, ENABLE);
    TIM_InitStructure.TIM_Period = (g_dur_tim_sec * TIM_PERIOD_1_SEC) - 1;
    TIM_InitStructure.TIM_Prescaler = 42000 - 1; //500us
    TIM_InitStructure.TIM_ClockDivision = 0;
    TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit (TIM3, &TIM_InitStructure);

    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    */
}

void init_usart(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_StructInit(&GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
    //TX
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //RX
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitTypeDef USART_InitStructure;
    USART_StructInit(&USART_InitStructure);
    USART_InitStructure.USART_BaudRate = 9600;// скорость
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //8 бит данных
    USART_InitStructure.USART_StopBits = USART_StopBits_1; //один стоп бит
    USART_InitStructure.USART_Parity = USART_Parity_No; //четность - нет
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // управлени потоком - нет
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // разрешаем прием и передачу
    USART_Init(USART1, &USART_InitStructure);

    NVIC_InitTypeDef NVIC_InitStructure;
    /* Enable the USARTx Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; //прерывание по uart1
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //задаем приоритет в группе
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //задаем приоритет в подгруппе
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //разрешаем прерывание
    NVIC_Init(&NVIC_InitStructure); //инициализируем

    NVIC_EnableIRQ(USART1_IRQn);
    USART_Cmd(USART1, ENABLE);
}

uint8_t set_pulse(led_t led, uint16_t value)
{
    if (value <= g_pwm_tim_period)
    {
        switch (led)
        {
          case led1:
            TIM_SetCompare1 (TIM1, value);
          break;

          case led2:
            TIM_SetCompare2 (TIM1, value);
          break;

          case led3:
            TIM_SetCompare3 (TIM1, value);
          break;
       }
    }
    else return ERROR_CONST;

    return OK_CONST;
}

uint16_t perc_to_pulse(uint8_t percents)
{
    return g_pwm_tim_period / 100 * percents;
}

void set_duration(uint8_t val)
{
   g_dur_tim_sec = val;
   if (val > 0) {
       TIM_SetCounter(TIM3, 0);
       TIM_SetPeriod(TIM3, (g_dur_tim_sec * TIM_PERIOD_1_SEC));
   }
}

void set_interval(uint16_t val)
{
    g_simple_tim_period = val;
    TIM_SetCounter(TIM2, 0);
    TIM_SetPeriod(TIM2,  g_simple_tim_period);
}

void pwm_tim_enable (void) {
    TIM_Cmd (TIM1, ENABLE);
}

void pwm_tim_disable (void) {
    TIM_Cmd (TIM1, DISABLE);
}

void simple_tim_enable (void) {
    TIM_Cmd (TIM2, ENABLE);
}

void simple_tim_disable (void) {
    TIM_Cmd (TIM1, DISABLE);
}

void duration_tim_enable (void) {
    TIM_Cmd (TIM3, ENABLE);
}

void duration_tim_disable (void) {
    TIM_Cmd (TIM3, DISABLE);
}

#endif
