#include "shared_context.h"
#include "ui_callbacks.h"

UX_STEP_NOCB(uxSignFlowInitial, pnn,
             {
                 &C_icon_certificate,
                 "Sign",
                 displayData.signMessageDisplayData.signingTypeText,
             });
UX_STEP_NOCB(uxSignMessageData, paging,
             {
                 .title = "Message hash",
                 .text = displayData.signMessageDisplayData.message,
             });
UX_STEP_NOCB(uxSignAmountData, paging,
             {
                 .title = "Amount",
                 .text = displayData.signMessageDisplayData.amount,
             });
UX_STEP_NOCB(uxSignAddressData, paging,
             {
                 .title = "From Address",
                 .text = displayData.signMessageDisplayData.address,
             });
UX_STEP_CB(uxSignMessageOk, pbb, io_seproxyhal_touch_sign_message_ok(NULL),
           {
               &C_icon_validate_14,
               "Sign",
               "message",
           });
UX_STEP_CB(uxSignMessageCancel, pbb, io_seproxyhal_touch_sign_message_cancel(NULL),
           {
               &C_icon_crossmark,
               "Cancel",
               "signature",
           });

const ux_flow_step_t *const uxSignFlow[5] = {
    &uxSignFlowInitial, &uxSignMessageData, &uxSignMessageOk, &uxSignMessageCancel, FLOW_END_STEP,
};

const ux_flow_step_t *const uxBaseTxFlow[6] = {
    &uxSignFlowInitial, &uxSignAmountData, &uxSignAddressData, &uxSignMessageOk, &uxSignMessageCancel, FLOW_END_STEP,
};

const ux_flow_step_t *const uxTxFlow[5] = {
    &uxSignFlowInitial, &uxSignAmountData, &uxSignMessageOk, &uxSignMessageCancel, FLOW_END_STEP,
};
