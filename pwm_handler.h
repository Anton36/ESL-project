#ifndef PWM_HANDLER_H
#define PWM_HANDLER_H

#include "nrfx_systick.h"
#define PWM_FREQUENCY_HZ 1000
#define PWM_STEP 1

extern volatile bool double_click;
extern int i_leds;

typedef struct
{
    int frequency_hz;
    int step;
    int current_duty;
    bool direction; /*false = нарастание,True = убывание*/
    bool state;     /*Состояние светодиода*/
    nrfx_systick_state_t pwm_time;
} pwm_systick_t;

void systick_get_time(pwm_systick_t *settings);

void pwm_systick_state(pwm_systick_t *settings, int maintain_flag);

#endif