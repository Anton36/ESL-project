#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "boards.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_log_backend_usb.h"

#include "app_usbd.h"
#include "app_usbd_serial_num.h"
#include "app_usbd_cdc_acm.h"
#include "usb_handler.h"
#include "led_handler.h"
extern saved_color_t color_list[MAX_NUMBER_OF_SAVED_COLORS];
usb_service_handler_t usb_service_handler = {
    .buffer_index = 0,
};

void command_buffer_char_append(char ch)
{
    if (usb_service_handler.buffer_index <= COMMAND_BUFFER_SIZE - 1)
    {
        usb_service_handler.command_buffer[usb_service_handler.buffer_index] = ch;
        usb_service_handler.buffer_index++;
        NRF_LOG_INFO("command buffer : %d", usb_service_handler.buffer_index);
    }
}

static void usb_ev_handler(app_usbd_class_inst_t const *p_inst,
                           app_usbd_cdc_acm_user_event_t event);

/* Make sure that they don't intersect with LOG_BACKEND_USB_CDC_ACM */
#define CDC_ACM_COMM_INTERFACE 2
#define CDC_ACM_COMM_EPIN NRF_DRV_USBD_EPIN3

#define CDC_ACM_DATA_INTERFACE 3
#define CDC_ACM_DATA_EPIN NRF_DRV_USBD_EPIN4
#define CDC_ACM_DATA_EPOUT NRF_DRV_USBD_EPOUT4

APP_USBD_CDC_ACM_GLOBAL_DEF(usb_cdc_acm,
                            usb_ev_handler,
                            CDC_ACM_COMM_INTERFACE,
                            CDC_ACM_DATA_INTERFACE,
                            CDC_ACM_COMM_EPIN,
                            CDC_ACM_DATA_EPIN,
                            CDC_ACM_DATA_EPOUT,
                            APP_USBD_CDC_COMM_PROTOCOL_NONE);

static void usb_ev_handler(app_usbd_class_inst_t const *p_inst,
                           app_usbd_cdc_acm_user_event_t event)
{
    switch (event)
    {
    case APP_USBD_CDC_ACM_USER_EVT_PORT_OPEN:
    {
        NRF_LOG_INFO("Port open");
        ret_code_t ret;

        ret = app_usbd_cdc_acm_read(&usb_cdc_acm, usb_service_handler.rx_buffer, READ_SIZE);
        UNUSED_VARIABLE(ret);
        break;
    }
    case APP_USBD_CDC_ACM_USER_EVT_PORT_CLOSE:
    {

        break;
    }
    case APP_USBD_CDC_ACM_USER_EVT_TX_DONE:
    {
        NRF_LOG_INFO("tx done");

        break;
    }
    case APP_USBD_CDC_ACM_USER_EVT_RX_DONE:
    {
        NRF_LOG_INFO("rx enter");

        ret_code_t ret;
        do
        {
            /*Get amount of data transfered*/
            size_t size = app_usbd_cdc_acm_rx_size(&usb_cdc_acm);
            NRF_LOG_INFO("rx size: %d", size);

            /* It's the simple version of an echo. Note that writing doesn't
             * block execution, and if we have a lot of characters to read and
             * write, some characters can be missed.
             */
            if (usb_service_handler.rx_buffer[0] == '\r' || usb_service_handler.rx_buffer[0] == '\n')
            {
                command_handler();
                ret = app_usbd_cdc_acm_write(&usb_cdc_acm, "\r\n", 2);
            }
            else
            {
                command_buffer_char_append(usb_service_handler.rx_buffer[0]);
                ret = app_usbd_cdc_acm_write(&usb_cdc_acm,
                                             usb_service_handler.rx_buffer,
                                             READ_SIZE);
            }

            /* Fetch data until internal buffer is empty */
            ret = app_usbd_cdc_acm_read(&usb_cdc_acm,
                                        usb_service_handler.rx_buffer,
                                        READ_SIZE);
        } while (ret == NRF_SUCCESS);

        break;
    }
    default:
        break;
    }
}

void usb_init(void)
{
    app_usbd_class_inst_t const *class_cdc_acm = app_usbd_cdc_acm_class_inst_get(&usb_cdc_acm);
    ret_code_t ret = app_usbd_class_append(class_cdc_acm);
    APP_ERROR_CHECK(ret);
}

void string_split(char *str, char *tokens, uint32_t max_number_of_operators, uint32_t max_parametr_size)
{

    uint32_t number_of_operators = 0;

    char *next = strtok(str, " ");
    while (next != NULL)
    {
        if (number_of_operators < max_number_of_operators)
        {
            uint32_t token_len = strlen(next);
            uint32_t copy_len = (token_len < (max_parametr_size - 1)) ? token_len : (max_parametr_size - 1);
            strncpy(tokens + (number_of_operators * max_parametr_size), next, copy_len);
            // Добавляем завершающий ноль
            tokens[number_of_operators * max_parametr_size + copy_len] = '\0';
        }
        number_of_operators++;
        next = strtok(NULL, " ");
    }
}

void command_handler(void)
{
    memcpy(usb_service_handler.command, usb_service_handler.command_buffer, sizeof(usb_service_handler.command_buffer));
    memset(usb_service_handler.command_buffer, 0, sizeof(usb_service_handler.command_buffer));
    usb_service_handler.buffer_index = 0;
    char response[64];
    if (usb_service_handler.command[0] == '\0')
    {
        NRF_LOG_INFO("emperty string");
        snprintf(response, sizeof(response), "\r\nemperty string please try again\r\n");
        usb_send_response(response);
        return;
    }

    char tokens[MAX_NUMBER_OF_OPERATORS][MAX_PARAMETR_SIZE];
    string_split(usb_service_handler.command, (char *)tokens, MAX_NUMBER_OF_OPERATORS, MAX_PARAMETR_SIZE);

    char *pend = NULL;

    if (strcmp(tokens[0], "RGB") == 0)
    {
        uint32_t r = strtol(tokens[1], &pend, 10);
        uint32_t g = strtol(tokens[2], &pend, 10);
        uint32_t b = strtol(tokens[3], &pend, 10);
        if (r <= RGB_MAX_VALUE && g <= RGB_MAX_VALUE && b <= RGB_MAX_VALUE)
        {
            rgb_to_hsv(r, g, b, &hsv_value.hue, &hsv_value.saturation, &hsv_value.value);
            snprintf(response, sizeof(response), "\r\nRGB set to %lu %lu %lu\r\n", r, g, b);
            usb_send_response(response);
        }
        else
        {
            snprintf(response, sizeof(response), "\r\n Invalid values \r\n");
            usb_send_response(response);
        }
    }
    else if (strcmp(tokens[0], "HSV") == 0)
    {

        uint32_t h = strtol(tokens[1], &pend, 10);
        uint32_t s = strtol(tokens[2], &pend, 10);
        uint32_t v = strtol(tokens[3], &pend, 10);
        if (h <= HUE_MAX_VALUE && s <= SAT_VALUE_MAX_VALUE && v <= SAT_VALUE_MAX_VALUE)
        {
            hsv_value.hue = h;
            hsv_value.saturation = s;
            hsv_value.value = v;
            snprintf(response, sizeof(response), "\r\nHSV set to %lu %lu %lu\r\n", h, s, v);
            usb_send_response(response);
        }
        else
        {
            snprintf(response, sizeof(response), "\r\n Invalid values \r\n");
            usb_send_response(response);
        }
    }
    else if (strcmp(tokens[0], "help") == 0)
    {
        usb_send_response("\r\nAvailable commands:\r\n RGB <r(0-255)> <g>(0-255) <b>(0-255) - Set color using RGB \r\n HSV <h>(0-360) <s>(0-100) <v>(0-100) - Set color using HSV \r\n add_rgb_color <r(0-255)> <g>(0-255) <b>(0-255) color_name - add color using RGB\r\n add_hsv_color <h>(0-360) <s>(0-100) <v>(0-100) color_name - add color using HSV\r\n add_current_color color_name - Add  color of RGB led with name to local storage\r\n del_color color_name - Delete the color by name \r\n apply_color color_name - Apply the color from local storage by name to RGB led\r\n list_colors - Show a list of saved colors \r\n");
    }
    else if (strcmp(tokens[0], "add_rgb_color") == 0)
    {
        uint32_t r = strtol(tokens[1], &pend, 10);
        uint32_t g = strtol(tokens[2], &pend, 10);
        uint32_t b = strtol(tokens[3], &pend, 10);
        if (r <= RGB_MAX_VALUE && g <= RGB_MAX_VALUE && b <= RGB_MAX_VALUE)
        {
            int index = 0;
            uint32_t hue = 0;
            uint32_t saturation = 0;
            uint32_t value = 0;
            rgb_to_hsv(r, g, b, &hue, &saturation, &value);

            if ((index = find_spot_to_save_light(color_list, MAX_NUMBER_OF_SAVED_COLORS)) != -1)

            {
                color_list[index].hue = hue;
                color_list[index].saturation = saturation;
                color_list[index].value = value;
                strncpy((char *)color_list[index].name, tokens[4], MAX_NAME_LENGHT);
                snprintf(response, sizeof(response), "\r\n color %s saved\r\n", tokens[4]);
                usb_send_response(response);
            }
            else
            {
                snprintf(response, sizeof(response), "\r\nlist is full");
                usb_send_response(response);
            }
        }
        else
        {
            snprintf(response, sizeof(response), "\r\n Invalid values \r\n");
            usb_send_response(response);
        }
    }
    else if (strcmp(tokens[0], "add_hsv_color") == 0)
    {
        uint32_t h = strtol(tokens[1], &pend, 10);
        uint32_t s = strtol(tokens[2], &pend, 10);
        uint32_t v = strtol(tokens[3], &pend, 10);
        if (h <= HUE_MAX_VALUE && s <= SAT_VALUE_MAX_VALUE && v <= SAT_VALUE_MAX_VALUE)
        {
            int index = 0;
            if ((index = find_spot_to_save_light(color_list, MAX_NUMBER_OF_SAVED_COLORS)) != -1)

            {
                color_list[index].hue = h;
                color_list[index].saturation = s;
                color_list[index].value = v;
                strncpy((char *)color_list[index].name, tokens[4], MAX_NAME_LENGHT);
                snprintf(response, sizeof(response), "color %s saved\r\n", tokens[4]);
                usb_send_response(response);
            }
            else
            {
                snprintf(response, sizeof(response), "list is full");
                usb_send_response(response);
            }
        }
        else
        {
            snprintf(response, sizeof(response), " Invalid values \r\n");
            usb_send_response(response);
        }
    }
    else if (strcmp(tokens[0], "add_current_color") == 0)
    {
        uint32_t h = hsv_value.hue;
        uint32_t s = hsv_value.saturation;
        uint32_t v = hsv_value.value;
        int index = 0;
        if ((index = find_spot_to_save_light(color_list, MAX_NUMBER_OF_SAVED_COLORS)) != -1)
        {
            color_list[index].hue = h;
            color_list[index].saturation = s;
            color_list[index].value = v;
            strncpy((char *)color_list[index].name, tokens[1], MAX_NAME_LENGHT);
            snprintf(response, sizeof(response), "\r\ncolor %s saved\r\n", tokens[4]);
            usb_send_response(response);
        }
        else
        {
            snprintf(response, sizeof(response), "list is full");
            usb_send_response(response);
        }
    }
    else if (strcmp(tokens[0], "del_color") == 0)
    {
        int index = find_color_in_list(color_list, MAX_NUMBER_OF_SAVED_COLORS, tokens[1]);
        if (index != -1)
        {
            memset(&color_list[index], 0, sizeof(color_list[index]));
            snprintf(response, sizeof(response), "\r\nColor '%s' deleted successfully.\r\n", tokens[1]);
            usb_send_response(response);
            return;
        }
        snprintf(response, sizeof(response), "\r\nColor '%s' not found.\r\n", tokens[1]);
        usb_send_response(response);
    }
    else if (strcmp(tokens[0], "apply_color") == 0)
    {
        int index = find_color_in_list(color_list, MAX_NUMBER_OF_SAVED_COLORS, tokens[1]);
        if (index != -1)
        {
            hsv_value.hue = color_list[index].hue;
            hsv_value.saturation = color_list[index].saturation;
            hsv_value.value = color_list[index].value;
            snprintf(response, sizeof(response), "\r\ncolor set to %s \r\n", tokens[1]);
            usb_send_response(response);
        }
        else
        {
            snprintf(response, sizeof(response), "\r\ncolor not found \r\n");
            usb_send_response(response);
        }
    }
    else if (strcmp(tokens[0], "list_colors") == 0)
    {
        print_color_list(color_list, MAX_NUMBER_OF_SAVED_COLORS);
    }
    else
    {
        usb_send_response("\r\nUnknown command. Type 'help' for a list of commands.\r\n");
    }
}

int find_spot_to_save_light(saved_color_t *color_list, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (color_list[i].name[0] == '\0')
        {
            return i;
        }
    }
    return -1;
}

void print_color_list(saved_color_t *color_list, int size)
{
    char response[64];
    for (int i = 0; i < size; i++)
    {
        if (color_list[i].name[0] != '\0')
        {
            strncat(response, "\r\n", 2);
            strncat(response, color_list[i].name, MAX_NAME_LENGHT);
        }
    }
    strncat(response, "\r\n", 2);
    usb_send_response(response);
}

int find_color_in_list(saved_color_t *color_list, int size, const char *name)
{
    for (int i = 0; i < size; i++)
    {
        if (strcmp(color_list[i].name, name) == 0)
        {
            return i;
            break;
        }
    }
    return -1;
}

void usb_send_response(const char *response)
{
    app_usbd_cdc_acm_write(&usb_cdc_acm, response, strlen(response));
}