#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "boards.h"

#include "nrfx_gpiote.h"

#include "app_timer.h"
#include "drv_rtc.h"
#include "nrfx_clock.h"



#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_log_backend_usb.h"

#include "led_handler.h"
#include "button_handler.h"
#include "pwm_handler.h"

#define MIN_DELAY 500
#define MAX_DELAY 1000
#define MICRO_DELAY 100




void clock_event_handler(nrfx_clock_evt_type_t event);

void logs_init();

volatile int current_duty_cycle = 0;

volatile bool double_click = false;

int led_digits[] = {LED0_GREEN_PIN, LED1_RED_PIN, LED2_GREEN_PIN, LED3_BLUE_PIN};
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
    app_timer_init();
    timer_init();
    nrfx_systick_init();
    nrfx_gpiote_init();
    gpio_button_init();
    led_init();
    logs_init();

    NRF_LOG_INFO("Starting up the test project with USB logging");

    /* Toggle LEDs. */
    while (true)
    {

        LOG_BACKEND_USB_PROCESS();
        NRF_LOG_PROCESS();
        if (double_click)
        {
            systick_pwm(i_leds);

            i_blink++;
            NRF_LOG_INFO("i_blink++");

            if (i_blink >= id_digits[i_leds])
            {
                i_blink = 0;
                i_leds = (i_leds + 1) % LEDS_NUMBER;
                NRF_LOG_INFO("i_leds++");
            }
        }
        else if ((double_click == 0) & (current_duty_cycle != 0))
        {

            maintain_pwm(i_leds, current_duty_cycle);
        }
    }
}



void clock_event_handler(nrfx_clock_evt_type_t event)
{
}

void logs_init()
{
    NRF_LOG_INIT(NULL);
    NRF_LOG_DEFAULT_BACKENDS_INIT();
}