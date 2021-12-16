#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <os.h>
#include <cx.h>
#include <os_io_seproxyhal.h>

#include "utils.h"

#define MAX_BIP32_PATH 10
#define MAX_SIGNING_TEXT 15
#define HASH_LENGTH 32

typedef struct public_key_context_s
{
    cx_ecfp_public_key_t public_key;
} public_key_context_t;

typedef struct message_signing_context_s
{
    uint8_t path_length;
    uint32_t bip32_path[MAX_BIP32_PATH];
    char signing_type_text[MAX_SIGNING_TEXT];
    uint8_t hash[HASH_LENGTH];
    uint32_t remaining_length;
} message_signing_context_t;

typedef union tmp_ctx_u
{
    public_key_context_t public_key_context;
    message_signing_context_t message_signing_context;
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
    char public_key[65 * 2 + 3]; // "0x" + hex public key + '\0'
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

extern tmp_ctx_t tmp_ctx;
extern strings_t strings;
extern cx_sha3_t sha3;

extern uint8_t app_state;

void reset_app_context(void);
