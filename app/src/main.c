/*******************************************************************************
*   Ledger App Coti
*   (c) 2017 Ledger
*   (c) 2020 Tchain Ltd. adaptation for COTI
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
********************************************************************************/

#include "shared_context.h"
#include "apdu_constants.h"
#include "ui_callbacks.h"

#ifdef HAVE_UX_FLOW
#include "ui_flow.h"
#endif

#include <os_io_seproxyhal.h>

#include "glyphs.h"
#include "utils.h"

unsigned char G_io_seproxyhal_spi_buffer[IO_SEPROXYHAL_BUFFER_SIZE_B];

tmp_ctx_t tmp_ctx;
strings_t strings;

cx_sha3_t sha3;

uint8_t app_state;

#ifdef HAVE_UX_FLOW
#include "ux.h"
ux_state_t G_ux;
bolos_ux_params_t G_ux_params;
#else  // HAVE_UX_FLOW
ux_state_t ux;

// display stepped screens
unsigned int ux_step;
unsigned int ux_step_count;
#endif // HAVE_UX_FLOW

void reset_app_context()
{
    PRINTF("!!RESET_APP_CONTEXT\n");
    app_state = APP_STATE_IDLE;
    os_memset((uint8_t *)&tmp_ctx, 0, sizeof(tmp_ctx));
}

void ui_idle(void)
{
#if defined(HAVE_UX_FLOW)
    // reserve a display stack slot if none yet
    if (G_ux.stack_count == 0)
    {
        ux_stack_push();
    }
    ux_flow_init(0, ux_idle_flow, NULL);
#endif // #if TARGET_ID
}

#if defined(TARGET_NANOS)
unsigned int ui_address_nanos_button(unsigned int button_mask, unsigned int button_mask_counter)
{
    switch (button_mask)
    {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT: // CANCEL
        io_seproxyhal_touch_address_cancel(NULL);
        break;

    case BUTTON_EVT_RELEASED | BUTTON_RIGHT:
    { // OK
        io_seproxyhal_touch_address_ok(NULL);
        break;
    }
    }
    return 0;
}
#endif // #if defined(TARGET_NANOS)

void format_signature_out(const uint8_t *signature)
{
    os_memset(G_io_apdu_buffer + 1, 0x00, 64);
    uint8_t offset = 1;
    uint8_t xoffset = 4; //point to r value
    //copy r
    uint8_t xlength = signature[xoffset - 1];
    if (xlength == 33)
    {
        xlength = 32;
        xoffset++;
    }
    memmove(G_io_apdu_buffer + offset + 32 - xlength, signature + xoffset, xlength);
    offset += 32;
    xoffset += xlength + 2; //move over rvalue and TagLEn
    //copy s value
    xlength = signature[xoffset - 1];
    if (xlength == 33)
    {
        xlength = 32;
        xoffset++;
    }
    memmove(G_io_apdu_buffer + offset + 32 - xlength, signature + xoffset, xlength);
}

#if defined(TARGET_NANOS)
unsigned int ui_approval_nanos_button(unsigned int button_mask, unsigned int button_mask_counter)
{
    switch (button_mask)
    {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT:
        io_seproxyhal_touch_tx_cancel(NULL);
        break;

    case BUTTON_EVT_RELEASED | BUTTON_RIGHT:
    {
        io_seproxyhal_touch_tx_ok(NULL);
        break;
    }
    }
    return 0;
}

unsigned int ui_approval_sign_message_nanos_button(unsigned int button_mask, unsigned int button_mask_counter)
{
    switch (button_mask)
    {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT:
        io_seproxyhal_touch_sign_message_cancel(NULL);
        break;

    case BUTTON_EVT_RELEASED | BUTTON_RIGHT:
    {
        io_seproxyhal_touch_sign_message_ok(NULL);
        break;
    }
    }
    return 0;
}

unsigned int ui_data_selector_nanos_button(unsigned int button_mask,
                                           unsigned int button_mask_counter)
{
    switch (button_mask)
    {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT:
        io_seproxyhal_touch_data_cancel(NULL);
        break;

    case BUTTON_EVT_RELEASED | BUTTON_RIGHT:
    {
        io_seproxyhal_touch_data_ok(NULL);
        break;
    }
    }
    return 0;
}

unsigned int ui_data_parameter_nanos_button(unsigned int button_mask,
                                            unsigned int button_mask_counter)
{
    switch (button_mask)
    {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT:
        io_seproxyhal_touch_data_cancel(NULL);
        break;

    case BUTTON_EVT_RELEASED | BUTTON_RIGHT:
    {
        io_seproxyhal_touch_data_ok(NULL);
        break;
    }
    }
    return 0;
}

#endif // #if defined(TARGET_NANOS)

unsigned short io_exchange_al(unsigned char channel, unsigned short tx_len)
{
    switch (channel & ~(IO_FLAGS))
    {
    case CHANNEL_KEYBOARD:
        break;

        // multiplexed io exchange over a SPI channel and TLV encapsulated protocol
    case CHANNEL_SPI:
        if (tx_len)
        {
            io_seproxyhal_spi_send(G_io_apdu_buffer, tx_len);

            if (channel & IO_RESET_AFTER_REPLIED)
            {
                reset();
            }
            return 0; // nothing received from the master so far (it's a tx transaction)
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

void handleApdu(unsigned int *flags, unsigned int *tx)
{
    unsigned short sw = 0;

    BEGIN_TRY
    {
        TRY
        {
            if (G_io_apdu_buffer[OFFSET_CLA] != CLA)
            {
                THROW(0x6E00);
            }

            switch (G_io_apdu_buffer[OFFSET_INS])
            {
            case INS_GET_PUBLIC_KEY:
                handle_get_public_key(G_io_apdu_buffer[OFFSET_P1], G_io_apdu_buffer[OFFSET_P2], G_io_apdu_buffer + OFFSET_CDATA, G_io_apdu_buffer[OFFSET_LC], flags, tx);
                break;

            case INS_SIGN_MESSAGE:
                handle_sign_message(G_io_apdu_buffer[OFFSET_P1], G_io_apdu_buffer[OFFSET_P2], G_io_apdu_buffer + OFFSET_CDATA, G_io_apdu_buffer[OFFSET_LC], flags, tx);
                break;

            default:
                THROW(0x6D00);
                break;
            }
        }
        CATCH(EXCEPTION_IO_RESET)
        {
            THROW(EXCEPTION_IO_RESET);
        }
        CATCH_OTHER(e)
        {
            switch (e & 0xF000)
            {
            case 0x6000:
                // Wipe the transaction context and report the exception
                sw = e;
                reset_app_context();
                break;
            case 0x9000:
                // All is well
                sw = e;
                break;
            default:
                // Internal error
                sw = 0x6800 | (e & 0x7FF);
                reset_app_context();
                break;
            }
            // Unexpected exception => report
            G_io_apdu_buffer[*tx] = sw >> 8;
            G_io_apdu_buffer[*tx + 1] = sw;
            *tx += 2;
        }
        FINALLY
        {
        }
    }
    END_TRY;
}

void sample_main(void)
{
    unsigned int rx = 0;
    unsigned int tx = 0;
    unsigned int flags = 0;

    // DESIGN NOTE: the bootloader ignores the way APDU are fetched. The only
    // goal is to retrieve APDU.
    // When APDU are to be fetched from multiple IOs, like NFC+USB+BLE, make
    // sure the io_event is called with a
    // switch event, before the apdu is replied to the bootloader. This avoid
    // APDU injection faults.
    for (;;)
    {
        unsigned short sw = 0;

        BEGIN_TRY
        {
            TRY
            {
                rx = tx;
                tx = 0; // ensure no race in catch_other if io_exchange throws
                        // an error
                rx = io_exchange(CHANNEL_APDU | flags, rx);
                flags = 0;

                // no apdu received, well, reset the session, and reset the
                // bootloader configuration
                if (rx == 0)
                {
                    THROW(0x6982);
                }
                PRINTF("New APDU received:\n%.*H\n", rx, G_io_apdu_buffer);

                handleApdu(&flags, &tx);
            }
            CATCH(EXCEPTION_IO_RESET)
            {
                THROW(EXCEPTION_IO_RESET);
            }
            CATCH_OTHER(e)
            {
                switch (e & 0xF000)
                {
                case 0x6000:
                    // Wipe the transaction context and report the exception
                    sw = e;
                    reset_app_context();
                    break;
                case 0x9000:
                    // All is well
                    sw = e;
                    break;
                default:
                    // Internal error
                    sw = 0x6800 | (e & 0x7FF);
                    reset_app_context();
                    break;
                }
                if (e != 0x9000)
                {
                    flags &= ~IO_ASYNCH_REPLY;
                }
                // Unexpected exception => report
                G_io_apdu_buffer[tx] = sw >> 8;
                G_io_apdu_buffer[tx + 1] = sw;
                tx += 2;
            }
            FINALLY
            {
            }
        }
        END_TRY;
    }

    return;
}

void io_seproxyhal_display(const bagl_element_t *element)
{
    io_seproxyhal_display_default((bagl_element_t *)element);
}

unsigned char io_event(unsigned char channel)
{
    // Can't have more than one tag in the reply, not supported yet.
    switch (G_io_seproxyhal_spi_buffer[0])
    {
    case SEPROXYHAL_TAG_FINGER_EVENT:
        UX_FINGER_EVENT(G_io_seproxyhal_spi_buffer);
        break;

    case SEPROXYHAL_TAG_BUTTON_PUSH_EVENT:
        UX_BUTTON_PUSH_EVENT(G_io_seproxyhal_spi_buffer);
        break;

    case SEPROXYHAL_TAG_STATUS_EVENT:
        if (G_io_apdu_media == IO_APDU_MEDIA_USB_HID && !(U4BE(G_io_seproxyhal_spi_buffer, 3) & SEPROXYHAL_TAG_STATUS_EVENT_FLAG_USB_POWERED))
        {
            THROW(EXCEPTION_IO_RESET);
        }
        // no break is intentional

    default:
        UX_DEFAULT_EVENT();
        break;

    case SEPROXYHAL_TAG_DISPLAY_PROCESSED_EVENT:
        UX_DISPLAYED_EVENT({});
        break;
    }

    // close the event if not done previously (by a display or whatever)
    if (!io_seproxyhal_spi_is_status_sent())
    {
        io_seproxyhal_general_status();
    }

    // command has been processed, DO NOT reset the current APDU transport
    return 1;
}

void app_exit(void)
{

    BEGIN_TRY_L(exit)
    {
        TRY_L(exit)
        {
            os_sched_exit(-1);
        }
        FINALLY_L(exit)
        {
        }
    }
    END_TRY_L(exit);
}

__attribute__((section(".boot"))) int main(int argc, char *argv[])
{
    // exit critical section
    __asm volatile("cpsie i");

    reset_app_context();

    // ensure exception will work as planned
    os_boot();

    for (;;)
    {
        UX_INIT();

        BEGIN_TRY
        {
            TRY
            {
                io_seproxyhal_init();

#ifdef TARGET_NANOX
                // grab the current plane mode setting
                G_io_app.plane_mode = os_setting_get(OS_SETTING_PLANEMODE, NULL, 0);
#endif // TARGET_NANOX
                USB_power(0);
                USB_power(1);

                ui_idle();

#ifdef HAVE_BLE
                BLE_power(0, NULL);
                BLE_power(1, "Nano X");
#endif // HAVE_BLE
                sample_main();
            }
            CATCH(EXCEPTION_IO_RESET)
            {
                // reset IO and UX before continuing
                CLOSE_TRY;
                continue;
            }
            CATCH_ALL
            {
                CLOSE_TRY;
                break;
            }
            FINALLY
            {
            }
        }
        END_TRY;
    }

    app_exit();

    return 0;
}
