#ifndef MEMORY_HANDLER_H
#define MEMORY_HANDLER_H


#include <stdint.h>
#include "nrfx_nvmc.h"
#include "nrf_nvmc.h"
#include "led_handler.h"


uint32_t address;

typedef struct 
{
    bool erase_necessity;
    uint32_t data_size;
    uint32_t *actual_adress;

} memory_control_structure_t;

void write_to_memory(uint32_t hue, uint32_t saturation, uint32_t value);
bool find_last_data_to_read(uint32_t *data_size,uint32_t **actual_adress,bool *erase_necessity);
uint32_t read_word(uint32_t *address);


void read_from_memory(uint32_t *hue,uint32_t *saturation, uint32_t *value);

#endif