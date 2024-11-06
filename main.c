#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "boards.h"

#include "nrfx_gpiote.h"

#include "app_timer.h"
#include "drv_rtc.h"
#include "nrfx_clock.h"

#include "nrfx_systick.h"



#define MIN_DELAY 500
#define MAX_DELAY 1000
#define MICRO_DELAY 100

#define PWM_PERIOD_US 1000
#define PWM_STEP 1

#define DEBOUNCING_DELAY 50
#define DOUBLE_CLICK_DELAY 500

#define LED_ON_STATE 0
#define LED_OFF_STATE 1
#define BUTTON_PRESSED_STATE 0
#define BUTTON_UNPRESSED_STATE 1

#define LED0_GREEN_PIN NRF_GPIO_PIN_MAP(0, 6)
#define LED1_RED_PIN NRF_GPIO_PIN_MAP(0, 8)
#define LED2_GREEN_PIN NRF_GPIO_PIN_MAP(1, 9)
#define LED3_BLUE_PIN NRF_GPIO_PIN_MAP(0, 12)

#define BUTTON_PIN NRF_GPIO_PIN_MAP(1, 6)

void gpio_init(void);
void led_on(int);
void led_off(int);
void timer_init(void);
void lfclk_request(void);
void systick_pwm(int);
void clock_event_handler(nrfx_clock_evt_type_t event);

volatile bool double_click = false;
volatile  int number_of_click = 0;
volatile bool debounce_timer_active = false;
volatile bool doubleclick_timer_active = false;

APP_TIMER_DEF(debounce_timer);
APP_TIMER_DEF(double_click_timer);

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
   lfclk_request();
    app_timer_init();
   timer_init();
    nrfx_systick_init();
    nrfx_gpiote_init();
    gpio_init();
    
   

    /* Toggle LEDs. */
    while (true)
    {
        while(double_click)
        {
            systick_pwm(i_leds);
            nrf_delay_ms(MICRO_DELAY);
            i_blink++;
            if (i_blink >= id_digits[i_leds])
            {
                i_blink = 0;
                i_leds = (i_leds + 1) % LEDS_NUMBER;
            }
        }
        
    }
}




void button_event_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    if(debounce_timer_active == 0)
    {
        app_timer_start(debounce_timer,APP_TIMER_TICKS(DEBOUNCING_DELAY),NULL);
        debounce_timer_active = true;
    
    }
        
    

    
        
        

}

void debounce_IRQHandler(void * p_context)
{
    debounce_timer_active = false;
    

    if (nrf_gpio_pin_read(BUTTON_PIN) == BUTTON_PRESSED_STATE)
    {
        number_of_click++;
        if(number_of_click == 1)
        {
            app_timer_start(double_click_timer,APP_TIMER_TICKS(DEBOUNCING_DELAY),NULL);
        } else if (number_of_click == 2)
        {
            double_click = true;
            number_of_click = 0;
        }

        
    }

}

void double_click_IRQHandler(void * p_context)
{
    number_of_click = 0;

}



void gpio_init()
{
    
    nrfx_gpiote_in_config_t btn_config = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
    btn_config.pull = NRF_GPIO_PIN_PULLUP;
    nrfx_gpiote_in_init(BUTTON_PIN, &btn_config, button_event_handler);
    nrfx_gpiote_in_event_enable(BUTTON_PIN, true);
   
   
   
    
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



void systick_pwm(int led_index)
{
    int duty_cycle = 0;
    nrfx_systick_state_t pwm_time;
    
    

        for (duty_cycle = 0; duty_cycle <= PWM_PERIOD_US; duty_cycle += PWM_STEP)
        {
            nrfx_systick_get(&pwm_time);
            led_on(led_index);
            while (!nrfx_systick_test(&pwm_time, duty_cycle))
            {
            }
            led_off(led_index);
            while (!nrfx_systick_test(&pwm_time, PWM_PERIOD_US - duty_cycle))
            {
            }
        }

        for (duty_cycle = PWM_PERIOD_US; duty_cycle > 0; duty_cycle -= PWM_STEP)
        {
            nrfx_systick_get(&pwm_time);
            led_on(led_index);
            while (!nrfx_systick_test(&pwm_time, duty_cycle))
            {
            }
            led_off(led_index);
            while (!nrfx_systick_test(&pwm_time, PWM_PERIOD_US - duty_cycle))
            {
            }
        }
    
}
void timer_init(void)
{
    
    
    

    app_timer_create(&debounce_timer,
                    APP_TIMER_MODE_SINGLE_SHOT,
                    debounce_IRQHandler);
    app_timer_create(&double_click_timer,
                    APP_TIMER_MODE_SINGLE_SHOT,
                    double_click_IRQHandler);
}

void clock_event_handler(nrfx_clock_evt_type_t event)
{
  
}

void lfclk_request(void)
{
    nrfx_clock_init(clock_event_handler);
    nrfx_clock_enable();
    nrfx_clock_lfclk_start();
}