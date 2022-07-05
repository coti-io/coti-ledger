#pragma once

#include <stdint.h>

void resetAppContext(void);
void resetAppDisplayData(void);
void setStatusWordToApduBuffer(uint16_t statusWord, uint16_t *txLength);
void setSignatureToApduBuffer(const uint8_t *signature);
