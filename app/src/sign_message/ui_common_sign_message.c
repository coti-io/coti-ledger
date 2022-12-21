#include "shared_context.h"
#include "ui_callbacks.h"
#include "apdu_constants.h"
#include "apdu_utils.h"
#include "ui_flow.h"

uint32_t io_seproxyhal_touch_sign_message_ok(const bagl_element_t *e)
{
    UNUSED(e);
    uint8_t privateKeyData[32];
    uint8_t signature[100];
    cx_ecfp_private_key_t privateKey;
    uint16_t txLength;
    io_seproxyhal_io_heartbeat();
    os_perso_derive_node_bip32(CX_CURVE_256K1, appContext.messageSigningContext.bip32Path, appContext.messageSigningContext.pathLength,
                               privateKeyData, NULL);
    io_seproxyhal_io_heartbeat();
    cx_ecfp_init_private_key(CX_CURVE_256K1, privateKeyData, sizeof(privateKeyData), &privateKey);
    memset(privateKeyData, 0, sizeof(privateKeyData));
    uint32_t info = 0;
    io_seproxyhal_io_heartbeat();
    cx_ecdsa_sign(&privateKey, CX_RND_RFC6979 | CX_LAST, CX_SHA256, appContext.messageSigningContext.hash,
                  sizeof(appContext.messageSigningContext.hash), signature, sizeof(signature), &info);
    memset(&privateKey, 0, sizeof(privateKey));
    G_io_apdu_buffer[0] = 27;
    if ((info & CX_ECCINFO_PARITY_ODD) != 0)
    {
        ++G_io_apdu_buffer[0];
    }
    if ((info & CX_ECCINFO_xGTn) != 0)
    {
        G_io_apdu_buffer[0] += 2;
    }
    setSignatureToApduBuffer(signature);
    txLength = 65;
    uint16_t sw = SW_OK;
    setStatusWordToApduBuffer(sw, &txLength);
    resetAppContext();
    // Send back the response, do not restart the event loop
    io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, txLength);
    // Display back the original UX
    uiMenuMain();
    // do not redraw the widget
    return 0;
}

uint32_t io_seproxyhal_touch_sign_message_cancel(const bagl_element_t *e)
{
    UNUSED(e);
    resetAppContext();
    uint16_t sw = SW_REJECTED_BY_USER;
    uint16_t txLength = 0;
    setStatusWordToApduBuffer(sw, &txLength);
    // Send back the response, do not restart the event loop
    io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, txLength);
    // Display back the original UX
    uiMenuMain();
    // do not redraw the widget
    return 0;
}
