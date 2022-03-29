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

    if (0 == unreadDataLength)
    {
        PRINTF("Invalid data\n");
        THROW(SW_INVALID_DATA);
    }

    if (P1_FIRST == p1)
    {
        if (appState != APP_STATE_IDLE)
        {
            resetAppContext();
        }

        appState = APP_STATE_SIGNING_MESSAGE;

        setPathLength(&workBufferPtr, &unreadDataLength);
        uint8_t minimalFirstRequestByteNumber =
            PATH_LENGTH_BYTES + (PATH_PARAMETER_BYTES * appContext.messageSigningContext.pathLength) + SIGNATURE_TYPE_BYTES + PARAMETER_LENGTH_BYTES;
        if (dataLength < minimalFirstRequestByteNumber)
        {
            PRINTF("Invalid data\n");
            THROW(SW_INVALID_DATA);
        }

        setBip32Path(&workBufferPtr, &unreadDataLength);

        setSigningType(&workBufferPtr, &unreadDataLength);

        setMessageLength(&workBufferPtr, &unreadDataLength);

        if ((TX == appContext.messageSigningContext.signingType) || (BASE_TX == appContext.messageSigningContext.signingType))
        {
            if (0 == unreadDataLength)
            {
                PRINTF("Invalid data\n");
                THROW(SW_INVALID_DATA);
            }
            setAmountLength(&workBufferPtr, &unreadDataLength);
        }
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

    setMessage(p2, &workBufferPtr, &unreadDataLength);

    if ((TX == appContext.messageSigningContext.signingType) || (BASE_TX == appContext.messageSigningContext.signingType))
    {
        setAmount(&workBufferPtr, &unreadDataLength);
        setAddress(workBufferPtr, unreadDataLength);
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
