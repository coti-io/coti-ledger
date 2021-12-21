#include "shared_context.h"
#include "get_public_key.h"
#include "ui_callbacks.h"
#include "apdu_constants.h"

unsigned int io_seproxyhal_touch_address_ok(const bagl_element_t *e)
{
    uint16_t txLength = setResultGetPublicKey();
    const uint16_t sw = SW_OK;
    G_io_apdu_buffer[txLength++] = sw >> 8;
    G_io_apdu_buffer[txLength++] = sw & 0xFF;
    resetAppContext();
    // Send back the response, do not restart the event loop
    io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, txLength);
    // Display back the original UX
    uiIdle();
    // do not redraw the widget
    return 0;
}

unsigned int io_seproxyhal_touch_address_cancel(const bagl_element_t *e)
{
    const uint16_t sw = SW_REJECTED_BY_USER;
    G_io_apdu_buffer[0] = sw >> 8;
    G_io_apdu_buffer[1] = sw & 0xFF;
    resetAppContext();
    // Send back the response, do not restart the event loop
    io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, 2);
    // Display back the original UX
    uiIdle();
    // do not redraw the widget
    return 0;
}
