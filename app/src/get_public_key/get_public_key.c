#include "shared_context.h"

uint32_t setResultGetPublicKey()
{
    uint32_t tx = 0;
    uint16_t publicKeyLength = sizeof(tmpCtx.publicKeyContext.publicKey.W);
    G_io_apdu_buffer[tx++] = publicKeyLength;
    os_memmove(G_io_apdu_buffer + tx, tmpCtx.publicKeyContext.publicKey.W, publicKeyLength);
    tx += publicKeyLength;

    return tx;
}
