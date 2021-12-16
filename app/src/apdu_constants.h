#pragma once

#include "shared_context.h"

#define CLA 0xE0
#define INS_GET_PUBLIC_KEY 0x02
#define INS_SIGN_MESSAGE 0x04

#define P1_CONFIRM 0x01
#define P1_NON_CONFIRM 0x00
#define P1_FIRST 0x00
#define P1_MORE 0x80
#define P2_HASHED 0x01
#define P2_NOT_HASHED 0x00

#define OFFSET_CLA 0
#define OFFSET_INS 1
#define OFFSET_P1 2
#define OFFSET_P2 3
#define OFFSET_LC 4
#define OFFSET_CDATA 5

// status words
#define SW_SECURITY_STATUS_NOT_SATISFIED 0x6982
#define SW_DEVICE_LOCKED 0x6983
#define SW_REJECTED_BY_USER 0x6985
#define SW_INVALID_DATA 0x6A80
#define SW_INVALID_PATH 0x6A81
#define SW_INSTRUCTION_NOT_INITIATED 0x6A82
#define SW_INCORRECT_P1_P2 0x6B00
#define SW_UNKNOWN_INSTRUCTION 0x6D00
#define SW_UNKNOWN_CLASS 0x6E00
#define SW_OK 0x9000

void handleGetPublicKey(uint8_t p1, uint8_t p2, const uint8_t *dataBuffer, uint16_t dataLength, uint32_t *flags, uint32_t *tx);
void handleSignMessage(uint8_t p1, uint8_t p2, const uint8_t *workBuffer, uint16_t dataLength, uint32_t *flags, const uint32_t *tx);
