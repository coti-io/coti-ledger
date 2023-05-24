#include "io.h"
#include <os.h>
#include <ux.h>
#include <os_io_seproxyhal.h>

void io_seproxyhal_display(const bagl_element_t *element)
{
    io_seproxyhal_display_default(element);
}

uint8_t io_event(uint8_t channel)
{
    UNUSED(channel);
    // Can't have more than one tag in the reply, not supported yet.
    switch (G_io_seproxyhal_spi_buffer[0])
    {
    case SEPROXYHAL_TAG_BUTTON_PUSH_EVENT:
        UX_BUTTON_PUSH_EVENT(G_io_seproxyhal_spi_buffer)
        break;

    case SEPROXYHAL_TAG_STATUS_EVENT:
        if ((G_io_apdu_media == IO_APDU_MEDIA_USB_HID) && ((U4BE(G_io_seproxyhal_spi_buffer, 3) & SEPROXYHAL_TAG_STATUS_EVENT_FLAG_USB_POWERED) != 0))
        {
            THROW(EXCEPTION_IO_RESET);
        }
        // no break is intentional
        __attribute__((fallthrough));
    case SEPROXYHAL_TAG_DISPLAY_PROCESSED_EVENT:
        UX_DISPLAYED_EVENT({})
        break;
    case SEPROXYHAL_TAG_TICKER_EVENT:
        UX_TICKER_EVENT(G_io_seproxyhal_spi_buffer, {});
        break;
    default:
        UX_DEFAULT_EVENT()
        break;
    }

    // close the event if not done previously (by a display or whatever)
    if (io_seproxyhal_spi_is_status_sent() != 0)
    {
        io_seproxyhal_general_status();
    }

    // command has been processed, DO NOT reset the current APDU transport
    return 1;
}

uint16_t io_exchange_al(uint8_t channel, uint16_t txLength)
{
    switch (channel & (uint8_t)~IO_FLAGS)
    {
    case CHANNEL_KEYBOARD:
        break;

        // multiplexed io exchange over an SPI channel and TLV encapsulated protocol
    case CHANNEL_SPI:
        if (txLength != 0)
        {
            io_seproxyhal_spi_send(G_io_apdu_buffer, txLength);

            if ((channel & IO_RESET_AFTER_REPLIED) != 0)
            {
                reset();
            }
            // nothing received from the master so far (it's a tx transaction)
            return 0;
        }
        else
        {
            return io_seproxyhal_spi_recv(G_io_apdu_buffer, sizeof(G_io_apdu_buffer), 0);
        }

    default:
        THROW(INVALID_PARAMETER);
    }

    return 0;
}
