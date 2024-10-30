/**
 * Copyright (c) 2014 - 2021, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/** @file
 *
 * @defgroup blinky_example_main main.c
 * @{
 * @ingroup blinky_example
 * @brief Blinky Example Application main file.
 *
 * This file contains the source code for a sample application to blink LEDs.
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "boards.h"
#include "nrf_gpio.h"

#define MIN_DELAY 500
#define MAX_DELAY 1000
#define MICRO_DELAY 100

#define LED0_GREEN_PIN NRF_GPIO_PIN_MAP(0, 6)
#define LED1_RED_PIN NRF_GPIO_PIN_MAP(0, 8)
#define LED2_GREEN_PIN NRF_GPIO_PIN_MAP(1, 9)
#define LED3_BLUE_PIN NRF_GPIO_PIN_MAP(0, 12)

#define BUTTON_PIN NRF_GPIO_PIN_MAP(1, 6)

void gpio_unit(void);
void led_blink(int);

static int led_digits[] = {LED0_GREEN_PIN, LED1_RED_PIN, LED2_GREEN_PIN, LED3_BLUE_PIN};
static int id_digits[] = {6, 6, 1, 5};

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    /* Configure board. */
    bsp_board_init(BSP_INIT_LEDS);
    gpio_unit();
    int i_leds = 0;
    int i_blink = 0;
    bool is_led_on = false;

    /* Toggle LEDs. */
    while (true)
    {

        if (nrf_gpio_pin_read(BUTTON_PIN) == 0)
        {
            while (true)
            {

                if (i_leds >= LEDS_NUMBER)
                    i_leds = 0;

                while (i_blink < id_digits[i_leds])
                {
                    if (is_led_on == true)
                        nrf_gpio_pin_write(led_digits[i_leds], 1);

                    nrf_gpio_pin_write(led_digits[i_leds], 0);
                    is_led_on = true;

                    if (nrf_gpio_pin_read(BUTTON_PIN))
                        break;

                    nrf_delay_ms(MIN_DELAY);

                    nrf_gpio_pin_write(led_digits[i_leds], 1);
                    is_led_on = false;

                    if (nrf_gpio_pin_read(BUTTON_PIN))
                        break;

                    nrf_delay_ms(MIN_DELAY);

                    i_blink++;
                }

                if (nrf_gpio_pin_read(BUTTON_PIN))
                    break;

                i_blink = 0;
                i_leds++;
            }
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
        nrf_gpio_pin_write(led_digits[i], 1);
    }
}
/*
     void led_blink(int i)
     {
         nrf_gpio_pin_write(led_digits[i], 0);
         nrf_delay_ms(MIN_DELAY);
         nrf_gpio_pin_write(led_digits[i], 1);
         nrf_delay_ms(MIN_DELAY);
     }

 */