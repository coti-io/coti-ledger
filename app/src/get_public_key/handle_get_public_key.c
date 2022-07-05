#include "shared_context.h"
#include "apdu_constants.h"

#ifdef HAVE_UX_FLOW
#include "ui_flow.h"
#endif

#include "get_public_key_utils.h"
#include "apdu_utils.h"

void handleGetPublicKey(uint8_t p1, uint8_t p2, const uint8_t *dataBuffer, uint16_t dataLength, uint8_t *flags, uint16_t *txLength)
{
    UNUSED(dataLength);
    const uint8_t *dataBufferPtr = dataBuffer;
    uint8_t privateKeyData[32];
    uint32_t bip32Path[MAX_BIP32_PATH];
    uint8_t bip32PathLength = *dataBufferPtr;
    cx_ecfp_private_key_t privateKey;

    dataBufferPtr += PATH_LENGTH_BYTES;

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
        dataBufferPtr += PATH_PARAMETER_BYTES;
    }

    setPublicKey(privateKeyData, sizeof(privateKeyData), bip32Path, bip32PathLength, &privateKey);

    if (P1_NON_CONFIRM == p1)
    {
        *txLength = setPublicKeyToApduBuffer();
        resetAppContext();
        THROW(SW_OK);
    }
    else
    {
        setPublicKeyDisplayData();

        ux_flow_init(0, uxDisplayPublicFlow, NULL);

        *flags |= IO_ASYNCH_REPLY;
    }
}
