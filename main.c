
#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "boards.h"
#include "nrf_gpio.h"

#define MIN_DELAY 500
#define MAX_DELAY 1000
#define MICRO_DELAY 100

#define LED_ON_STATE 0
#define LED_OFF_STATE 1
#define BUTTON_PRESSED_STATE 0
#define BUTTON_UNPRESSED_STATE 1

#define LED0_GREEN_PIN NRF_GPIO_PIN_MAP(0, 6)
#define LED1_RED_PIN NRF_GPIO_PIN_MAP(0, 8)
#define LED2_GREEN_PIN NRF_GPIO_PIN_MAP(1, 9)
#define LED3_BLUE_PIN NRF_GPIO_PIN_MAP(0, 12)

#define BUTTON_PIN NRF_GPIO_PIN_MAP(1, 6)

void gpio_unit(void);
void led_on(int);
void led_off(int);
void blinking(int);

static int led_digits[] = {LED0_GREEN_PIN, LED1_RED_PIN, LED2_GREEN_PIN, LED3_BLUE_PIN};
static int id_digits[] = {6, 6, 1, 5};
int i_leds = 0;
int i_blink = 0;

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    /* Configure board. */
    bsp_board_init(BSP_INIT_LEDS);
    gpio_unit();

    /* Toggle LEDs. */
    while (true)
    {
        if (nrf_gpio_pin_read(BUTTON_PIN) == BUTTON_PRESSED_STATE)
        {
            
            blinking(i_leds);
            i_blink++;
            if (i_blink >= id_digits[i_leds])
            {
                i_blink = 0;
                i_leds = (i_leds + 1) % LEDS_NUMBER;
            }
        }
        else
        {
            nrf_delay_ms(MICRO_DELAY);
        }
    }
}
void gpio_unit()
{
    nrf_gpio_cfg_input(BUTTON_PIN, NRF_GPIO_PIN_PULLUP); // конфигурация кнопки в качестве входа с подтягивающим вверх резистором

    nrf_gpio_cfg_output(LED0_GREEN_PIN); // LED0
    nrf_gpio_cfg_output(LED1_RED_PIN);   // LED1
    nrf_gpio_cfg_output(LED2_GREEN_PIN); // LED2
    nrf_gpio_cfg_output(LED3_BLUE_PIN);  // LED3
    for (int i = 0; i < LEDS_NUMBER; i++)
    {
        nrf_gpio_pin_write(led_digits[i], LED_OFF_STATE);
    }
}

void led_on(int led_index)
{
    nrf_gpio_pin_write(led_digits[led_index], LED_ON_STATE);
}

void led_off(int led_index)
{
    nrf_gpio_pin_write(led_digits[led_index], LED_OFF_STATE);
}

void blinking(int led_index)
{
    led_on(led_index);

    for (size_t i = 0; i < MIN_DELAY / 100; i++)
    {
        if (nrf_gpio_pin_read(BUTTON_PIN) == BUTTON_UNPRESSED_STATE)
        {
            return;
        }
        nrf_delay_ms(MICRO_DELAY);
    }
    led_off(led_index);
    nrf_delay_ms(MIN_DELAY);
    
}