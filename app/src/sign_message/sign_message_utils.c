#include "sign_message_utils.h"
#include "shared_context.h"
#ifdef HAVE_UX_FLOW
#include "ui_flow.h"
#endif
#include "sign_message_constants.h"
#include "apdu_constants.h"

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
