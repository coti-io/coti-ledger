#include "shared_context.h"
#include "ui_callbacks.h"
#include "apdu_constants.h"

unsigned int io_seproxyhal_touch_sign_message_ok(const bagl_element_t *e)
{
    uint8_t private_key_data[32];
    uint8_t signature[100];
    uint8_t signature_length;
    cx_ecfp_private_key_t private_key;
    uint32_t tx = 0;
    io_seproxyhal_io_heartbeat();
    os_perso_derive_node_bip32(CX_CURVE_256K1, tmp_ctx.message_signing_context.bip32_path, tmp_ctx.message_signing_context.path_length,
                               private_key_data, NULL);
    io_seproxyhal_io_heartbeat();
    cx_ecfp_init_private_key(CX_CURVE_256K1, private_key_data, 32, &private_key);
    os_memset(private_key_data, 0, sizeof(private_key_data));
    unsigned int info = 0;
    io_seproxyhal_io_heartbeat();
    signature_length = cx_ecdsa_sign(&private_key, CX_RND_RFC6979 | CX_LAST, CX_SHA256, tmp_ctx.message_signing_context.hash,
                                     sizeof(tmp_ctx.message_signing_context.hash), signature, sizeof(signature), &info);
    os_memset(&private_key, 0, sizeof(private_key));
    G_io_apdu_buffer[0] = 27;
    if (info & CX_ECCINFO_PARITY_ODD)
    {
        G_io_apdu_buffer[0]++;
    }
    if (info & CX_ECCINFO_xGTn)
    {
        G_io_apdu_buffer[0] += 2;
    }
    format_signature_out(signature);
    tx = 65;
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

unsigned int io_seproxyhal_touch_sign_message_cancel(const bagl_element_t *e)
{
    reset_app_context();
    unsigned short sw = SW_REJECTED_BY_USER;
    G_io_apdu_buffer[0] = sw >> 8;
    G_io_apdu_buffer[1] = sw & 0xFF;
    // Send back the response, do not restart the event loop
    io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, 2);
    // Display back the original UX
    ui_idle();
    return 0; // do not redraw the widget
}
