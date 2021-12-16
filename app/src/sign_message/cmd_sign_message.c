#include "shared_context.h"
#include "apdu_constants.h"
#include "utils.h"

#ifdef HAVE_UX_FLOW
#include "ui_flow.h"
#endif

#include "sign_message_constants.h"

void handleSignMessage(uint8_t p1, uint8_t p2, const uint8_t *workBuffer, uint16_t dataLength, uint32_t *flags, const uint32_t *tx)
{
    UNUSED(tx);
    const uint8_t *workBufferPtr = workBuffer;
    if ((p1 != P1_FIRST) && (p1 != P1_MORE))
    {
        PRINTF("Incorrect p1\n");
        THROW(SW_INCORRECT_P1_P2);
    }

    if ((p2 != P2_HASHED) && (p2 != P2_NOT_HASHED))
    {
        PRINTF("Incorrect p2\n");
        THROW(SW_INCORRECT_P1_P2);
    }

    if (P1_FIRST == p1)
    {

        if (dataLength < 1)
        {
            PRINTF("Invalid data\n");
            THROW(SW_INVALID_DATA);
        }

        if (app_state != APP_STATE_IDLE)
        {
            reset_app_context();
        }

        app_state = APP_STATE_SIGNING_MESSAGE;
        tmp_ctx.message_signing_context.path_length = workBuffer[0];
        if ((tmp_ctx.message_signing_context.path_length < 0x01) || (tmp_ctx.message_signing_context.path_length > MAX_BIP32_PATH))
        {
            PRINTF("Invalid path\n");
            THROW(SW_INVALID_PATH);
        }

        ++workBufferPtr;
        dataLength--;

        if (dataLength < ((4 * tmp_ctx.message_signing_context.path_length) + 5))
        {
            PRINTF("Invalid data\n");
            THROW(SW_INVALID_DATA);
        }

        for (uint32_t i = 0; i < tmp_ctx.message_signing_context.path_length; ++i)
        {
            tmp_ctx.message_signing_context.bip32_path[i] = U4BE(workBufferPtr, 0);
            workBufferPtr += 4;
            dataLength -= 4;
        }

        uint8_t signingType = workBufferPtr[0];
        ++workBufferPtr;
        dataLength--;

        if (signingType >= MAX_SIGNING_TYPE)
        {
            PRINTF("Invalid data\n");
            THROW(SW_INVALID_DATA);
        }

        strcpy(tmp_ctx.message_signing_context.signing_type_text, signing_type_texts[signingType]);

        tmp_ctx.message_signing_context.remaining_length = U4BE(workBufferPtr, 0);
        workBufferPtr += 4;
        dataLength -= 4;
        if (P2_NOT_HASHED == p2)
        {

            cx_keccak_init(&sha3, 256);
        }
        else if (tmp_ctx.message_signing_context.remaining_length != HASH_LENGTH)
        {
            PRINTF("Invalid data\n");
            THROW(SW_INVALID_DATA);
        }
    }

    if ((P1_MORE == p1) && (app_state != APP_STATE_SIGNING_MESSAGE))
    {
        PRINTF("Signature not initialized\n");
        THROW(SW_INSTRUCTION_NOT_INITIATED);
    }

    if (dataLength > tmp_ctx.message_signing_context.remaining_length)
    {
        THROW(SW_INVALID_DATA);
    }

    if (P2_NOT_HASHED == p2)
    {
        cx_hash((cx_hash_t *)&sha3, 0, workBufferPtr, dataLength, NULL, 0);
    }

    tmp_ctx.message_signing_context.remaining_length -= dataLength;
    if (0 == tmp_ctx.message_signing_context.remaining_length)
    {
        if (P2_NOT_HASHED == p2)
        {
            cx_hash((cx_hash_t *)&sha3, CX_LAST, workBufferPtr, 0, tmp_ctx.message_signing_context.hash,
                    sizeof(tmp_ctx.message_signing_context.hash));
        }
        else
        {
            memcpy(tmp_ctx.message_signing_context.hash, workBufferPtr, sizeof(tmp_ctx.message_signing_context.hash));
        }

        array_hexstr(strings.tmp.tmp, &tmp_ctx.message_signing_context.hash, sizeof(tmp_ctx.message_signing_context.hash));

        ux_flow_init(0, ux_sign_flow, NULL);

        *flags |= IO_ASYNCH_REPLY;
    }
    else
    {
        THROW(SW_OK);
    }
}
