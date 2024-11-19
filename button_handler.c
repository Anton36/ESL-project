#include "button_handler.h"
#include "nrfx_gpiote.h"
#include "nrf_log.h"


volatile int number_of_click = 0;
volatile bool debounce_timer_active = false;
volatile bool doubleclick_timer_active = false;

APP_TIMER_DEF(debounce_timer);
APP_TIMER_DEF(double_click_timer);


void button_event_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    

    if (debounce_timer_active == 0)
    {

        app_timer_start(debounce_timer, APP_TIMER_TICKS(DEBOUNCING_DELAY), NULL);
    }
}

void debounce_Handler(void *p_context)
{

    if (nrf_gpio_pin_read(BUTTON_PIN) == BUTTON_PRESSED_STATE)
    {
        number_of_click++;
        if (number_of_click == 1)
        {

            app_timer_start(double_click_timer, APP_TIMER_TICKS(DOUBLE_CLICK_DELAY), NULL);
        }
    }
}

void double_click_Handler(void *p_context)
{
    if (number_of_click == 2)
    {
        if (double_click == true)
        {
            NRF_LOG_INFO("stoped");
            double_click = false;
        }
        else
        {
            NRF_LOG_INFO("double click");
            
            double_click = true;
        }
    }
    number_of_click = 0;
}


void gpio_button_init()
{

    nrfx_gpiote_in_config_t btn_config = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
    btn_config.pull = NRF_GPIO_PIN_PULLUP;
    nrfx_gpiote_in_init(BUTTON_PIN, &btn_config, button_event_handler);
    nrfx_gpiote_in_event_enable(BUTTON_PIN, true);

    
}

void timer_init(void)
{

    app_timer_create(&double_click_timer,
                     APP_TIMER_MODE_SINGLE_SHOT,
                     double_click_Handler);

    app_timer_create(&debounce_timer,
                     APP_TIMER_MODE_SINGLE_SHOT,
                     debounce_Handler);
}

