#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "boards.h"

#include "nrfx_gpiote.h"

#include "app_timer.h"
#include "drv_rtc.h"
#include "nrfx_clock.h"

#include "nrfx_systick.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_log_backend_usb.h"

#include "led_handler.h"
#include "button_handler.h"

#define MIN_DELAY 500
#define MAX_DELAY 1000
#define MICRO_DELAY 100

#define PWM_PERIOD_US 1000
#define PWM_STEP 1

void systick_pwm(int);
void clock_event_handler(nrfx_clock_evt_type_t event);
void maintain_pwm(int led_index, int duty_cycle);
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

void clock_event_handler(nrfx_clock_evt_type_t event)
{
}

void logs_init()
{
    NRF_LOG_INIT(NULL);
    NRF_LOG_DEFAULT_BACKENDS_INIT();
}