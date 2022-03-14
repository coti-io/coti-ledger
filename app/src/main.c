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
#include "apdu_handlers.h"
#include "apdu_utils.h"
#include "ui_callbacks.h"

#ifdef HAVE_UX_FLOW
#include "ui_flow.h"
#include "ux.h"
#endif

#include "glyphs.h"
#include "utils.h"

unsigned char G_io_seproxyhal_spi_buffer[IO_SEPROXYHAL_BUFFER_SIZE_B];

AppContext_t appContext;
Strings_t strings;

cx_sha3_t sha3;

uint8_t appState;

#ifdef HAVE_UX_FLOW
ux_state_t G_ux;
bolos_ux_params_t G_ux_params;
#else  // HAVE_UX_FLOW
ux_state_t ux;

// display stepped screens
unsigned int ux_step;
unsigned int ux_step_count;
#endif // HAVE_UX_FLOW

#if defined(TARGET_NANOS)
unsigned int ui_address_nanos_button(unsigned int button_mask, unsigned int button_mask_counter)
{
    switch (button_mask)
    {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT: // CANCEL
        io_seproxyhal_touch_address_cancel(NULL);
        break;

    case BUTTON_EVT_RELEASED | BUTTON_RIGHT:
        // OK
        io_seproxyhal_touch_address_ok(NULL);
        break;
    default:
        break;
    }
    return 0;
}
#endif // #if defined(TARGET_NANOS)

#if defined(TARGET_NANOS)
unsigned int ui_approval_nanos_button(unsigned int button_mask, unsigned int button_mask_counter)
{
    switch (button_mask)
    {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT:
        io_seproxyhal_touch_tx_cancel(NULL);
        break;

    case BUTTON_EVT_RELEASED | BUTTON_RIGHT:
        io_seproxyhal_touch_tx_ok(NULL);
        break;
    default:
        break;
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
        io_seproxyhal_touch_sign_message_ok(NULL);
        break;
    default:
        break;
    }
    return 0;
}

unsigned int ui_data_selector_nanos_button(unsigned int button_mask, unsigned int button_mask_counter)
{
    switch (button_mask)
    {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT:
        io_seproxyhal_touch_data_cancel(NULL);
        break;

    case BUTTON_EVT_RELEASED | BUTTON_RIGHT:
        io_seproxyhal_touch_data_ok(NULL);
        break;
    default:
        break;
    }
    return 0;
}

unsigned int ui_data_parameter_nanos_button(unsigned int button_mask, unsigned int button_mask_counter)
{
    switch (button_mask)
    {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT:
        io_seproxyhal_touch_data_cancel(NULL);
        break;

    case BUTTON_EVT_RELEASED | BUTTON_RIGHT:
        io_seproxyhal_touch_data_ok(NULL);
        break;
    default:
        break;
    }
    return 0;
}

#endif // #if defined(TARGET_NANOS)

unsigned short io_exchange_al(unsigned char channel, unsigned short tx_len)
{
    switch (channel & (uint8_t)~IO_FLAGS)
    {
    case CHANNEL_KEYBOARD:
        break;

        // multiplexed io exchange over an SPI channel and TLV encapsulated protocol
    case CHANNEL_SPI:
        if (tx_len != 0)
        {
            io_seproxyhal_spi_send(G_io_apdu_buffer, tx_len);

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

void handleApdu(uint8_t *flags, uint16_t *txLength)
{
    uint16_t sw;

    BEGIN_TRY
    {
        TRY
        {
            if (G_io_apdu_buffer[OFFSET_CLA] != CLA)
            {
                THROW(SW_UNKNOWN_CLASS);
            }

            switch (G_io_apdu_buffer[OFFSET_INS])
            {
            case INS_GET_PUBLIC_KEY:
                handleGetPublicKey(G_io_apdu_buffer[OFFSET_P1], G_io_apdu_buffer[OFFSET_P2], G_io_apdu_buffer + OFFSET_CDATA,
                                   G_io_apdu_buffer[OFFSET_LC], flags, txLength);
                break;

            case INS_SIGN_MESSAGE:
                handleSignMessage(G_io_apdu_buffer[OFFSET_P1], G_io_apdu_buffer[OFFSET_P2], G_io_apdu_buffer + OFFSET_CDATA,
                                  G_io_apdu_buffer[OFFSET_LC], flags, txLength);
                break;

            default:
                THROW(SW_UNKNOWN_INSTRUCTION);
            }
        }
        CATCH(EXCEPTION_IO_RESET)
        {
            THROW(EXCEPTION_IO_RESET);
        }
        CATCH_OTHER(e)
        {
            //  PRINTF("Exception %x\n", e)
            switch (e & 0xF000)
            {
            case 0x6000:
                // Wipe the transaction context and report the exception
                sw = e;
                resetAppContext();
                break;
            case 0x9000:
                // All is well
                sw = e;
                break;
            default:
                // Internal error
                sw = 0x6800 | (e & 0x7FF);
                resetAppContext();
                break;
            }
            // Unexpected exception => report
            setStatusWordToApduBuffer(sw, txLength);
        }
        FINALLY
        {
        }
    }
    END_TRY
}

void cotiMain(void)
{
    uint16_t rxLength;
    uint16_t txLength = 0;
    uint8_t flags = 0;

    // DESIGN NOTE: the bootloader ignores the way APDU are fetched. The only
    // goal is to retrieve APDU.
    // When APDU are to be fetched from multiple IOs, like NFC+USB+BLE, make
    // sure the io_event is called with a
    // switch event, before the apdu is replied to the bootloader. This avoid
    // APDU injection faults.
    for (;;)
    {
        uint16_t sw;

        BEGIN_TRY
        {
            TRY
            {
                rxLength = txLength;
                // ensure no race in catch_other if io_exchange throw an error
                txLength = 0;
                rxLength = io_exchange(CHANNEL_APDU | flags, rxLength);
                flags = 0;

                // no apdu received, well, reset the session, and reset the
                // bootloader configuration
                if (0 == rxLength)
                {
                    THROW(SW_SECURITY_STATUS_NOT_SATISFIED);
                }

                if (os_global_pin_is_validated() != BOLOS_TRUE)
                {
                    THROW(SW_DEVICE_LOCKED);
                }

                PRINTF("New APDU received:\n%.*H\n", rxLength, G_io_apdu_buffer);

                handleApdu(&flags, &txLength);
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
                    resetAppContext();
                    break;
                case 0x9000:
                    // All is well
                    sw = e;
                    break;
                default:
                    // Internal error
                    sw = 0x6800 | (e & 0x7FF);
                    resetAppContext();
                    break;
                }
                if (e != SW_OK)
                {
                    flags &= (uint8_t)~IO_ASYNCH_REPLY;
                }
                // Unexpected exception => report
                setStatusWordToApduBuffer(sw, &txLength);
            }
            FINALLY
            {
            }
        }
        END_TRY
    }
}

void io_seproxyhal_display(const bagl_element_t *element)
{
    io_seproxyhal_display_default(element);
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
        UX_BUTTON_PUSH_EVENT(G_io_seproxyhal_spi_buffer)
        break;

    case SEPROXYHAL_TAG_STATUS_EVENT:
        if (G_io_apdu_media == IO_APDU_MEDIA_USB_HID && !(U4BE(G_io_seproxyhal_spi_buffer, 3) & SEPROXYHAL_TAG_STATUS_EVENT_FLAG_USB_POWERED))
        {
            THROW(EXCEPTION_IO_RESET);
        }
        // no break is intentional
    case SEPROXYHAL_TAG_DISPLAY_PROCESSED_EVENT:
        UX_DISPLAYED_EVENT({})
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

void appExit(void)
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
    END_TRY_L(exit)
}

void exitCriticalSection(void)
{
    __asm volatile("cpsie i");
}

__attribute__((section(".boot"))) int main(int argc, char *argv[])
{
    exitCriticalSection();

    resetAppContext();

    // ensure exception will work as planned
    os_boot();

    for (;;)
    {
        UX_INIT()

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

                uiIdle();

#ifdef HAVE_BLE
                BLE_power(0, NULL);
                BLE_power(1, "Nano X");
#endif // HAVE_BLE
                cotiMain();
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
        END_TRY
    }

    appExit();

    return 0;
}
