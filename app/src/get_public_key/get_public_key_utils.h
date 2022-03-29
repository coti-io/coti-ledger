#pragma once

#include "shared_context.h"

void setPublicKey(uint8_t *privateKeyData, uint32_t privateKeyDataLength, const uint32_t *bip32Path, uint8_t bip32PathLength,
                  cx_ecfp_private_key_t *privateKey);
uint16_t setPublicKeyToApduBuffer(void);
void setPublicKeyDisplayData(void);
