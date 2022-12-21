#include "shared_context.h"
#include "ui_callbacks.h"

#ifdef HAVE_UX_FLOW

UX_STEP_NOCB(uxDisplayPublicInitialStep, pnn,
             {
                 &C_icon_eye,
                 "Verify",
                 "Public Key",
             });
UX_STEP_NOCB(uxDisplayPublicDataStep, paging,
             {
                 .title = "Public Key",
                 .text = displayData.publicKeyDisplayData.publicKey,
             });
UX_STEP_CB(uxDisplayPublicApproveStep, pb, io_seproxyhal_touch_address_ok(NULL),
           {
               &C_icon_validate_14,
               "Approve",
           });
UX_STEP_CB(uxDisplayPublicRejectstep, pb, io_seproxyhal_touch_address_cancel(NULL),
           {
               &C_icon_crossmark,
               "Reject",
           });

const ux_flow_step_t *const uxDisplayPublicFlow[5] = {
    &uxDisplayPublicInitialStep, &uxDisplayPublicDataStep, &uxDisplayPublicApproveStep, &uxDisplayPublicRejectstep, FLOW_END_STEP,
};

#endif
