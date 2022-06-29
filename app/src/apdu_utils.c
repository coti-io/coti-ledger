#include <stdint.h>
#include <os.h>
#include "shared_context.h"

void resetAppDisplayData(void)
{
    memset((uint8_t *)&displayData, 0, sizeof(displayData));
}

void resetAppContext(void)
{
    PRINTF("!!RESET_APP_CONTEXT\n");
    appState = APP_STATE_IDLE;
    memset((uint8_t *)&appContext, 0, sizeof(appContext));
    resetAppDisplayData();
}

void setStatusWordToApduBuffer(uint16_t statusWord, uint16_t *txLength)
{
    G_io_apdu_buffer[(*txLength)++] = statusWord >> 8;
    G_io_apdu_buffer[(*txLength)++] = statusWord & 0xFF;
}

void setSignatureToApduBuffer(const uint8_t *signature)
{
    const uint32_t signatureLength = 64;
    memset(G_io_apdu_buffer + 1, 0, signatureLength);
    uint8_t offset = 1;
    // point to r value
    uint8_t xOffset = 4;
    // copy r
    uint8_t xLength = signature[xOffset - 1];
    const uint32_t halfSignatureLength = signatureLength / 2;
    if ((1 + halfSignatureLength) == xLength)
    {
        xLength = halfSignatureLength;
        ++xOffset;
    }
    memmove(G_io_apdu_buffer + offset + halfSignatureLength - xLength, signature + xOffset, xLength);
    offset += halfSignatureLength;
    // move over rvalue and TagLEn
    xOffset += xLength + 2;
    // copy s value
    xLength = signature[xOffset - 1];
    if ((1 + halfSignatureLength) == xLength)
    {
        xLength = halfSignatureLength;
        ++xOffset;
    }
    memmove(G_io_apdu_buffer + offset + halfSignatureLength - xLength, signature + xOffset, xLength);
}
