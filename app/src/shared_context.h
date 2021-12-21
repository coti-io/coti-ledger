#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <os.h>
#include <cx.h>
#include <os_io_seproxyhal.h>

#include "utils.h"
#include "shared_constants.h"

typedef struct public_key_context_s
{
    cx_ecfp_public_key_t publicKey;
} public_key_context_t;

typedef struct message_signing_context_s
{
    uint8_t pathLength;
    uint32_t bip32Path[MAX_BIP32_PATH];
    char signingTypeText[MAX_SIGNING_TEXT];
    uint8_t hash[HASH_LENGTH];
    uint32_t remainingLength;
} message_signing_context_t;

typedef union tmp_ctx_u
{
    public_key_context_t publicKeyContext;
    message_signing_context_t messageSigningContext;
} tmp_ctx_t;

typedef enum signing_type_e
{
    MESSAGE,
    FULL_NODE_FEE,
    TX_TRUST_SCORE,
    BASE_TX,
    TX,
    MAX_SIGNING_TYPE
} signing_type_t;

typedef enum app_state_e
{
    APP_STATE_IDLE,
    APP_STATE_SIGNING_MESSAGE
} app_state_t;

typedef struct str_data_s
{
    // "0x" + hex public key + '\0'
    char publicKey[65 * 2 + 3];
} str_data_t;

typedef struct str_data_tmp_s
{
    char tmp[100];
    char tmp2[40];
} str_data_tmp_t;

typedef union strings_u
{
    str_data_t common;
    str_data_tmp_t tmp;
} strings_t;

extern tmp_ctx_t tmpCtx;
extern strings_t strings;
extern cx_sha3_t sha3;

extern uint8_t appState;

void resetAppContext(void);
