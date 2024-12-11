#include <stdbool.h>
#include <stdint.h>

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


usb_service_handler_t usb_service_handler = {
    .buffer_index = 0,
} ;

void command_buffer_char_append(char ch)
{
    if (usb_service_handler.buffer_index <= COMAND_BUFFER_SIZE - 1)
    {
        usb_service_handler.command_buffer[usb_service_handler.buffer_index] = ch;
        usb_service_handler.buffer_index++;
        NRF_LOG_INFO("command buffer : %d", usb_service_handler.buffer_index);
    }

}

void command_handler()
{

}

static void usb_ev_handler(app_usbd_class_inst_t const * p_inst,
                           app_usbd_cdc_acm_user_event_t event);

/* Make sure that they don't intersect with LOG_BACKEND_USB_CDC_ACM */
#define CDC_ACM_COMM_INTERFACE  2
#define CDC_ACM_COMM_EPIN       NRF_DRV_USBD_EPIN3

#define CDC_ACM_DATA_INTERFACE  3
#define CDC_ACM_DATA_EPIN       NRF_DRV_USBD_EPIN4
#define CDC_ACM_DATA_EPOUT      NRF_DRV_USBD_EPOUT4

APP_USBD_CDC_ACM_GLOBAL_DEF(usb_cdc_acm,
                            usb_ev_handler,
                            CDC_ACM_COMM_INTERFACE,
                            CDC_ACM_DATA_INTERFACE,
                            CDC_ACM_COMM_EPIN,
                            CDC_ACM_DATA_EPIN,
                            CDC_ACM_DATA_EPOUT,
                            APP_USBD_CDC_COMM_PROTOCOL_NONE);


static void usb_ev_handler(app_usbd_class_inst_t const * p_inst,
                           app_usbd_cdc_acm_user_event_t event)
{
    switch (event)
    {
    case APP_USBD_CDC_ACM_USER_EVT_PORT_OPEN:
    {
        NRF_LOG_INFO("Port open");
        ret_code_t ret;
        bsp_board_led_on(0);
        ret = app_usbd_cdc_acm_read(&usb_cdc_acm, usb_service_handler.rx_buffer, READ_SIZE);
        UNUSED_VARIABLE(ret);
        break;
    }
    case APP_USBD_CDC_ACM_USER_EVT_PORT_CLOSE:
    {
        bsp_board_led_off(0);
        break;
    }
    case APP_USBD_CDC_ACM_USER_EVT_TX_DONE:
    {
        NRF_LOG_INFO("tx done");
        bsp_board_led_invert(1);
        break;
    }
    case APP_USBD_CDC_ACM_USER_EVT_RX_DONE:
    {
        NRF_LOG_INFO("rx enter");
        bsp_board_led_invert(2);
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
    app_usbd_class_inst_t const * class_cdc_acm = app_usbd_cdc_acm_class_inst_get(&usb_cdc_acm);
    ret_code_t ret = app_usbd_class_append(class_cdc_acm);
    APP_ERROR_CHECK(ret);

}

