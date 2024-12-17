#ifndef USB_HANDLER_H
#define USB_HANDLER_H
#include <stdint.h>
#include "led_handler.h"

#define READ_SIZE 1
#define COMMAND_BUFFER_SIZE 32
#define COMMAND_BUFFER_SIZE 32
#define MAX_NUMBER_OF_OPERATORS 5
#define MAX_PARAMETR_SIZE 20

typedef struct {
    char rx_buffer[READ_SIZE];
    char command_buffer[COMMAND_BUFFER_SIZE];
    char command[COMMAND_BUFFER_SIZE];
    volatile uint8_t buffer_index;
} usb_service_handler_t;
extern usb_service_handler_t usb_service_handler;

void usb_init(void);
void command_buffer_char_append(char ch);
void command_handler();
void usb_send_response(const char* response);
int find_spot_to_save_light(saved_color_t *color_list, int size);
void print_color_list(saved_color_t *color_list, int size);

int find_color_in_list(saved_color_t *color_list, int size, const char *name);

#endif