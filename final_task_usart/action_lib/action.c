#include <action.h>

static sys_status_t system_status;

//функция возвращения статуса системы
sys_status_t get_status (void)
{
  return system_status;
}

//функция инициализация переферии
void init_system (void)
{
  //init periph
  init_leds();
  init_btns();
  init_tim();
  MT_USART_Init();

  MT_USART_WaitToTransmit = FALSE;
  MT_USART_WaitToReceive = FALSE;

  //init struct
  system_status.led1.status = 0;
  system_status.led1.bright = 0;

  system_status.led2.status = 0;
  system_status.led2.bright = 0;

  system_status.led3.status = 0;
  system_status.led3.bright = 0;

  system_status.tim.status = 0;
  system_status.tim.interval = TIM_PERIOD_1_SEC;
  system_status.tim.duration = 0;

  pwm_tim_enable();
}

//установка уровня яркости (в %) для выбранного светодиода
bool set_bright (led_t led, uint8_t val)
{
  if (val > 100) return FALSE;
  uint8_t tmp_val = 100 - val;
  if (set_pulse(led, perc_to_pulse(tmp_val))) {
      switch (led)
      {
        case led1:
          system_status.led1.bright = val;
        break;

        case led2:
          system_status.led2.bright = val;
        break;

        case led3:
          system_status.led3.bright = val;
        break;

        default: return FALSE;
      }
  }
  else return FALSE;

  return TRUE;
}

//запуск таймеров для мигания диода
void tim_start (void)
{
  system_status.tim.status = 1;
  interval_tim_enable();
  if (system_status.tim.duration > 0)
      duration_tim_enable();
}

//остановка таймеров для мигания диода
void tim_stop (void)
{
  system_status.tim.status = 0;
  interval_tim_disable();
  duration_tim_disable();
}

//установка интервала между миганием диода
void tim_set_interval (uint16_t val)
{
  system_status.tim.interval = val;
  set_interval(val);
}

//установка времени в течении которого диод будет мигать
void tim_set_duration (uint8_t val)
{
  if (val == 0) {
    duration_tim_disable();
  }
  else {
    set_duration(val);
  }
  system_status.tim.duration = val;
}
