#include "shared_context.h"
#include "ui_callbacks.h"

#ifdef HAVE_UX_FLOW

UX_FLOW_DEF_NOCB(
    ux_idle_flow_1_step,
    nn,
    {
        "Application",
        "is ready",
    });
UX_FLOW_DEF_NOCB(
    ux_idle_flow_2_step,
    bn,
    {
        "Version",
        APPVERSION,
    });
UX_FLOW_DEF_VALID(
    ux_idle_flow_3_step,
    pb,
    os_sched_exit(-1),
    {
        &C_icon_dashboard_x,
        "Quit",
    });
const ux_flow_step_t *const ux_idle_flow[] = {
    &ux_idle_flow_1_step,
    &ux_idle_flow_2_step,
    &ux_idle_flow_3_step,
    FLOW_END_STEP,
};

#endif
