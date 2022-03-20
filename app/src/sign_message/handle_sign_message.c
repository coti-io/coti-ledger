#include "shared_context.h"
#include "apdu_constants.h"
#include "apdu_utils.h"
#include "utils.h"

#include "sign_message_utils.h"

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

        setPathLength(&workBufferPtr, &unreadDataLength);

        if (unreadDataLength < ((PATH_PARAMETER_BYTES * appContext.messageSigningContext.pathLength) + SIGNATURE_TYPE_BYTES + PARAMETER_LENGTH_BYTES))
        {
            PRINTF("Invalid data\n");
            THROW(SW_INVALID_DATA);
        }

        setBip32Path(&workBufferPtr, &unreadDataLength);

        setSigningType(&workBufferPtr, &unreadDataLength);

        if (appContext.messageSigningContext.signingType >= MAX_SIGNING_TYPE)
        {
            PRINTF("Invalid data\n");
            THROW(SW_INVALID_DATA);
        }

        setMessageLength(&workBufferPtr, &unreadDataLength);
        if (P2_NOT_HASHED == p2)
        {
            cx_keccak_init(&sha3, keccakOutputSize);
        }
        else if (appContext.messageSigningContext.signMessageLength != HASH_LENGTH)
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

    //    if (unreadDataLength > appContext.messageSigningContext.signMessageLength)
    //    {
    //        THROW(SW_INVALID_DATA);
    //    }
    uint32_t remainingMessageLength = appContext.messageSigningContext.signMessageLength - appContext.messageSigningContext.processedMessageLength;
    uint32_t messageLengthToProcess = unreadDataLength > remainingMessageLength ? remainingMessageLength : unreadDataLength;
    if (P2_NOT_HASHED == p2)
    {

        cx_hash((cx_hash_t *)&sha3, 0, workBufferPtr, messageLengthToProcess, NULL, 0);
    }

    appContext.messageSigningContext.processedMessageLength += messageLengthToProcess;

    if (appContext.messageSigningContext.processedMessageLength == appContext.messageSigningContext.signMessageLength)
    {
        if (P2_NOT_HASHED == p2)
        {
            cx_hash((cx_hash_t *)&sha3, CX_LAST, NULL, 0, appContext.messageSigningContext.hash, sizeof(appContext.messageSigningContext.hash));
        }
        else
        {
            os_memmove(appContext.messageSigningContext.hash, workBufferPtr, sizeof(appContext.messageSigningContext.hash));
        }

        setSignMessageDisplayData();

        uxSignFlowInit();

        *flags |= IO_ASYNCH_REPLY;
    }
    else
    {
        THROW(SW_OK);
    }
}
