#include <stdint.h>
#include <string.h>
#include <os.h>

void setStatusWordToApduBuffer(uint16_t statusWord, uint16_t *txLength)
{
    G_io_apdu_buffer[(*txLength)++] = statusWord >> 8;
    G_io_apdu_buffer[(*txLength)++] = statusWord & 0xFF;
}

void setSignatureToApduBuffer(const uint8_t *signature)
{
    const uint32_t signatureLength = 64;
    os_memset(G_io_apdu_buffer + 1, 0, signatureLength);
    uint8_t offset = 1;
    // point to r value
    uint8_t xoffset = 4;
    // copy r
    uint8_t xlength = signature[xoffset - 1];
    if (33 == xlength)
    {
        xlength = 32;
        ++xoffset;
    }
    memmove(G_io_apdu_buffer + offset + 32 - xlength, signature + xoffset, xlength);
    offset += 32;
    // move over rvalue and TagLEn
    xoffset += xlength + 2;
    // copy s value
    xlength = signature[xoffset - 1];
    if (33 == xlength)
    {
        xlength = 32;
        ++xoffset;
    }
    memmove(G_io_apdu_buffer + offset + 32 - xlength, signature + xoffset, xlength);
}
