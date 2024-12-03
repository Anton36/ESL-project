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
#define HUE_MAX_VALUE 360

extern int led_digits[];
extern int duty_cycle;

void led_on(int);
void led_off(int);
void led_init(void);
void modify_duty_cycle_for_LED1(void);
void modify_duty_cycle_for_HSV(uint32_t *value, bool *direction, uint8_t step);
void modify_hsv(void);
void display_current_color(void);
void hsv_to_rgb(uint32_t h, uint32_t s, uint32_t v, uint32_t *r, uint32_t *g, uint32_t *b);

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
    uint32_t red;
    uint32_t green;
    uint32_t blue;
} rgb_values;

typedef struct
{
    uint32_t hue;
    uint32_t saturation;
    uint32_t value;
} hsv_values;
extern hsv_values hsv_value;
typedef struct
{
    bool is_hue_increasing;
    bool is_saturation_increasing;
    bool is_value_increasing;
} direction_hsv;

#endif