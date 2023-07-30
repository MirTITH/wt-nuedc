#pragma once

#include "lvgl/lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

void StartLvglThread();
void LvglLock();
void LvglUnlock();

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#include "FreeRTOS.h"
#include "task.h"
#include "freertos_lock/freertos_lock.hpp"
#include <mutex>

extern freertos_lock::RecursiveMutex LvglMutex;
#define LvglLockGuard() std::lock_guard<freertos_lock::RecursiveMutex> LvglThreadLock(LvglMutex)

#endif
