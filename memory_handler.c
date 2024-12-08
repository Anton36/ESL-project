
#include "nrfx_nvmc.h"
#include "nrf_nvmc.h"
#include "nrf_log.h"
#include "led_handler.h"
#include <string.h>
#include <stdint.h>
#include "memory_handler.h"


memory_control_structure_t memory_control = {
    .erase_necessity = true,
    .data_size = sizeof(hsv_value),
    .adress_to_write = NULL,
};



void write_to_memory(uint32_t hue, uint32_t saturation, uint32_t value)
{
    nrfx_nvmc_page_erase(DATA_PAGE_START);
    uint32_t data_to_write[3] = {hue, saturation, value};

    for (size_t i = 0; i < sizeof(data_to_write) / sizeof(data_to_write[0]); i++)
    {
        uint32_t address = DATA_PAGE_START + i * 4;

        if (nrfx_nvmc_word_writable_check(address, data_to_write[i]))
        {

            nrfx_nvmc_word_write(address, data_to_write[i]);
        }
        while (nrfx_nvmc_write_done_check() != true)
        {
            NRF_LOG_INFO("Writing in progress");
        }
        NRF_LOG_INFO("Writing is complete");
    }
}

void read_from_memory(uint32_t *hue, uint32_t *saturation, uint32_t *value)
{
    uint32_t hue_temp;
    uint32_t saturation_temp;
    uint32_t value_temp;

    hue_temp = *((uint32_t *)(DATA_PAGE_START));
    saturation_temp = *((uint32_t *)(DATA_PAGE_START + sizeof(uint32_t)));
    value_temp = *((uint32_t *)(DATA_PAGE_START + (sizeof(uint32_t) * 2)));

    if (hue_temp <= HUE_MAX_VALUE && saturation_temp <= PWM_TOP_VALUE && value_temp <= PWM_TOP_VALUE)
    {
        *hue = hue_temp;
        *saturation = saturation_temp;
        *value = value_temp;
        NRF_LOG_INFO("Loaded color: H:%d S:%d B:%d", *hue, *saturation, *value);
    }
}