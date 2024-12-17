
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
#define DATA_PAGE2_START (BOOTLOADER_START_ADDR - (2 * PAGE_SIZE))
#define DATA_PAGE_END (DATA_PAGE_START + PAGE_SIZE)
#define DATA_PAGE2_END (DATA_PAGE2_START + PAGE_SIZE)
#define EMPTY_DATA_VALUE 0xFFFFFFFF
#define WORD_SIZE (sizeof(uint32_t))
extern saved_color_t color_list[MAX_NUMBER_OF_SAVED_COLORS];
memory_control_structure_t memory_control_for_page1 = {
    .erase_necessity = false,
    .data_size = sizeof(hsv_value),
    .actual_adress = NULL,
};
memory_control_structure_t memory_control_for_page2 = {
    .erase_necessity = false,
    .data_size = sizeof(color_list),
    .actual_adress = NULL,
};

bool find_last_data_to_read(uint32_t *data_size, uint32_t **actual_adress, bool *erase_necessity, uint32_t start_of_page, uint32_t end_of_page)
{

    for (uint32_t i = start_of_page; i <= end_of_page; i += WORD_SIZE)
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

void read_HSV_from_memory(uint32_t *hue, uint32_t *saturation, uint32_t *value)
{

    if (find_last_data_to_read(&memory_control_for_page1.data_size, &memory_control_for_page1.actual_adress, &memory_control_for_page1.erase_necessity, DATA_PAGE_START, DATA_PAGE_END) == false)
        return;

    uint32_t data_to_write[memory_control_for_page1.data_size / WORD_SIZE];

    for (uint32_t i = 0; i < memory_control_for_page1.data_size / WORD_SIZE; i++)
    {
        data_to_write[i] = read_word(memory_control_for_page1.actual_adress++);
    }
    if (data_to_write[0] <= HUE_MAX_VALUE && data_to_write[1] <= PWM_TOP_VALUE && data_to_write[2] <= PWM_TOP_VALUE)
    {
        *hue = data_to_write[0];
        *saturation = data_to_write[1];
        *value = data_to_write[2];
        NRF_LOG_INFO("Loaded color: H:%d S:%d B:%d", *hue, *saturation, *value);
    }
}

void write_HSV_to_memory(uint32_t hue, uint32_t saturation, uint32_t value)
{
    if (memory_control_for_page1.erase_necessity == true)
    {
        nrfx_nvmc_page_erase(DATA_PAGE_START);
        memory_control_for_page1.actual_adress = (uint32_t *)DATA_PAGE_START;
        memory_control_for_page1.erase_necessity = false;
    }

    uint32_t data_to_write[3] = {hue, saturation, value};
    size_t total_words = memory_control_for_page1.data_size / WORD_SIZE;

    for (size_t i = 0; i < total_words; i++)
    {

        if (nrfx_nvmc_word_writable_check((uint32_t)memory_control_for_page1.actual_adress, data_to_write[i]))
        {
            nrfx_nvmc_word_write((uint32_t)memory_control_for_page1.actual_adress, data_to_write[i]);
        }
        while (nrfx_nvmc_write_done_check() != true)
        {
            NRF_LOG_INFO("Writing in progress");
        }
        memory_control_for_page1.actual_adress++;
        NRF_LOG_INFO("Writing is complete");
    }
}

void write_colors_to_memory(saved_color_t *color_list, int size)
{

    if (memory_control_for_page2.erase_necessity == true)
    {
        nrfx_nvmc_page_erase(DATA_PAGE2_START);
        memory_control_for_page2.actual_adress = (uint32_t *)DATA_PAGE2_START;
        memory_control_for_page2.erase_necessity = false;
    }
    uint32_t *data_to_write = (uint32_t *)color_list;
    size_t total_words = memory_control_for_page2.data_size / WORD_SIZE;
    NRF_LOG_INFO("Writing starting");

    for (size_t i = 0; i < total_words; i++)
    {

        if (nrfx_nvmc_word_writable_check((uint32_t)memory_control_for_page2.actual_adress, data_to_write[i]))
        {
            nrfx_nvmc_word_write((uint32_t)memory_control_for_page2.actual_adress, data_to_write[i]);
        }

        while (nrfx_nvmc_write_done_check() != true)
        {
            NRF_LOG_INFO("Writing in progress");
        }


        memory_control_for_page2.actual_adress++;
    }
}

void read_colors_from_memory(saved_color_t *color_list, int size)
{

    if (find_last_data_to_read(&memory_control_for_page2.data_size, &memory_control_for_page2.actual_adress, &memory_control_for_page2.erase_necessity, DATA_PAGE2_START, DATA_PAGE2_END) == false)
        return;

    size_t words_to_read = memory_control_for_page2.data_size / WORD_SIZE;
    uint32_t data_buffer[words_to_read];

    for (uint32_t i = 0; i < words_to_read; i++)
    {
        data_buffer[i] = read_word(memory_control_for_page2.actual_adress++);
    }
    memcpy(color_list, data_buffer, memory_control_for_page2.data_size);

    for (int i = 0; i < size; i++)
    {
        if (color_list[i].name[0] == '\0')
        {
            memset(&color_list[i], 0, sizeof(color_list[i]));
        }
    }
}