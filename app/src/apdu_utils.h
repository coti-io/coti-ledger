#pragma once

#include <stdint.h>

void setStatusWordToApduBuffer(uint16_t statusWord, uint16_t *txLength);
void setSignatureToApduBuffer(const uint8_t *signature);
