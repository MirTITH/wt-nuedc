#pragma once

#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t kFastTimCallbackDuration;
extern uint32_t kFastTimCallbackCount;

void FastTimCallback();

#ifdef __cplusplus
}
#endif
