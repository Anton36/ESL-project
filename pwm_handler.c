
#include "pwm_handler.h"
#include "nrfx_systick.h"
#include "led_handler.h"



void systick_pwm(int led_index)
{
    int duty_cycle = current_duty_cycle;
    nrfx_systick_state_t pwm_time;

    for (; duty_cycle <= PWM_PERIOD_US; duty_cycle += PWM_STEP)
    {
        current_duty_cycle = duty_cycle;
        led_off(led_index);
        nrfx_systick_get(&pwm_time);
        led_on(led_index);
        while (!nrfx_systick_test(&pwm_time, duty_cycle))
        {
            if (double_click == 0)
            {
                return;
            }
        }
        nrfx_systick_get(&pwm_time);
        led_off(led_index);
        while (!nrfx_systick_test(&pwm_time, PWM_PERIOD_US - duty_cycle))
        {
            if (double_click == 0)
            {
                return;
            }
        }
    }
    duty_cycle = current_duty_cycle;

    for (; duty_cycle >= 0; duty_cycle -= PWM_STEP)
    {
        current_duty_cycle = duty_cycle;
        led_off(led_index);
        nrfx_systick_get(&pwm_time);
        led_on(led_index);
        while (!nrfx_systick_test(&pwm_time, duty_cycle))
        {
            if (double_click == 0)
            {
                return;
            }
        }
        nrfx_systick_get(&pwm_time);
        led_off(led_index);
        while (!nrfx_systick_test(&pwm_time, PWM_PERIOD_US - duty_cycle))
        {
            if (double_click == 0)
            {
                return;
            }
        }
    }
}

void maintain_pwm(int led_index, int duty_cycle)
{

    nrfx_systick_state_t pwm_time;

    led_off(led_index);
    nrfx_systick_get(&pwm_time);
    led_on(led_index);
    while (!nrfx_systick_test(&pwm_time, duty_cycle))
    {
        if (double_click == 1)
        {
            return;
        }
    }
    nrfx_systick_get(&pwm_time);
    led_off(led_index);
    while (!nrfx_systick_test(&pwm_time, PWM_PERIOD_US - duty_cycle))
    {
        if (double_click == 1)
        {
            return;
        }
    }
}