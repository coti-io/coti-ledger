#include "shared_context.h"
#include "apdu_constants.h"

#ifdef HAVE_UX_FLOW
#include "ui_flow.h"
#endif

#include "get_public_key.h"

void handleGetPublicKey(uint8_t p1, uint8_t p2, const uint8_t *dataBuffer, uint16_t dataLength, uint32_t *flags, uint16_t *txLength)
{
    UNUSED(dataLength);
    const uint8_t *dataBufferPtr = dataBuffer;
    uint8_t privateKeyData[32];
    uint32_t bip32Path[MAX_BIP32_PATH];
    uint8_t bip32PathLength = *(dataBufferPtr++);
    cx_ecfp_private_key_t privateKey;

    resetAppContext();

    if ((bip32PathLength < 0x01) || (bip32PathLength > MAX_BIP32_PATH))
    {
        PRINTF("Invalid path\n");
        THROW(SW_INVALID_PATH);
    }

    if ((p1 != P1_CONFIRM) && (p1 != P1_NON_CONFIRM))
    {
        PRINTF("Incorrect p1\n");
        THROW(SW_INCORRECT_P1_P2);
    }

    if (p2 != 0)
    {
        PRINTF("Incorrect p2\n");
        THROW(SW_INCORRECT_P1_P2);
    }

    for (uint32_t i = 0; i < bip32PathLength; ++i)
    {
        bip32Path[i] = U4BE(dataBufferPtr, 0);
        dataBufferPtr += 4;
    }

    io_seproxyhal_io_heartbeat();
    os_perso_derive_node_bip32(CX_CURVE_256K1, bip32Path, bip32PathLength, privateKeyData, NULL);
    cx_ecfp_init_private_key(CX_CURVE_256K1, privateKeyData, sizeof(privateKeyData), &privateKey);
    io_seproxyhal_io_heartbeat();
    cx_ecfp_generate_pair(CX_CURVE_256K1, &tmpCtx.publicKeyContext.publicKey, &privateKey, 1);
    os_memset(&privateKey, 0, sizeof(privateKey));
    os_memset(privateKeyData, 0, sizeof(privateKeyData));

    if (P1_NON_CONFIRM == p1)
    {
        *txLength = setResultGetPublicKey();
        THROW(SW_OK);
    }
    else
    {
        // prepare for a UI based reply
        snprintf(strings.common.publicKey, sizeof(strings.common.publicKey), "0x");
        const uint16_t publicKeyStartIndex = 2;
        arrayHexstr(&strings.common.publicKey[publicKeyStartIndex], &tmpCtx.publicKeyContext.publicKey.W,
                    sizeof(tmpCtx.publicKeyContext.publicKey.W));

        ux_flow_init(0, ux_display_public_flow, NULL);

        *flags |= IO_ASYNCH_REPLY;
    }
}
