#include "shared_context.h"
#include "apdu_constants.h"
#include "utils.h"

#ifdef HAVE_UX_FLOW
#include "ui_flow.h"
#endif

void handle_sign_message(uint8_t p1, uint8_t p2, uint8_t *work_buffer, uint16_t data_length, unsigned int *flags, unsigned int *tx)
{
    UNUSED(tx);
    if (p1 == P1_FIRST)
    {
        uint32_t i;

        if (data_length < 1)
        {
            PRINTF("Invalid data\n");
            THROW(0x6a80);
        }

        if (app_state != APP_STATE_IDLE)
        {
            reset_app_context();
        }

        app_state = APP_STATE_SIGNING_MESSAGE;
        tmp_ctx.message_signing_context.path_length = work_buffer[0];
        if ((tmp_ctx.message_signing_context.path_length < 0x01) ||
            (tmp_ctx.message_signing_context.path_length > MAX_BIP32_PATH))
        {
            PRINTF("Invalid path\n");
            THROW(0x6a80);
        }

        work_buffer++;
        data_length--;
        for (i = 0; i < tmp_ctx.message_signing_context.path_length; i++)
        {
            if (data_length < 4)
            {
                PRINTF("Invalid data\n");
                THROW(0x6a80);
            }

            tmp_ctx.message_signing_context.bip32_path[i] = U4BE(work_buffer, 0);
            work_buffer += 4;
            data_length -= 4;
        }

        if (data_length < 4)
        {
            PRINTF("Invalid data\n");
            THROW(0x6a80);
        }

        tmp_ctx.message_signing_context.remaining_length = U4BE(work_buffer, 0);
        work_buffer += 4;
        data_length -= 4;

        cx_keccak_init(&sha3, 256);
    }
    else if (p1 != P1_MORE)
    {
        THROW(0x6B00);
    }

    if (p2 != 0)
    {
        THROW(0x6B00);
    }

    if ((p1 == P1_MORE) && (app_state != APP_STATE_SIGNING_MESSAGE))
    {
        PRINTF("Signature not initialized\n");
        THROW(0x6985);
    }

    if (data_length > tmp_ctx.message_signing_context.remaining_length)
    {
        THROW(0x6A80);
    }

    cx_hash((cx_hash_t *)&sha3, 0, work_buffer, data_length, NULL, 0);
    tmp_ctx.message_signing_context.remaining_length -= data_length;
    if (tmp_ctx.message_signing_context.remaining_length == 0)
    {
        cx_hash((cx_hash_t *)&sha3, CX_LAST, work_buffer, 0, tmp_ctx.message_signing_context.hash,
                sizeof(tmp_ctx.message_signing_context.hash));

        array_hexstr(strings.tmp.tmp, &tmp_ctx.message_signing_context.hash, sizeof(tmp_ctx.message_signing_context.hash));

        ux_flow_init(0, ux_sign_flow, NULL);

        *flags |= IO_ASYNCH_REPLY;
    }
    else
    {
        THROW(0x9000);
    }
}
