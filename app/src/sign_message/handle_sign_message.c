#include "shared_context.h"
#include "apdu_constants.h"
#include "apdu_utils.h"
#include "utils.h"

#ifdef HAVE_UX_FLOW
#include "ui_flow.h"
#endif

#include "sign_message_constants.h"

void handleSignMessage(uint8_t p1, uint8_t p2, const uint8_t *workBuffer, uint16_t dataLength, uint8_t *flags, const uint16_t *txLength)
{
    UNUSED(txLength);
    const uint8_t *workBufferPtr = workBuffer;
    uint16_t unreadDataLength = dataLength;
    const uint32_t keccakOutputSize = 256;
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

        if (unreadDataLength < 1)
        {
            PRINTF("Invalid data\n");
            THROW(SW_INVALID_DATA);
        }

        if (appState != APP_STATE_IDLE)
        {
            resetAppContext();
        }

        appState = APP_STATE_SIGNING_MESSAGE;
        appContext.messageSigningContext.pathLength = workBufferPtr[0];
        if ((appContext.messageSigningContext.pathLength < 0x01) || (appContext.messageSigningContext.pathLength > MAX_BIP32_PATH))
        {
            PRINTF("Invalid path\n");
            THROW(SW_INVALID_PATH);
        }

        workBufferPtr += PATH_LENGTH_BYTES;
        unreadDataLength -= PATH_LENGTH_BYTES;

        if (unreadDataLength < ((PATH_PARAMETER_BYTES * appContext.messageSigningContext.pathLength) + SIGNATURE_TYPE_BYTES + REMAINING_LENGTH_BYTES))
        {
            PRINTF("Invalid data\n");
            THROW(SW_INVALID_DATA);
        }

        for (uint32_t i = 0; i < appContext.messageSigningContext.pathLength; ++i)
        {
            appContext.messageSigningContext.bip32Path[i] = U4BE(workBufferPtr, 0);
            workBufferPtr += PATH_PARAMETER_BYTES;
            unreadDataLength -= PATH_PARAMETER_BYTES;
        }

        uint8_t signingType = workBufferPtr[0];
        workBufferPtr += SIGNATURE_TYPE_BYTES;
        unreadDataLength -= SIGNATURE_TYPE_BYTES;

        if (signingType >= MAX_SIGNING_TYPE)
        {
            PRINTF("Invalid data\n");
            THROW(SW_INVALID_DATA);
        }

        os_memmove(appContext.messageSigningContext.signingTypeText, signing_type_texts[signingType],
                   sizeof(appContext.messageSigningContext.signingTypeText));

        appContext.messageSigningContext.remainingLength = U4BE(workBufferPtr, 0);
        workBufferPtr += REMAINING_LENGTH_BYTES;
        unreadDataLength -= REMAINING_LENGTH_BYTES;
        if (P2_NOT_HASHED == p2)
        {

            cx_keccak_init(&sha3, keccakOutputSize);
        }
        else if (appContext.messageSigningContext.remainingLength != HASH_LENGTH)
        {
            PRINTF("Invalid data\n");
            THROW(SW_INVALID_DATA);
        }
    }

    if ((P1_MORE == p1) && (appState != APP_STATE_SIGNING_MESSAGE))
    {
        PRINTF("Signature not initialized\n");
        THROW(SW_INSTRUCTION_NOT_INITIATED);
    }

    if (unreadDataLength > appContext.messageSigningContext.remainingLength)
    {
        THROW(SW_INVALID_DATA);
    }

    if (P2_NOT_HASHED == p2)
    {
        cx_hash((cx_hash_t *)&sha3, 0, workBufferPtr, unreadDataLength, NULL, 0);
    }

    appContext.messageSigningContext.remainingLength -= unreadDataLength;
    if (0 == appContext.messageSigningContext.remainingLength)
    {
        if (P2_NOT_HASHED == p2)
        {
            cx_hash((cx_hash_t *)&sha3, CX_LAST, workBufferPtr, 0, appContext.messageSigningContext.hash,
                    sizeof(appContext.messageSigningContext.hash));
        }
        else
        {
            os_memmove(appContext.messageSigningContext.hash, workBufferPtr, sizeof(appContext.messageSigningContext.hash));
        }

        arrayHexstr(strings.tmp.tmp, &appContext.messageSigningContext.hash, sizeof(appContext.messageSigningContext.hash));

        ux_flow_init(0, ux_sign_flow, NULL);

        *flags |= IO_ASYNCH_REPLY;
    }
    else
    {
        THROW(SW_OK);
    }
}
