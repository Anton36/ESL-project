
#include "nrfx_nvmc.h"
#include "nrf_nvmc.h"
#include "nrf_log.h"
#include "led_handler.h"
#include <string.h>
#include <stdint.h>
#include "memory_handler.h"

#define BOOTLOADER_START_ADDR 0x000E0000
#define PAGE_SIZE 0x1000
#define DATA_PAGE_START (BOOTLOADER_START_ADDR - (3 * PAGE_SIZE))
#define DATA_PAGE_END (DATA_PAGE_START + PAGE_SIZE)
#define EMPTY_DATA_VALUE 0xFFFFFFFF
#define WORD_SIZE (sizeof(uint32_t))

memory_control_structure_t memory_control = {
    .erase_necessity = false,
    .data_size = sizeof(hsv_value),
    .actual_adress = NULL,
};

bool find_last_data_to_read(uint32_t *data_size, uint32_t **actual_adress, bool *erase_necessity)
{

    for (uint32_t i = DATA_PAGE_START; i <= DATA_PAGE_END; i += WORD_SIZE)
    {
        if (read_word((uint32_t *)i) == EMPTY_DATA_VALUE)
        {
            uint32_t start_of_data = i - *data_size;
            *actual_adress = (uint32_t *)start_of_data;
            return true;
        }
    }
    *erase_necessity = true;
    return false;
}
uint32_t read_word(uint32_t *address)
{
    uint32_t data = *address;
    return data;
}

void read_from_memory(uint32_t *hue, uint32_t *saturation, uint32_t *value)
{

    if (find_last_data_to_read(&memory_control.data_size, &memory_control.actual_adress, &memory_control.erase_necessity) == false)
        return;

    uint32_t data_to_read[memory_control.data_size / WORD_SIZE];

    for (uint32_t i = 0; i < memory_control.data_size / WORD_SIZE; i++)
    {
        data_to_read[i] = read_word(memory_control.actual_adress++);
    }
    if (data_to_read[0] <= HUE_MAX_VALUE && data_to_read[1] <= PWM_TOP_VALUE && data_to_read[2] <= PWM_TOP_VALUE)
    {
        *hue = data_to_read[0];
        *saturation = data_to_read[1];
        *value = data_to_read[2];
        NRF_LOG_INFO("Loaded color: H:%d S:%d B:%d", *hue, *saturation, *value);
    }
}

void write_to_memory(uint32_t hue, uint32_t saturation, uint32_t value)
{
    if (memory_control.erase_necessity == true)
    {
        nrfx_nvmc_page_erase(DATA_PAGE_START);
        memory_control.actual_adress = (uint32_t *)DATA_PAGE_START;
        memory_control.erase_necessity = false;
    }
    else
    {
    }

    uint32_t data_to_write[3] = {hue, saturation, value};

    for (size_t i = 0; i < (memory_control.data_size / WORD_SIZE); i++)
    {

        if (nrfx_nvmc_word_writable_check((uint32_t)memory_control.actual_adress, data_to_write[i]))
        {
            nrfx_nvmc_word_write((uint32_t)memory_control.actual_adress, data_to_write[i]);
        }
        while (nrfx_nvmc_write_done_check() != true)
        {
            NRF_LOG_INFO("Writing in progress");
        }
        memory_control.actual_adress++;
        NRF_LOG_INFO("Writing is complete");
    }
}