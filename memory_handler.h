#ifndef MEMORY_HANDLER_H
#define MEMORY_HANDLER_H

#define BOOTLOADER_START_ADDR  0x000E0000
#define PAGE_SIZE  0x1000
#define DATA_PAGE_START (BOOTLOADER_START_ADDR - (3 * PAGE_SIZE) )
#include <stdint.h>
#include "nrfx_nvmc.h"
#include "nrf_nvmc.h"
#include "led_handler.h"

uint32_t data_to_write[3];
uint32_t address;

typedef struct 
{
    bool erase_necessity;
    uint32_t data_size;
    uint32_t *adress_to_write;

} memory_control_structure_t;

void write_to_memory(uint32_t hue, uint32_t saturation, uint32_t value);



void read_from_memory(uint32_t *hue,uint32_t *saturation, uint32_t *value);

#endif