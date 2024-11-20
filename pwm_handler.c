
#include "pwm_handler.h"
#include "nrfx_systick.h"
#include "led_handler.h"
#include "nrf_log.h"

void systick_get_time(pwm_systick_t *pwm_systick)
{
    nrfx_systick_get(&pwm_systick->pwm_time);
}

void pwm_systick_state(pwm_systick_t *pwm_systick, int maintain_flag)
{
    int timeout;

    if (pwm_systick->state)
    {
        timeout = pwm_systick->frequency_hz - pwm_systick->current_duty;
    }
    else
    {
        timeout = pwm_systick->current_duty;
    }

    if (!nrfx_systick_test(&pwm_systick->pwm_time, timeout))
    {
        return;
    }

    pwm_systick->state = !pwm_systick->state;
    if (maintain_flag == 0)
    {

        if ((pwm_systick->current_duty == 1000) && (!pwm_systick->direction))
        {
            pwm_systick->direction = true;
            NRF_LOG_INFO("Достигли предела скважности ,начинаем убывать");
        }

        if ((pwm_systick->current_duty == 0) && (pwm_systick->direction))
        {
            pwm_systick->direction = false;
            NRF_LOG_INFO("Достигли начала скважности ,начинаем возрастать");
        }

        if (!pwm_systick->state && pwm_systick->direction)
        {
            pwm_systick->current_duty -= pwm_systick->step;
        }

        if (pwm_systick->state && !pwm_systick->direction)
        {
            pwm_systick->current_duty += pwm_systick->step;
        }
    }

    systick_get_time(pwm_systick);
    if (pwm_systick->state == 0)
    {
        led_on(i_leds);
    }
    else
    {
        led_off(i_leds);
    }
}
