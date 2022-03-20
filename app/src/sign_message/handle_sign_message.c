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

    validateParameters(p1, p2);

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
    uint32_t remainingMessageLength = getRemainingMessageLength();
    if (remainingMessageLength != 0)
    {
        uint32_t messageLengthToProcess = unreadDataLength > remainingMessageLength ? remainingMessageLength : unreadDataLength;
        if (P2_NOT_HASHED == p2)
        {
            cx_hash((cx_hash_t *)&sha3, 0, workBufferPtr, messageLengthToProcess, NULL, 0);
        }

        appContext.messageSigningContext.processedMessageLength += messageLengthToProcess;
        if (0 == getRemainingMessageLength())
        {
            setMessageHash(p2, workBufferPtr);
        }
        workBufferPtr += messageLengthToProcess;
        unreadDataLength -= messageLengthToProcess;
    }
    if ((TX == appContext.messageSigningContext.signingType) || (BASE_TX == appContext.messageSigningContext.signingType))
    {
        if ((unreadDataLength != 0) && (0 == appContext.messageSigningContext.amountLength))
        {

            appContext.messageSigningContext.amountLength = U4BE(workBufferPtr, 0);
            workBufferPtr += PARAMETER_LENGTH_BYTES;
            unreadDataLength -= PARAMETER_LENGTH_BYTES;
        }
        if (unreadDataLength != 0)
        {
            setAmount(&workBufferPtr, &unreadDataLength);
        }
        if ((unreadDataLength != 0) && (BASE_TX == appContext.messageSigningContext.signingType))
        {
            setAddress(workBufferPtr, unreadDataLength);
        }
    }
    if (isFinalRequest())
    {
        setSignMessageDisplayData();

        uxSignFlowInit();

        *flags |= IO_ASYNCH_REPLY;
    }
    else
    {
        THROW(SW_OK);
    }
}
