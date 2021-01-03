#include "shared_context.h"
#include "apdu_constants.h"

#ifdef HAVE_UX_FLOW
#include "ui_flow.h"
#endif

#include "get_public_key.h"

void handle_get_public_key(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t data_length, unsigned int *flags, unsigned int *tx)
{
    UNUSED(data_length);
    uint8_t private_key_data[32];
    uint32_t bip32_path[MAX_BIP32_PATH];
    uint32_t i;
    uint8_t bip32_path_length = *(dataBuffer++);
    cx_ecfp_private_key_t private_key;

    reset_app_context();

    if ((bip32_path_length < 0x01) || (bip32_path_length > MAX_BIP32_PATH))
    {
        PRINTF("Invalid path\n");
        THROW(INVALID_DATA);
    }

    if ((p1 != P1_CONFIRM) && (p1 != P1_NON_CONFIRM))
    {
        THROW(INVALID_PARAMETER);
    }

    if (p2 != 0)
    {
        THROW(INVALID_PARAMETER);
    }

    for (i = 0; i < bip32_path_length; i++)
    {
        bip32_path[i] = U4BE(dataBuffer, 0);
        dataBuffer += 4;
    }

    io_seproxyhal_io_heartbeat();
    os_perso_derive_node_bip32(CX_CURVE_256K1, bip32_path, bip32_path_length, private_key_data, NULL);
    cx_ecfp_init_private_key(CX_CURVE_256K1, private_key_data, 32, &private_key);
    io_seproxyhal_io_heartbeat();
    cx_ecfp_generate_pair(CX_CURVE_256K1, &tmp_ctx.public_key_context.public_key, &private_key, 1);
    os_memset(&private_key, 0, sizeof(private_key));
    os_memset(private_key_data, 0, sizeof(private_key_data));
    io_seproxyhal_io_heartbeat();

    if (p1 == P1_NON_CONFIRM)
    {
        *tx = set_result_get_public_key();
        THROW(OK);
    }
    else
    {
        // prepare for a UI based reply
        snprintf(strings.common.public_key, sizeof(strings.common.public_key), "0x");
        array_hexstr(&strings.common.public_key[2], &tmp_ctx.public_key_context.public_key.W, 65);

        ux_flow_init(0, ux_display_public_flow, NULL);

        *flags |= IO_ASYNCH_REPLY;
    }
}
