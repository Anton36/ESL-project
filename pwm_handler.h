#ifndef PWM_HANDLER_H 
#define PWM_HANDLER_H

#include <stdint.h>
#include "nrfx_systick.h"

#define PWM_PERIOD_US 1000
#define PWM_STEP 1


extern volatile bool double_click;
extern volatile int current_duty_cycle;


void systick_pwm(int);
void maintain_pwm(int led_index, int duty_cycle);

#endif