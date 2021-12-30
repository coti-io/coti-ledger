#include "shared_context.h"

uint16_t setResultGetPublicKey(void)
{
    uint16_t txLength = 0;
    uint8_t publicKeyLength = sizeof(appContext.publicKeyContext.publicKey.W);
    G_io_apdu_buffer[txLength++] = publicKeyLength;
    os_memmove(G_io_apdu_buffer + txLength, appContext.publicKeyContext.publicKey.W, publicKeyLength);
    txLength += publicKeyLength;

    return txLength;
}
