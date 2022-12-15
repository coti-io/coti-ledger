#pragma once

#include <stdint.h>
#include <ux.h>

void io_seproxyhal_display(const bagl_element_t *element);
uint8_t io_event(uint8_t channel);
uint16_t io_exchange_al(uint8_t channel, uint16_t txLength);
