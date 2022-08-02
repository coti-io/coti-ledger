/*******************************************************************************
 *   Ledger App Coti
 *   (c) 2017 Ledger
 *   (c) 2022 Tchain Ltd. adaptation for COTI
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

#ifdef HAVE_UX_FLOW
#include "ui_flow.h"
#include "ux.h"
#endif

#include "glyphs.h"
#include "utils.h"

uint8_t G_io_seproxyhal_spi_buffer[IO_SEPROXYHAL_BUFFER_SIZE_B];

AppContext_t appContext;
DisplayData_t displayData;

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
    END_TRY;
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
        END_TRY;
    }
}

void appExit(void)
{

    BEGIN_TRY_L(exit)
    {
        TRY_L(exit)
        {
            os_sched_exit((bolos_task_status_t)-1);
        }
        FINALLY_L(exit)
        {
        }
    }
    END_TRY_L(exit);
}

void exitCriticalSection(void)
{
    __asm volatile("cpsie i");
}

__attribute__((section(".boot"))) int main(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);
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

                uiMenuMain();

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
        END_TRY;
    }

    appExit();

    return 0;
}
