#pragma once

#include <stdint.h>

void handleGetPublicKey(uint8_t p1, uint8_t p2, const uint8_t *dataBuffer, uint16_t dataLength, uint8_t *flags, uint16_t *txLength);
void handleSignMessage(uint8_t p1, uint8_t p2, const uint8_t *workBuffer, uint16_t dataLength, uint8_t *flags, const uint16_t *txLength);
