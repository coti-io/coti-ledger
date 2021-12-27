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
    uint8_t signingTypeText[MAX_SIGNING_TEXT];
    uint8_t hash[HASH_LENGTH];
    uint32_t remainingLength;
} MessageSigningContext_t;

typedef union TmpCtx
{
    PublicKeyContext_t publicKeyContext;
    MessageSigningContext_t messageSigningContext;
} TmpCtx_t;

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

typedef struct StrData
{
    // "0x" + hex public key + '\0'
    char publicKey[65 * 2 + 3];
} StrData_t;

typedef struct StrDataTmp
{
    char tmp[100];
    char tmp2[40];
} StrDataTmp_t;

typedef union Strings
{
    StrData_t common;
    StrDataTmp_t tmp;
} Strings_t;

extern TmpCtx_t tmpCtx;
extern Strings_t strings;
extern cx_sha3_t sha3;

extern uint8_t appState;

void resetAppContext(void);
