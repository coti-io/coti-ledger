#include <stdint.h>
#include <os.h>

void setStatusWordToApduBuffer(uint16_t statusWord, uint16_t *txLength)
{
    G_io_apdu_buffer[(*txLength)++] = statusWord >> 8;
    G_io_apdu_buffer[(*txLength)++] = statusWord & 0xFF;
}
