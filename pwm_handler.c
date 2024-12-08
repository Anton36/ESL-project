

#include "led_handler.h"
#include "nrf_log.h"
#include "pwm_handler.h"
#include "nrfx_pwm.h"
#include <stdint.h>
#include "button_handler.h"
#include "memory_handler.h"
static nrfx_pwm_t pwm = NRFX_PWM_INSTANCE(0);
APP_TIMER_DEF(pwm_starter_timer);
static nrf_pwm_values_individual_t pwm_values;
static nrf_pwm_sequence_t const pwm_seq =
    {
        .values.p_individual = &pwm_values,
        .length = NRF_PWM_VALUES_LENGTH(pwm_values),
        .repeats = 0,
        .end_delay = 0};

void pwm_init()
{

    nrfx_pwm_config_t pwm_config =
        {
            .output_pins = {LED0_GREEN_PIN | NRFX_PWM_PIN_INVERTED,
                            LED1_RED_PIN | NRFX_PWM_PIN_INVERTED,
                            LED2_GREEN_PIN | NRFX_PWM_PIN_INVERTED,
                            LED3_BLUE_PIN | NRFX_PWM_PIN_INVERTED},
            .irq_priority = NRFX_PWM_DEFAULT_CONFIG_IRQ_PRIORITY,
            .base_clock = (nrf_pwm_clk_t)NRFX_PWM_DEFAULT_CONFIG_BASE_CLOCK,
            .count_mode = (nrf_pwm_mode_t)NRFX_PWM_DEFAULT_CONFIG_COUNT_MODE,
            .top_value = PWM_TOP_VALUE,
            .load_mode = NRF_PWM_LOAD_INDIVIDUAL,
            .step_mode = (nrf_pwm_dec_step_t)NRFX_PWM_DEFAULT_CONFIG_STEP_MODE,

        };

    nrfx_pwm_init(&pwm, &pwm_config, NULL);
    app_timer_init();
    app_timer_create(&pwm_starter_timer, APP_TIMER_MODE_REPEATED, pwm_starter_timer_handler);
}

void pwm_starter_timer_handler(void *p_context)
{
    display_current_color();
    modify_led1();
}

void pwm_timer_start(void)
{
    app_timer_start(pwm_starter_timer, APP_TIMER_TICKS(PWM_TIMER_DELAY), NULL);
}

void start_pwm_playback()
{
    nrfx_pwm_simple_playback(&pwm, &pwm_seq, 1, NRFX_PWM_FLAG_LOOP);
}

void pwm_set_duty_cycle(int channel, int duty_cycle)
{
    duty_cycle %= PWM_TOP_VALUE + 1;

    switch (channel)
    {
    case 0:
        pwm_values.channel_0 = duty_cycle;
        break;
    case 1:
        pwm_values.channel_1 = duty_cycle;
        break;
    case 2:
        pwm_values.channel_2 = duty_cycle;
        break;
    case 3:
        pwm_values.channel_3 = duty_cycle;
        break;
    }
}

void changing_mode()
{
    duty_cycle = 0;
    if (current_mode == MODE_HUE_MODIFY)
    {
        current_mode = MODE_SAT_MODIFY;
        NRF_LOG_INFO("Saturation mode");
    }
    else if (current_mode == MODE_SAT_MODIFY)
    {
        current_mode = MODE_BRIGHT_MODIFY;
        NRF_LOG_INFO("Brightnes mode");
    }
    else if (current_mode == MODE_BRIGHT_MODIFY)
    {
        current_mode = MODE_DISPLAY_COLOR;
        NRF_LOG_INFO("Dispaly color mode");
        write_to_memory(hsv_value.hue, hsv_value.saturation, hsv_value.value);
    }
    else if (current_mode == MODE_DISPLAY_COLOR)
    {
        current_mode = MODE_HUE_MODIFY;
        NRF_LOG_INFO("Hue mode");
    }
}
