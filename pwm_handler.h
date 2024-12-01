#ifndef PWM_HANDLER_H
#define PWM_HANDLER_H


#include "nrfx_pwm.h"
#include "led_handler.h"
#include "nrf_gpio.h"
#include <stdint.h>
#define PWM_TOP_VALUE 255
#define PWM_MIN_VALUE 0

extern volatile bool double_click;
extern int i_leds;
 

void pwm_init(void);
void pwm_set_duty_cycle(int channel, int duty_cycle);
void start_pwm_playback(void);
void changing_mode(void);
void pwm_timer_start(void);
 void pwm_starter_timer_handler(void *p_context);

#endif