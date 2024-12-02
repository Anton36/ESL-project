
#include "nrfx_nvmc.h"
#include "nrf_nvmc.h"
#include "nrf_log.h"
#include "led_handler.h"
#include <string.h>
#include <stdint.h>
#include "memory_handler.h"
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
    uint32_t hue_t, saturation_t, value_t;

    hue_t = *((uint32_t *)(DATA_PAGE_START));                            // Читаем первое значение
    saturation_t = *((uint32_t *)(DATA_PAGE_START + sizeof(uint32_t)));  // Читаем второе значение
    value_t = *((uint32_t *)(DATA_PAGE_START + (sizeof(uint32_t) * 2))); // Читаем третье значение

    if (hue_t <= HUE_MAX_VALUE && saturation_t <= PWM_TOP_VALUE && value_t <= PWM_TOP_VALUE)
    {
        *hue = hue_t;
        *saturation = saturation_t;
        *value = value_t;
        NRF_LOG_INFO("Loaded color: H:%d S:%d B:%d", *hue, *saturation, *value);
    }
}