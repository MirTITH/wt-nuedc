#pragma once

#include <cstdint>
#include <atomic>

extern uint32_t kFastTimCallbackDuration;
extern uint32_t kFastTimCallbackCount;

void FastTimCallback();

extern std::atomic<float> kMod;
