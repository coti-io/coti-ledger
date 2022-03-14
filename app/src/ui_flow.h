#pragma once

#include "shared_context.h"
#include <ux_flow_engine.h>

extern const ux_flow_step_t *const ux_idle_flow[];
extern const ux_flow_step_t *const ux_display_public_flow[];
extern const ux_flow_step_t *const ux_sign_flow[];

void uiIdle(void);
