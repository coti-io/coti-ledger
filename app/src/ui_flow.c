#include "shared_context.h"
#include "ui_callbacks.h"
#include "ui_flow.h"

#ifdef HAVE_UX_FLOW

UX_STEP_NOCB(uxMenuInfoStep, bn, {"Coti Ledger App", "(c) 2022 Coti"});
UX_STEP_CB(uxMenuBackStep, pb, uiMenuAbout(), {&C_icon_back, "Back"});

UX_FLOW(uxMenuAboutFlow, &uxMenuInfoStep, &uxMenuBackStep, FLOW_LOOP);

void uiMenuAbout(void)
{
    ux_flow_init(0, uxMenuAboutFlow, NULL);
}

UX_STEP_NOCB(uxMenuReadyStep, nn,
             {
                 "Application",
                 "is ready",
             });
UX_STEP_NOCB(uxMenuVersionStep, bn,
             {
                 "Version",
                 APPVERSION,
             });
UX_STEP_CB(uxMenuAboutStep, pb, uiMenuAbout(), {&C_icon_certificate, "About"});

UX_STEP_VALID(uxMenuExitStep, pb, os_sched_exit(-1),
              {
                  &C_icon_dashboard_x,
                  "Quit",
              });
UX_FLOW(uxMenuMainFlow, &uxMenuReadyStep, &uxMenuVersionStep, &uxMenuAboutStep, &uxMenuExitStep, FLOW_LOOP);

#endif

void uiMenuMain(void)
{
#if defined(HAVE_UX_FLOW)
    // reserve a display stack slot if none yet
    if (0 == G_ux.stack_count)
    {
        ux_stack_push();
    }
    ux_flow_init(0, uxMenuMainFlow, NULL);
#endif // #if defined(HAVE_UX_FLOW)
}
