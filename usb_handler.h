#ifndef USB_HANDLER_H
#define USB_HANDLER_H
#include <stdint.h>

#define READ_SIZE 1
#define COMMAND_BUFFER_SIZE 32
#define COMMAND_BUFFER_SIZE 32
#define MAX_NUMBER_OF_OPERATORS 5
#define MAX_PARAMETR_SIZE 10

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

#endif