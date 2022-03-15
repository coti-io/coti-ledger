#include "shared_context.h"
#include "ui_callbacks.h"

#ifdef HAVE_UX_FLOW

UX_STEP_NOCB(ux_idle_flow_1_step, nn,
             {
                 "Application",
                 "is ready",
             });
UX_STEP_NOCB(ux_idle_flow_2_step, bn,
             {
                 "Version",
                 APPVERSION,
             });
UX_STEP_CB(ux_idle_flow_3_step, pb, os_sched_exit(-1),
           {
               &C_icon_dashboard_x,
               "Quit",
           });
const ux_flow_step_t *const uxIdleFlow[4] = {
    &ux_idle_flow_1_step,
    &ux_idle_flow_2_step,
    &ux_idle_flow_3_step,
    FLOW_END_STEP,
};

#endif

void uiIdle(void)
{
#if defined(HAVE_UX_FLOW)
    // reserve a display stack slot if none yet
    if (0 == G_ux.stack_count)
    {
        ux_stack_push();
    }
    ux_flow_init(0, uxIdleFlow, NULL);
#endif // #if defined(HAVE_UX_FLOW)
}
