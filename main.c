#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "boards.h"
#include "nordic_common.h"
#include "nrfx_gpiote.h"

#include "app_timer.h"

#include "nrfx_clock.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_log_backend_usb.h"

#include "led_handler.h"
#include "button_handler.h"
#include "pwm_handler.h"
#include "nrfx_pwm.h"
#include "nrfx_common.h"
#include "memory_handler.h"
#include "app_usbd.h"
#include "app_usbd_serial_num.h"
#include "app_usbd_cdc_acm.h"
#include "usb_handler.h"

void logs_init();

volatile bool double_click = false;
extern saved_color_t color_list[MAX_NUMBER_OF_SAVED_COLORS];

int led_digits[] = {LED0_GREEN_PIN, LED1_RED_PIN, LED2_GREEN_PIN, LED3_BLUE_PIN};

int i_leds = 0;
int i_blink = 0;
bool maintain_flag = 1;

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    /* Configure board. */

    app_timer_init();
    timer_init();
    nrfx_gpiote_init();
    gpio_button_init();
    led_init();
    logs_init();
    pwm_init();
    start_pwm_playback();
    pwm_timer_start();
    usb_init();

    read_HSV_from_memory(&hsv_value.hue, &hsv_value.saturation, &hsv_value.value);
    read_colors_from_memory(color_list, MAX_NUMBER_OF_SAVED_COLORS);

    NRF_LOG_INFO("Starting up  project with USB logging");

    /* Toggle LEDs. */
    while (true)
    {
        LOG_BACKEND_USB_PROCESS();
        NRF_LOG_PROCESS();

        //__WFI();
        while (app_usbd_event_queue_process())
        {
            /* Nothing to do */
        }
    }
}

void logs_init()
{
    NRF_LOG_INIT(NULL);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}
