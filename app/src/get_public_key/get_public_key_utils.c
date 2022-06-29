#include "shared_context.h"

void setPublicKey(uint8_t *privateKeyData, uint32_t privateKeyDataLength, const uint32_t *bip32Path, uint8_t bip32PathLength,
                  cx_ecfp_private_key_t *privateKey)
{
    io_seproxyhal_io_heartbeat();
    os_perso_derive_node_bip32(CX_CURVE_256K1, bip32Path, bip32PathLength, privateKeyData, NULL);
    cx_ecfp_init_private_key(CX_CURVE_256K1, privateKeyData, privateKeyDataLength, privateKey);
    io_seproxyhal_io_heartbeat();
    cx_ecfp_generate_pair(CX_CURVE_256K1, &appContext.publicKeyContext.publicKey, privateKey, 1);
    memset(privateKey, 0, sizeof(*privateKey));
    memset(privateKeyData, 0, privateKeyDataLength);
}

uint16_t setPublicKeyToApduBuffer(void)
{
    uint16_t txLength = 0;
    uint8_t publicKeyLength = sizeof(appContext.publicKeyContext.publicKey.W);
    G_io_apdu_buffer[txLength++] = publicKeyLength;
    memmove(G_io_apdu_buffer + txLength, appContext.publicKeyContext.publicKey.W, publicKeyLength);
    txLength += publicKeyLength;

    return txLength;
}

void setPublicKeyDisplayData(void)
{ // prepare for a UI based reply
    snprintf(displayData.publicKeyDisplayData.publicKey, sizeof(displayData.publicKeyDisplayData.publicKey), "0x");
    const uint16_t publicKeyStartIndex = 2;
    arrayHexstr(&displayData.publicKeyDisplayData.publicKey[publicKeyStartIndex], &appContext.publicKeyContext.publicKey.W,
                sizeof(appContext.publicKeyContext.publicKey.W));
}
