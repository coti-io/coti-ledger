#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <os.h>
#include <cx.h>
#include <os_io_seproxyhal.h>

#include "utils.h"
#include "shared_constants.h"

typedef struct PublicKeyContext
{
    cx_ecfp_public_key_t publicKey;
} PublicKeyContext_t;

typedef struct MessageSigningContext
{
    uint8_t pathLength;
    uint32_t bip32Path[MAX_BIP32_PATH];
    uint8_t signingType;
    uint8_t hash[HASH_LENGTH];
    uint32_t signMessageLength;
    uint32_t processedMessageLength;
    uint8_t amount[MAX_AMOUNT_LENGTH];
    uint32_t amountLength;
    uint8_t address[ADDRESS_LENGTH];
} MessageSigningContext_t;

typedef union AppContext
{
    PublicKeyContext_t publicKeyContext;
    MessageSigningContext_t messageSigningContext;
} AppContext_t;

typedef enum SigningType
{
    MESSAGE,
    FULL_NODE_FEE,
    TX_TRUST_SCORE,
    BASE_TX,
    TX,
    MAX_SIGNING_TYPE
} SigningType_t;

typedef enum AppState
{
    APP_STATE_IDLE,
    APP_STATE_SIGNING_MESSAGE
} AppState_t;

typedef struct PublicKeyDisplayData
{
    // "0x" + hex public key + '\0'
    char publicKey[65 * 2 + 3];
} PublicKeyDisplayData_t;

typedef struct SignMessageDisplayData
{
    char message[2 * HASH_LENGTH + 1];
    char signingTypeText[MAX_SIGNING_TEXT + 1];
    char amount[MAX_AMOUNT_LENGTH + COTI_STRING_LENGTH + 1];
    char address[ADDRESS_LENGTH + 1];
} SignMessageDisplayData_t;

typedef union DisplayData
{
    PublicKeyDisplayData_t publicKeyDisplayData;
    SignMessageDisplayData_t signMessageDisplayData;
} DisplayData_t;

extern AppContext_t appContext;
extern DisplayData_t displayData;
extern cx_sha3_t sha3;

extern uint8_t appState;
