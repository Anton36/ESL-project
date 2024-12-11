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

void command_handler()
{
    memcpy(usb_service_handler.command, usb_service_handler.command_buffer, sizeof(usb_service_handler.command_buffer));
    memset(usb_service_handler.command_buffer, 0, sizeof(usb_service_handler.command_buffer));
    usb_service_handler.buffer_index = 0;

    char tokens[MAX_NUMBER_OF_OPERATORS][MAX_PARAMETR_SIZE];
    string_split(usb_service_handler.command, (char *)tokens, MAX_NUMBER_OF_OPERATORS, MAX_PARAMETR_SIZE);

    char response[64];
    char *pend = NULL;
    if (strcmp(tokens[0], "RGB") == 0)
    {
        uint32_t r = strtol(tokens[1], &pend, 10);
        uint32_t g = strtol(tokens[2], &pend, 10);
        uint32_t b = strtol(tokens[3], &pend, 10);
        rgb_to_hsv(r, g, b, &hsv_value.hue, &hsv_value.saturation, &hsv_value.value);
        snprintf(response, sizeof(response), "\r\nRGB set to %lu %lu %lu\r\n", r, g, b);
        usb_send_response(response);
    }
    else if (strcmp(tokens[0], "HSV") == 0)
    {

        uint32_t h = strtol(tokens[1], &pend, 10);
        uint32_t s = strtol(tokens[2], &pend, 10);
        uint32_t v = strtol(tokens[3], &pend, 10);
        hsv_value.hue = h;
        hsv_value.saturation = s;
        hsv_value.value = v;
        snprintf(response, sizeof(response), "\r\nHSV set to %lu %lu %lu\r\n", h, s, v);
        usb_send_response(response);
    }
    else if (strcmp(tokens[0], "help") == 0)
    {
        usb_send_response("\r\nAvailable commands:\r\n RGB <r> <g> <b> - Set color using RGB \r\n HSV <h> <s> <v> - Set color using HSV \r\n");
    }
    else
    {
        usb_send_response("\r\nUnknown command. Type 'help' for a list of commands.\r\n");
    }
}

void usb_send_response(const char *response)
{
    app_usbd_cdc_acm_write(&usb_cdc_acm, response, strlen(response));
}