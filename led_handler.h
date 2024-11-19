#ifndef LED_HANDLER_H
#define LED_HANDLER_H

#define LED_ON_STATE 0
#define LED_OFF_STATE 1
#define LEDS_NUMBER 4
#define LED0_GREEN_PIN NRF_GPIO_PIN_MAP(0, 6)
#define LED1_RED_PIN NRF_GPIO_PIN_MAP(0, 8)
#define LED2_GREEN_PIN NRF_GPIO_PIN_MAP(1, 9)
#define LED3_BLUE_PIN NRF_GPIO_PIN_MAP(0, 12)

extern int led_digits[];



void led_on(int);
void led_off(int);
void led_init(void);

#endif