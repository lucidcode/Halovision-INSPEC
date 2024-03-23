#pragma once

#include <stdbool.h>

#include "broadcom/defines.h"

void BP_EnableIRQs(void);
void BP_DisableIRQs(void);

// Minimum priority is the highest priority value. (0 is highest priority)
void BP_SetMinPriority(uint8_t priority);

// We mimic the NVIC used in Cortex M SoCs.
void BP_EnableIRQ(IRQn_Type IRQn);
bool BP_GetEnableIRQ(IRQn_Type IRQn);
void BP_DisableIRQ(IRQn_Type IRQn);
bool BP_GetPendingIRQ(IRQn_Type IRQn);
void BP_SetPendingIRQ(IRQn_Type IRQn);
void BP_ClearPendingIRQ(IRQn_Type IRQn);
bool BP_GetActive(IRQn_Type IRQn);
void BP_SetPriority(IRQn_Type IRQn, uint8_t priority);
uint8_t BP_GetPriority(IRQn_Type IRQn);
