#ifndef LED_HANDLER_H
#define LED_HANDLER_H
#include "pwm_handler.h"

#define LED_ON_STATE 0
#define LED_OFF_STATE 1
#define LEDS_NUMBER 4
#define LED0_GREEN_PIN NRF_GPIO_PIN_MAP(0, 6)
#define LED1_RED_PIN NRF_GPIO_PIN_MAP(0, 8)
#define LED2_GREEN_PIN NRF_GPIO_PIN_MAP(1, 9)
#define LED3_BLUE_PIN NRF_GPIO_PIN_MAP(0, 12)

extern int led_digits[];
extern int duty_cycle;

void led_on(int);
void led_off(int);
void led_init(void);
void modify_duty_cycle_for_LED1(void);
void modify_duty_cycle_for_HSV(uint16_t *value, bool *direction, uint8_t step);
void modify_hsv(void);
void display_current_color(void);
void hsv_to_rgb(uint16_t h, uint16_t s, uint16_t v,  uint8_t *r,  uint8_t *g,  uint8_t *b);

typedef enum
{
    MODE_HUE_MODIFY,
    MODE_SAT_MODIFY,
    MODE_BRIGHT_MODIFY,
    MODE_DISPLAY_COLOR
} controller_mode_t;

controller_mode_t current_mode;

typedef struct
{
    uint8_t for_hue_mode;
    uint8_t for_sat_mode;
    uint8_t for_display_mode;

} steps_for_mods;

typedef struct
{
    uint8_t for_hue_mode;
    uint8_t for_sat_mode;
    uint8_t for_brightness_mode;

} steps_for_hsv_changing;

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} rgb_values;

typedef struct
{
    uint16_t hue;
    uint16_t saturation;
    uint16_t value;
} hsv_values;

typedef struct
{
    bool is_hue_increasing;
    bool is_saturation_increasing;
    bool is_value_increasing;
} direction_hsv;

#endif