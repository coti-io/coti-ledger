#include "shared_context.h"
#include "get_public_key.h"
#include "ui_callbacks.h"
#include "apdu_constants.h"

unsigned int io_seproxyhal_touch_address_ok(const bagl_element_t *e)
{
    uint32_t tx = set_result_get_public_key();
    unsigned short sw = SW_OK;
    G_io_apdu_buffer[tx++] = sw >> 8;
    G_io_apdu_buffer[tx++] = sw & 0xFF;
    reset_app_context();
    // Send back the response, do not restart the event loop
    io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, tx);
    // Display back the original UX
    ui_idle();
    return 0; // do not redraw the widget
}

unsigned int io_seproxyhal_touch_address_cancel(const bagl_element_t *e)
{
    unsigned short sw = SW_REJECTED_BY_USER;
    G_io_apdu_buffer[0] = sw >> 8;
    G_io_apdu_buffer[1] = sw & 0xFF;
    reset_app_context();
    // Send back the response, do not restart the event loop
    io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, 2);
    // Display back the original UX
    ui_idle();
    return 0; // do not redraw the widget
}
