#include <action.h>

static sys_status_t system_status;

sys_status_t get_status (void)
{
  return system_status;
}

void init_system (void)
{
  //init periph
  init_leds();
  init_btns();
  init_tim();
  //init_usart();

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


uint8_t set_bright (led_t led, uint8_t val)
{
  if (val > 100) return ERROR_CONST;
  uint8_t tmp_val = 100 - val;
  if (set_pulse(led, perc_to_pulse(tmp_val)) == OK_CONST) {
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
      }
  }
  else return ERROR_CONST;

  return OK_CONST;
}

void tim_start (void)
{
  system_status.tim.status = 1;
  interval_tim_enable();
  if (system_status.tim.duration > 0)
      duration_tim_enable();
}

void tim_stop (void)
{
  system_status.tim.status = 0;
  interval_tim_disable();
  duration_tim_disable();
}

void tim_set_interval (uint16_t val)
{
  system_status.tim.interval = val;
  set_interval(val);
}

void tim_set_duration (uint8_t val)
{
  if (val == 0) {
    duration_tim_disable();
  }
  else {
    set_duration(val);
    duration_tim_enable();
  }
  system_status.tim.duration = val;
}
