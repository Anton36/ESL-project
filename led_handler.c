#include "led_handler.h"
#include "button_handler.h"
#include "nrf_log.h"
#include "nrf_gpio.h"
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "memory_handler.h"

#include "nrf_log_backend_usb.h"
uint32_t duty_cycle = 0;
bool direction_led1 = true;

controller_mode_t current_mode = MODE_DISPLAY_COLOR;

direction_hsv_t direction_for_hsv = {
    .is_hue_increasing = true,
    .is_saturation_increasing = true,
    .is_value_increasing = true,
};

static steps_for_mods_t steps_for_mode = {
    .for_hue_mode = 6,
    .for_sat_mode = 10,
    .for_display_mode = 0,

};

static steps_for_hsv_changing_t steps_for_hsv_change = {
    .for_hue_mode = 2,
    .for_sat_mode = 1,
    .for_brightness_mode = 4,

};
rgb_values_t rgb_value = {
    .red = 0,
    .green = 0,
    .blue = 0,
};

hsv_values_t hsv_value = {
    .hue = HUE_MAX_VALUE * 0.15,
    .saturation = PWM_TOP_VALUE,
    .value = PWM_TOP_VALUE,
};

void led_on(int led_index)
{
    nrf_gpio_pin_write(led_digits[led_index], LED_ON_STATE);
}

void led_off(int led_index)
{
    nrf_gpio_pin_write(led_digits[led_index], LED_OFF_STATE);
}

void led_init()
{
    nrf_gpio_cfg_output(LED0_GREEN_PIN); // LED0
    nrf_gpio_cfg_output(LED1_RED_PIN);   // LED1
    nrf_gpio_cfg_output(LED2_GREEN_PIN); // LED2
    nrf_gpio_cfg_output(LED3_BLUE_PIN);  // LED3
    for (int i = 0; i < LEDS_NUMBER; i++)
    {
        nrf_gpio_pin_write(led_digits[i], LED_OFF_STATE);
    }
}
void modify_led1()
{

    switch (current_mode)
    {
    case MODE_HUE_MODIFY:
        modify_duty_cycle(&duty_cycle, &direction_led1, steps_for_mode.for_hue_mode, PWM_TOP_VALUE, PWM_MIN_VALUE);
        pwm_set_duty_cycle(0, duty_cycle);
        break;

    case MODE_SAT_MODIFY:
        modify_duty_cycle(&duty_cycle, &direction_led1, steps_for_mode.for_sat_mode, PWM_TOP_VALUE, PWM_MIN_VALUE);
        pwm_set_duty_cycle(0, duty_cycle);
        break;

    case MODE_DISPLAY_COLOR:
        // modify_duty_cycle(&duty_cycle, &direction_led1, steps_for_mode.for_display_mode,  PWM_TOP_VALUE, PWM_MIN_VALUE);
        pwm_set_duty_cycle(0, PWM_MIN_VALUE);
        break;

    case MODE_BRIGHT_MODIFY:
        pwm_set_duty_cycle(0, 100);
        break;
    }
}

void modify_hsv()
{
    switch (current_mode)
    {
    case MODE_HUE_MODIFY:
        hsv_value.hue = (hsv_value.hue + steps_for_hsv_change.for_hue_mode) % HUE_MAX_VALUE;

        break;

    case MODE_SAT_MODIFY:
        modify_duty_cycle(&hsv_value.saturation, &direction_for_hsv.is_saturation_increasing, steps_for_hsv_change.for_sat_mode, PWM_TOP_VALUE, PWM_MIN_VALUE);

        break;

    case MODE_BRIGHT_MODIFY:
        modify_duty_cycle(&hsv_value.value, &direction_for_hsv.is_value_increasing, steps_for_hsv_change.for_brightness_mode, PWM_TOP_VALUE, PWM_MIN_VALUE);

        break;

    default:
        break;
    }
}

void modify_duty_cycle(uint32_t *value, bool *direction, uint32_t step, uint32_t max_value, uint32_t min_value)
{
    if (*direction)
    {

        if (*value + step > max_value)
        {
            *value = max_value;
            *direction = false;
        }
        else
        {
            *value += step;
            NRF_LOG_INFO("%d = value", *value);
        }
    }
    else
    {

        if (*value < min_value + step)
        {
            *value = min_value;
            *direction = true;
        }
        else
        {
            *value -= step;
            NRF_LOG_INFO("%d = value", *value);
        }
    }
    
}
void display_current_color(void)
{

    hsv_to_rgb(hsv_value.hue, hsv_value.saturation, hsv_value.value, &rgb_value.red, &rgb_value.green, &rgb_value.blue);

    // NRF_LOG_INFO("Current color R:%d G:%d B:%d", rgb_value.red, rgb_value.green, rgb_value.blue);
    //  NRF_LOG_INFO("Current color H:%d S:%d L:%d", hsv_value.hue, hsv_value.saturation, hsv_value.value);

    pwm_set_duty_cycle(1, rgb_value.red);
    pwm_set_duty_cycle(2, rgb_value.green);
    pwm_set_duty_cycle(3, rgb_value.blue);
}

void hsv_to_rgb(uint32_t h, uint32_t s, uint32_t v, uint32_t *r, uint32_t *g, uint32_t *b)
{

    h %= 360;

    uint8_t region = h / 60;
    uint8_t remainder = h % 60;

    uint32_t p = (v * (255 - s)) / 255;
    uint32_t q = (v * (255 - ((s * remainder) / 60))) / 255;
    uint32_t t = (v * (255 - ((s * (60 - remainder)) / 60))) / 255;

    switch (region)
    {
    case 0:
        *r = v;
        *g = t;
        *b = p;
        break;
    case 1:
        *r = q;
        *g = v;
        *b = p;
        break;
    case 2:
        *r = p;
        *g = v;
        *b = t;
        break;
    case 3:
        *r = p;
        *g = q;
        *b = v;
        break;
    case 4:
        *r = t;
        *g = p;
        *b = v;
        break;
    case 5:
    default:
        *r = v;
        *g = p;
        *b = q;
        break;
    }
}