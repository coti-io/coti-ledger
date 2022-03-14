#pragma once

#include "shared_context.h"

uint32_t io_seproxyhal_touch_tx_ok(const bagl_element_t *e);
uint32_t io_seproxyhal_touch_tx_cancel(const bagl_element_t *e);
uint32_t io_seproxyhal_touch_address_ok(const bagl_element_t *e);
uint32_t io_seproxyhal_touch_address_cancel(const bagl_element_t *e);
uint32_t io_seproxyhal_touch_sign_message_ok(const bagl_element_t *e);
uint32_t io_seproxyhal_touch_sign_message_cancel(const bagl_element_t *e);
uint32_t io_seproxyhal_touch_data_ok(const bagl_element_t *e);
uint32_t io_seproxyhal_touch_data_cancel(const bagl_element_t *e);
