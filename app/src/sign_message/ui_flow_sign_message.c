#include "shared_context.h"
#include "ui_callbacks.h"

UX_STEP_NOCB(ux_sign_flow_1_step, pnn,
             {
                 &C_icon_certificate,
                 "Sign",
                 tmpCtx.messageSigningContext.signingTypeText,
             });
UX_STEP_NOCB(ux_sign_flow_2_step, bnnn_paging,
             {
                 .title = "Message hash",
                 .text = strings.tmp.tmp,
             });
UX_STEP_CB(ux_sign_flow_3_step, pbb, io_seproxyhal_touch_sign_message_ok(NULL),
           {
               &C_icon_validate_14,
               "Sign",
               "message",
           });
UX_STEP_CB(ux_sign_flow_4_step, pbb, io_seproxyhal_touch_sign_message_cancel(NULL),
           {
               &C_icon_crossmark,
               "Cancel",
               "signature",
           });

const ux_flow_step_t *const ux_sign_flow[5] = {
    &ux_sign_flow_1_step, &ux_sign_flow_2_step, &ux_sign_flow_3_step, &ux_sign_flow_4_step, FLOW_END_STEP,
};
