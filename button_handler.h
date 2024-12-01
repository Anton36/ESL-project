#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include <stdbool.h>
#include "app_timer.h"
#define BUTTON_PRESSED_STATE 0
#define BUTTON_UNPRESSED_STATE 1

#define DEBOUNCING_DELAY 50
#define DOUBLE_CLICK_DELAY 500
#define LONG_PRESS_INITIAL_DELAY 800
#define LONG_PRESS_REPEAT_DELAY 50
#define PWM_TIMER_DELAY 30

#define BUTTON_PIN NRF_GPIO_PIN_MAP(1, 6)
extern volatile bool double_click;
extern bool maintain_flag;

void gpio_button_init();
void timer_init(void);

#endif