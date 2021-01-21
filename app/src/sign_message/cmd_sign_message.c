#include "shared_context.h"
#include "apdu_constants.h"
#include "utils.h"

#ifdef HAVE_UX_FLOW
#include "ui_flow.h"
#endif

#include "sign_message_constants.h"

void handle_sign_message(uint8_t p1, uint8_t p2, uint8_t *work_buffer, uint16_t data_length, unsigned int *flags, unsigned int *tx)
{
    UNUSED(tx);

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

    uint32_t i;

    if (p1 == P1_FIRST)
    {

        if (data_length < 1)
        {
            PRINTF("Invalid data\n");
            THROW(SW_INVALID_DATA);
        }

        if (app_state != APP_STATE_IDLE)
        {
            reset_app_context();
        }

        app_state = APP_STATE_SIGNING_MESSAGE;
        tmp_ctx.message_signing_context.path_length = work_buffer[0];
        if ((tmp_ctx.message_signing_context.path_length < 0x01) || (tmp_ctx.message_signing_context.path_length > MAX_BIP32_PATH))
        {
            PRINTF("Invalid path\n");
            THROW(SW_INVALID_PATH);
        }

        work_buffer++;
        data_length--;

        if (data_length < 4 * tmp_ctx.message_signing_context.path_length + 5)
        {
            PRINTF("Invalid data\n");
            THROW(SW_INVALID_DATA);
        }

        for (i = 0; i < tmp_ctx.message_signing_context.path_length; i++)
        {
            tmp_ctx.message_signing_context.bip32_path[i] = U4BE(work_buffer, 0);
            work_buffer += 4;
            data_length -= 4;
        }

        uint8_t signing_type = work_buffer[0];
        work_buffer++;
        data_length--;

        if (signing_type >= MAX_SIGNING_TYPE)
        {
            PRINTF("Invalid data\n");
            THROW(SW_INVALID_DATA);
        }

        strcpy(tmp_ctx.message_signing_context.signing_type_text, signing_type_texts[signing_type]);

        tmp_ctx.message_signing_context.remaining_length = U4BE(work_buffer, 0);
        work_buffer += 4;
        data_length -= 4;
        if (p2 == P2_NOT_HASHED)
        {
            cx_keccak_init(&sha3, 256);
        }
        else if (tmp_ctx.message_signing_context.remaining_length != HASH_LENGTH)
        {
            PRINTF("Invalid data\n");
            THROW(SW_INVALID_DATA);
        }
    }

    if ((p1 == P1_MORE) && (app_state != APP_STATE_SIGNING_MESSAGE))
    {
        PRINTF("Signature not initialized\n");
        THROW(SW_INSTRUCTION_NOT_INITIATED);
    }

    if (data_length > tmp_ctx.message_signing_context.remaining_length)
    {
        THROW(SW_INVALID_DATA);
    }

    if (p2 == P2_NOT_HASHED)
    {
        cx_hash((cx_hash_t *)&sha3, 0, work_buffer, data_length, NULL, 0);
    }

    tmp_ctx.message_signing_context.remaining_length -= data_length;
    if (tmp_ctx.message_signing_context.remaining_length == 0)
    {
        if (p2 == P2_NOT_HASHED)
        {
            cx_hash((cx_hash_t *)&sha3, CX_LAST, work_buffer, 0, tmp_ctx.message_signing_context.hash, sizeof(tmp_ctx.message_signing_context.hash));
        }
        else
        {
            memcpy(tmp_ctx.message_signing_context.hash, work_buffer, sizeof(tmp_ctx.message_signing_context.hash));
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
