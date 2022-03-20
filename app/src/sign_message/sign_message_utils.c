#include "sign_message_utils.h"
#include "shared_context.h"
#ifdef HAVE_UX_FLOW
#include "ui_flow.h"
#endif
#include "sign_message_constants.h"
#include "apdu_constants.h"

void validateParameters(uint8_t p1, uint8_t p2)
{
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
}

void setPathLength(const uint8_t **workBufferPtr, uint16_t *unreadDataLength)
{
    appContext.messageSigningContext.pathLength = (*workBufferPtr)[0];
    if ((appContext.messageSigningContext.pathLength < 0x01) || (appContext.messageSigningContext.pathLength > MAX_BIP32_PATH))
    {
        PRINTF("Invalid path\n");
        THROW(SW_INVALID_PATH);
    }

    *workBufferPtr += PATH_LENGTH_BYTES;
    *unreadDataLength -= PATH_LENGTH_BYTES;
}

void setMessageLength(const uint8_t **workBufferPtr, uint16_t *unreadDataLength)
{
    appContext.messageSigningContext.signMessageLength = U4BE(*workBufferPtr, 0);
    *workBufferPtr += PARAMETER_LENGTH_BYTES;
    *unreadDataLength -= PARAMETER_LENGTH_BYTES;
}

void setSigningType(const uint8_t **workBufferPtr, uint16_t *unreadDataLength)
{
    appContext.messageSigningContext.signingType = (*workBufferPtr)[0];
    *workBufferPtr += SIGNATURE_TYPE_BYTES;
    *unreadDataLength -= SIGNATURE_TYPE_BYTES;
}

void setBip32Path(const uint8_t **workBufferPtr, uint16_t *unreadDataLength)
{
    for (uint32_t i = 0; i < appContext.messageSigningContext.pathLength; ++i)
    {
        appContext.messageSigningContext.bip32Path[i] = U4BE(*workBufferPtr, 0);
        *workBufferPtr += PATH_PARAMETER_BYTES;
        *unreadDataLength -= PATH_PARAMETER_BYTES;
    }
}

void setMessageHash(uint8_t p2, const uint8_t *workBufferPtr)
{
    if (P2_NOT_HASHED == p2)
    {
        cx_hash((cx_hash_t *)&sha3, CX_LAST, NULL, 0, appContext.messageSigningContext.hash, sizeof(appContext.messageSigningContext.hash));
    }
    else
    {
        os_memmove(appContext.messageSigningContext.hash, workBufferPtr, sizeof(appContext.messageSigningContext.hash));
    }
}

uint32_t getRemainingMessageLength(void)
{
    return appContext.messageSigningContext.signMessageLength - appContext.messageSigningContext.processedMessageLength;
}

bool isFinalRequest(void)
{
    bool isFinalRequest;
    switch (appContext.messageSigningContext.signingType)
    {
    case BASE_TX:
        isFinalRequest = 0 == getRemainingAddressLength();
        break;
    case TX:
        isFinalRequest = 0 == getRemainingAmountLength();
        break;
    default:
        isFinalRequest = appContext.messageSigningContext.processedMessageLength == appContext.messageSigningContext.signMessageLength;
    }
    return isFinalRequest;
}

uint32_t getProcessedAmountLength(void)
{
    return (uint32_t)strlen((char *)appContext.messageSigningContext.amount);
}

uint32_t getRemainingAmountLength(void)
{
    return appContext.messageSigningContext.amountLength - getProcessedAmountLength();
}

void setAmount(const uint8_t **workBufferPtr, uint16_t *unreadDataLength)
{
    uint32_t processedAmountLength = getProcessedAmountLength();
    uint32_t remainingAmountLength = getRemainingAmountLength();
    if (remainingAmountLength != 0)
    {
        uint32_t amountLengthToProcess = *unreadDataLength > remainingAmountLength ? remainingAmountLength : *unreadDataLength;
        os_memmove(&displayData.signMessageDisplayData.amount[processedAmountLength], *workBufferPtr, amountLengthToProcess);
        *workBufferPtr += amountLengthToProcess;
        *unreadDataLength -= amountLengthToProcess;
    }
}

uint32_t getProcessedAddressLength(void)
{
    return (uint32_t)strlen((char *)appContext.messageSigningContext.address);
}

uint32_t getRemainingAddressLength(void)
{
    return ADDRESS_LENGTH - getProcessedAddressLength();
}

void setAddress(const uint8_t *workBufferPtr, uint16_t unreadDataLength)
{
    uint32_t processedAddressLength = getProcessedAddressLength();
    uint32_t remainingAddressLength = getRemainingAddressLength();
    if (remainingAddressLength != 0)
    {
        uint32_t addressLengthToProcess = unreadDataLength > remainingAddressLength ? remainingAddressLength : unreadDataLength;
        os_memmove(&displayData.signMessageDisplayData.address[processedAddressLength], workBufferPtr, addressLengthToProcess);
    }
}

void setSignMessageDisplayData(void)
{
    arrayHexstr(displayData.signMessageDisplayData.message, &appContext.messageSigningContext.hash, sizeof(appContext.messageSigningContext.hash));

    os_memmove(displayData.signMessageDisplayData.signingTypeText, signing_type_texts[appContext.messageSigningContext.signingType],
               sizeof(displayData.signMessageDisplayData.signingTypeText));
}

void uxSignFlowInit(void)
{
    switch (appContext.messageSigningContext.signingType)
    {
    case BASE_TX:
        ux_flow_init(0, uxBaseTxFlow, NULL);
        break;
    case TX:
        ux_flow_init(0, uxTxFlow, NULL);
        break;
    default:
        ux_flow_init(0, uxSignFlow, NULL);
        break;
    }
}
