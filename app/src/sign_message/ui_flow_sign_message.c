#include "shared_context.h"
#include "ui_callbacks.h"

UX_STEP_NOCB(uxSignInitialStep, pnn,
             {
                 &C_icon_certificate,
                 "Sign",
                 displayData.signMessageDisplayData.signingTypeText,
             });
UX_STEP_NOCB(uxSignMessageDataStep, paging,
             {
                 .title = "Message hash",
                 .text = displayData.signMessageDisplayData.message,
             });
UX_STEP_NOCB(uxSignAmountDataStep, paging,
             {
                 .title = "Amount",
                 .text = displayData.signMessageDisplayData.amount,
             });
UX_STEP_NOCB(uxSignAddressDataStep, paging,
             {
                 .title = "From Address",
                 .text = displayData.signMessageDisplayData.address,
             });
UX_STEP_NOCB(uxSignDestinationAddressDataStep, paging,
             {
                 .title = "To Address",
                 .text = displayData.signMessageDisplayData.address,
             });
UX_STEP_CB(uxSignMessageOkStep, pbb, io_seproxyhal_touch_sign_message_ok(NULL),
           {
               &C_icon_validate_14,
               "Sign",
               "message",
           });
UX_STEP_CB(uxSignMessageCancelStep, pbb, io_seproxyhal_touch_sign_message_cancel(NULL),
           {
               &C_icon_crossmark,
               "Cancel",
               "signature",
           });

const ux_flow_step_t *const uxSignFlow[5] = {
    &uxSignInitialStep, &uxSignMessageDataStep, &uxSignMessageOkStep, &uxSignMessageCancelStep, FLOW_END_STEP,
};

const ux_flow_step_t *const uxBaseTxFlow[6] = {
    &uxSignInitialStep, &uxSignAmountDataStep, &uxSignAddressDataStep, &uxSignMessageOkStep, &uxSignMessageCancelStep, FLOW_END_STEP,
};

const ux_flow_step_t *const uxTxFlow[6] = {
    &uxSignInitialStep, &uxSignAmountDataStep, &uxSignDestinationAddressDataStep, &uxSignMessageOkStep, &uxSignMessageCancelStep, FLOW_END_STEP,
};
