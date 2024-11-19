#include "led_handler.h"
#include "button_handler.h"
#include "nrf_log.h"
#include "nrf_gpio.h"
#include <string.h>

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
