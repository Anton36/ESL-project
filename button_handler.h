#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include <stdbool.h>
#include "app_timer.h"
#define BUTTON_PRESSED_STATE 0
#define BUTTON_UNPRESSED_STATE 1

#define DEBOUNCING_DELAY 50
#define DOUBLE_CLICK_DELAY 500

#define BUTTON_PIN NRF_GPIO_PIN_MAP(1, 6)
extern volatile bool double_click;

void gpio_button_init();
void timer_init(void);

#endif