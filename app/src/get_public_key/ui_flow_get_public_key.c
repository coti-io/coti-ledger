#include "shared_context.h"
#include "ui_callbacks.h"

#ifdef HAVE_UX_FLOW

UX_STEP_NOCB(ux_display_public_flow_1_step, pnn,
             {
                 &C_icon_eye,
                 "Verify",
                 "Public Key",
             });
UX_STEP_NOCB(ux_display_public_flow_2_step, paging,
             {
                 .title = "Public Key",
                 .text = displayData.publicKeyDisplayData.publicKey,
             });
UX_STEP_CB(ux_display_public_flow_3_step, pb, io_seproxyhal_touch_address_ok(NULL),
           {
               &C_icon_validate_14,
               "Approve",
           });
UX_STEP_CB(ux_display_public_flow_4_step, pb, io_seproxyhal_touch_address_cancel(NULL),
           {
               &C_icon_crossmark,
               "Reject",
           });

const ux_flow_step_t *const uxDisplayPublicFlow[5] = {
    &ux_display_public_flow_1_step, &ux_display_public_flow_2_step, &ux_display_public_flow_3_step, &ux_display_public_flow_4_step, FLOW_END_STEP,
};

#endif
