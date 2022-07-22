#pragma once

#include "shared_context.h"
#include <ux_flow_engine.h>

extern const ux_flow_step_t *const uxDisplayPublicFlow[];
extern const ux_flow_step_t *const uxSignFlow[];
extern const ux_flow_step_t *const uxBaseTxFlow[];
extern const ux_flow_step_t *const uxTxFlow[];

void uiMenuAbout(void);
void uiMenuMain(void);
